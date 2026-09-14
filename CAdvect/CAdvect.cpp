#include <array>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <netcdf>
#include <yaml-cpp/yaml.h>
#include "Fields.h"
#include "Particles.h"

// FORWARD DECLARATIONS
// find cell index in sorted array
size_t find_cell_index(const double value, const std::vector<double>& arr);
// bilinear interpolation of u and v fields
std::array<double, 2> bilinear_uv(const Particle& particle, const Field& field,
                                  const std::vector<double>& x_edges, const std::vector<double>& y_edges);
// simple forward euler time step with reflecting BCs
void forward_euler (Particle& particle, const std::array<double, 2>& particle_uv, const float dt,
                    const std::vector<double>& x_edges, const std::vector<double>& y_edges);
// write to netcdf
void write_to_output (const std::vector<Particle>& particles, netCDF::NcFile& output_file,
                      netCDF::NcVar& time_var, netCDF::NcVar& xpos_var, netCDF::NcVar& ypos_var, int step, float time);


int main(int argc, char* argv[]) {
// particle advection solver, configured by config file. Overview:
// 1. read config file
// 2. initialise u, v fields
// 3. initialise particles
// 4. initialise (netcdf) output file
// 5. enter time stepping loo


//  CONFIG FILE
    std::filesystem::path filepath;
    YAML::Node config;
    bool found_input = false;

    // loop input arguments looking for '-i'
    for (int i = 1; i < argc; ++i){
        auto flag = argv[i];
        // if input arg begins with '-'
        if (flag[0] == '-' && i + 1 < argc){
            // if input arg is '-i'
            if (flag[1] == 'i'){
                // take next arg and look for file
                filepath = argv[i + 1];
                std::cout << "Reading from config file: " << filepath << "\n";

                // check file exists, exit if not
                if (!std::filesystem::exists(filepath)) {
                    throw std::runtime_error("Config file does not exist!");
                 }

                found_input = true;
                // load yaml
                config = YAML::LoadFile(filepath);
            }

        }
    }
    // exit if no input file found
    if (!found_input) {throw std::runtime_error("WARNING: No config file found!");}


    // INITIALISE FIELDS
    // Counter Clockwise hardcoded
    std::cout << "Initialising u, v fields...\n";
    const int nx = config["Field"]["Nx"].as<int>();
    const int ny = config["Field"]["Ny"].as<int>();
    // initialise on cell edgues by passing nx + 1, ny + 1
    Field field = single_gyre(nx + 1, ny + 1,
                              config["Field"]["Mag"].as<double>(),
                              Direction::CounterClockwise);
    // create cell edge data
    const std::vector<double> x_edges = generate_gridpoints (config["Field"]["Xbounds"][0].as<double>(),
                                                             config["Field"]["Xbounds"][1].as<double>(), nx, GridType::Edges);
    const std::vector<double> y_edges = generate_gridpoints (config["Field"]["Ybounds"][0].as<double>(),
                                                             config["Field"]["Ybounds"][1].as<double>(), ny, GridType::Edges);


    // INITIALISE PARTICLES
    std::cout << "Initialising particles... ";
    std::vector<Particle> particles = from_line( config["Particles"]["Point_a"].as<std::vector<double>>(),
                                                   config["Particles"]["Point_b"].as<std::vector<double>>(),
                                                         config["Particles"]["N"].as<int>());
    std::cout << "Initialised " << particles.size() << " particles:\n";


    // INITIALISE NETCDF OUTPUT
    // try clause used to handle scope of variables
    const std::filesystem::path outpath = config["Output"]["Path"].as<std::string>();
    try {
        // Create file object
        netCDF::NcFile ncFile(outpath, netCDF::NcFile::replace);

        // Create netCDF dimensions, time is unlimited dimension
        netCDF::NcDim xDim = ncFile.addDim("x", nx + 1);
        netCDF::NcDim yDim = ncFile.addDim("y", ny + 1);
        netCDF::NcDim tDim = ncFile.addDim("time");
        netCDF::NcDim partDim = ncFile.addDim("particle", particles.size());

        // create coordinates
        netCDF::NcVar xVar = ncFile.addVar("x", netCDF::ncFloat, xDim);
        netCDF::NcVar yVar = ncFile.addVar("y", netCDF::ncFloat, yDim);
        netCDF::NcVar tVar = ncFile.addVar("time", netCDF::ncFloat, tDim);
        netCDF::NcVar partVar = ncFile.addVar("particle", netCDF::ncInt, partDim);

        //write coordinates of u, v components
        xVar.putVar(x_edges.data());
        yVar.putVar(y_edges.data());

        // generate and particle ids
        std::vector<int> part_ids(particles.size());
        for ( int i = 0; i < particles.size(); i++ ) {
            part_ids[i] = i + 1;
        }
        // write particle ids as particle coordinate
        partVar.putVar(part_ids.data());

        // Write the U and V field data variables.
        std::vector<netCDF::NcDim> dims;
        dims.push_back(xDim);
        dims.push_back(yDim);

        netCDF::NcVar u = ncFile.addVar("u", netCDF::ncDouble, dims);
        netCDF::NcVar v = ncFile.addVar("v", netCDF::ncDouble, dims);

        u.putVar(field.u.data());
        v.putVar(field.v.data());

        // add empty variables for particle position data
        dims[1] = partDim;
        dims[0] = tDim;

        netCDF::NcVar x_pos = ncFile.addVar("x_pos", netCDF::ncDouble, dims);
        netCDF::NcVar y_pos = ncFile.addVar("y_pos", netCDF::ncDouble, dims);

    } catch(netCDF::exceptions::NcException& e) {
      // copied from docs
      std::cout<<"FAILURE**************************\n";
      std::cout << e.what() << "\n";
        return 2;
    }


    // TIMESTEP LOOP
    // 1. write particle positions (including initial positions) and time data
    // 2. loop over particles interpolating u, v field then advecting

    const int max_time = config["Integration"]["Total"].as<int>();
    const float dt = config["Integration"]["dT"].as<float>();

    // open output file and get variable objects
    netCDF::NcFile output_file(outpath, netCDF::NcFile::write);
    netCDF::NcVar time_var = output_file.getVar("time");
    netCDF::NcVar xpos_var = output_file.getVar("x_pos");
    netCDF::NcVar ypos_var = output_file.getVar("y_pos");

    // enter iteration loop
    std::cout << "Beginning iteration. Max: " << max_time << "\tdt: "<< dt << "\n";
    for ( int step = 0; step <= max_time; step++ ){
        const float time = step * dt;
        if ( step % 10 == 0 ) {
            // progress indicator
            std::cout << "iterating step " << step << "\n";
            output_file.sync();
        }

        // write output data
        write_to_output(particles, output_file, time_var, xpos_var, ypos_var, step, time);

        // enter loop over particles
        for ( auto& particle : particles ) {
            // interpolate u, v fields
            std::array<double, 2> particle_uv = bilinear_uv(particle, field, x_edges, y_edges) ;
            // apply advection kernel
            forward_euler(particle, particle_uv, dt, x_edges, y_edges);
        }
    }

}


// find left index of value in sorted array
size_t find_cell_index(const double value, const std::vector<double>& arr) {
    // return left index by applying - 1
    auto idx = std::lower_bound(arr.begin(), arr.end(), value) - arr.begin() - 1;

    // limit minimum returned index to 0
    return static_cast<size_t>(std::max(static_cast<int>(idx), 0));
}


// bilinear interpolation of u and v velocity fields
std::array<double, 2> bilinear_uv(const Particle& particle, const Field& field,
                                  const std::vector<double>& x_edges, const std::vector<double>& y_edges) {
    // find indexes of cell edges left of particle position
    size_t idx_x0 = find_cell_index(particle.x, x_edges);
    size_t idx_y0 = find_cell_index(particle.y, y_edges);

    // distances to x and y points
    double dx0 = particle.x - x_edges[idx_x0];
    double dx1 = x_edges[idx_x0 + 1] - particle.x;
    double dy0 = particle.y - y_edges[idx_y0];
    double dy1 = y_edges[idx_y0 + 1] - particle.y;

    // bilinear interpolation normalisation factor
    double norm = 1. / ((dx0 + dx1) * (dy0 + dy1));

    // points around particle labeled a, b, c, d clockwise from bottom left
    double ua = field.u[(idx_y0 * x_edges.size()) + idx_x0];
    double ub = field.u[((idx_y0 + 1) * x_edges.size()) + idx_x0];
    double uc = field.u[((idx_y0 + 1) * x_edges.size()) + idx_x0 + 1];
    double ud = field.u[(idx_y0 * x_edges.size()) + idx_x0 + 1];

    double va = field.v[(idx_y0 * x_edges.size()) + idx_x0];
    double vb = field.v[((idx_y0 + 1) * x_edges.size()) + idx_x0];
    double vc = field.v[((idx_y0 + 1) * x_edges.size()) + idx_x0 + 1];
    double vd = field.v[(idx_y0 * x_edges.size()) + idx_x0 + 1];

    // weights
    double weight_a = dx1 * dy1;
    double weight_b = dx1 * dy0;
    double weight_c = dx0 * dy0;
    double weight_d = dx0 * dy1;

    // interpolated velocities
    double particle_u = ((weight_a * ua) + (weight_b * ub) + (weight_c * uc) + (weight_d * ud)) * norm;
    double particle_v = ((weight_a * va) + (weight_b * vb) + (weight_c * vc) + (weight_d * vd)) * norm;

    return std::array<double, 2>{particle_u, particle_v};
}


// simple forward euler advection kernel applying reflecting boundary conditions
void forward_euler (Particle& particle, const std::array<double, 2>& particle_uv, const float dt, const std::vector<double>& x_edges, const std::vector<double>& y_edges) {
    // calculate particle next position
    double x2 = particle.x + dt * particle_uv[0];
    double y2 = particle.y + dt * particle_uv[1];

    // reflect at x boundaries
    if ( x2 < x_edges.front() ) {
        x2 = x_edges.front() + (x_edges.front() - x2);
    } else if ( x2 > x_edges.back() ) {
        x2 = x_edges.back() + (x_edges.back() - x2);
    }

    // reflect at y boundaries
    if ( y2 < y_edges.front() ) {
        y2 = y_edges.front() + (y_edges.front() - y2);
    } else if ( y2 > y_edges.back() ) {
        y2 = y_edges.back() + (y_edges.back() - y2);
    }

    // update particle position
    particle.x = x2;
    particle.y = y2;
}


// write time and particle positions to output netcdf file
void write_to_output (const std::vector<Particle>& particles,
                      netCDF::NcFile& output_file,
                      netCDF::NcVar& time_var,
                      netCDF::NcVar& xpos_var,
                      netCDF::NcVar& ypos_var,
                      int step, float time) {

    // save time coord
    time_var.putVar(std::vector<size_t>{static_cast<size_t>(step)}, time);
    // putVar needs the array indexing to be a vector of type size_t, that's why i have to create a vector and static_cast the ints

    // save xy positions per particle;
    for (int i = 0; i < particles.size(); i++) {
       xpos_var.putVar(std::vector<size_t>{static_cast<size_t>(step), static_cast<size_t>(i)}, particles[i].x);
       ypos_var.putVar(std::vector<size_t>{static_cast<size_t>(step), static_cast<size_t>(i)}, particles[i].y);
    }
}