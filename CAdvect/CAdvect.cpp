#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <netcdf>
#include <yaml-cpp/yaml.h>
#include "Fields.h"
#include "Particles.h"

// functions declared after main
void write_to_output (const std::vector<Particle>& particles, netCDF::NcFile& output_file,
                        netCDF::NcVar& time_var, netCDF::NcVar& xpos_var, netCDF::NcVar& ypos_var, int step, float time);
std::array<double, 2> bilinear(const Particle& particle);
void forward_euler (Particle& particle, const std::array<double, 2>& particle_uv, const float dt,
                    const std::vector<double>& x_edges, const std::vector<double>& y_edges);

int main(int argc, char* argv[]) {

//  determine config file
    std::filesystem::path filepath;
    YAML::Node config;
    bool found_input = false;

    for (int i = 1; i < argc; ++i){
        auto flag = argv[i];
        if (flag[0] == '-' && i + 1 < argc){
            filepath = argv[i + 1];

            if (flag[1] == 'i'){
                std::cout << "Reading from config file: " << filepath << "\n";

                if (!std::filesystem::exists(filepath)) {
                    throw std::runtime_error("Config file does not exist!");
                 }

                found_input = true;
                config = YAML::LoadFile(filepath);
            }

        }
    }
    if (!found_input) {throw std::runtime_error("WARNING: No config file found!");}


    // load field data
    std::cout << "Initialising u, v fields...\n";
    const int nx = config["Field"]["Nx"].as<int>();
    const int ny = config["Field"]["Ny"].as<int>();
    Field field = single_gyre(nx, ny,
                              config["Field"]["Mag"].as<int>(),
                              Direction::CounterClockwise);
    const std::vector<double> x_edges = generate_gridpoints (config["Field"]["Xbounds"][0].as<double>(),
                                                             config["Field"]["Xbounds"][1].as<double>(), nx, GridType::Edges);
    const std::vector<double> x_centres = generate_gridpoints (config["Field"]["Xbounds"][0].as<double>(),
                                                               config["Field"]["Xbounds"][1].as<double>(), nx, GridType::Centres);
    const std::vector<double> y_edges = generate_gridpoints (config["Field"]["Ybounds"][0].as<double>(),
                                                             config["Field"]["Ybounds"][1].as<double>(), ny, GridType::Edges);
    const std::vector<double> y_centres = generate_gridpoints (config["Field"]["Ybounds"][0].as<double>(),
                                                              config["Field"]["Ybounds"][1].as<double>(), ny, GridType::Centres);


    // initialise particles
    std::cout << "Initialising particles... ";
    std::vector<Particle> particles = from_line( config["Particles"]["Point_a"].as<std::vector<double>>(),
                                                  config["Particles"]["Point_b"].as<std::vector<double>>(),
                                                        config["Particles"]["N"].as<int>());
    std::cout << "Initialised " << particles.size() << " particles:\n";


    // initialise netcdf output file, use "try" to handle scope
    const std::filesystem::path outpath = config["Output"]["Path"].as<std::string>();
    try {
        netCDF::NcFile ncFile(outpath, netCDF::NcFile::replace);

        // Create netCDF dimensions, time is unlimited dimension
        netCDF::NcDim xDim = ncFile.addDim("x", nx);
        netCDF::NcDim yDim = ncFile.addDim("y", ny);
        netCDF::NcDim tDim = ncFile.addDim("time");
        netCDF::NcDim partDim = ncFile.addDim("particle", particles.size());

        // create and write coordinates
        netCDF::NcVar xVar = ncFile.addVar("x", netCDF::ncFloat, xDim);
        netCDF::NcVar yVar = ncFile.addVar("y", netCDF::ncFloat, yDim);
        netCDF::NcVar tVar = ncFile.addVar("time", netCDF::ncFloat, tDim);
        netCDF::NcVar partVar = ncFile.addVar("particle", netCDF::ncInt, partDim);

        //WRITE NOT YET IMPLEMENTED BECAUSE I DONT HAVE CELL CENTRES
        xVar.putVar(x_centres.data());
        yVar.putVar(y_centres.data());

        // generate and write particle ids
        std::vector<int> part_ids(particles.size());
        for ( int i = 0; i < particles.size(); i++ ) {
            part_ids[i] = i + 1;
        }
        partVar.putVar(part_ids.data());


        // Write the U and V fields.
        std::vector<netCDF::NcDim> dims;
        dims.push_back(xDim);
        dims.push_back(yDim);
        netCDF::NcVar u = ncFile.addVar("u", netCDF::ncDouble, dims);
        u.putVar(field.u.data());
        netCDF::NcVar v = ncFile.addVar("v", netCDF::ncDouble, dims);
        v.putVar(field.v.data());

        // add the fields for particle position data
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

    /*
    Enter the timestepping loop. begin by opening the output file in write mode and keep it open until
    the end. flush it periodically with .sync()

    Begin by writing
    */
    // 1. write positions
    // 2. update positions
    // 3. increment

    const int max_time = config["Integration"]["Total"].as<int>();
    const float dt = config["Integration"]["dT"].as<float>();

    netCDF::NcFile output_file(outpath, netCDF::NcFile::write);
    netCDF::NcVar time_var = output_file.getVar("time");
    netCDF::NcVar xpos_var = output_file.getVar("x_pos");
    netCDF::NcVar ypos_var = output_file.getVar("y_pos");

    std::cout << "Beginning iteration. Max: " << max_time << "\tdt: "<< dt << "\n";
    for ( int step = 0; step <= max_time; step++ ){
        const float time = step * dt;
        write_to_output(particles, output_file, time_var, xpos_var, ypos_var, step, time);
        if ( step % 10 == 0 ) {
            std::cout << "iterating step " << step << "\n";
            output_file.sync();
        }

        // step through particles once. First interpolate the u, v components then advect
        for ( auto& particle : particles ) {
            std::array<double, 2> particle_uv = bilinear(particle) ;
            //forward_euler(particle, particle_uv, dt, x_edges, y_edges);
        }
    }


}

void write_to_output (const std::vector<Particle>& particles,
                      netCDF::NcFile& output_file,
                      netCDF::NcVar& time_var,
                      netCDF::NcVar& xpos_var,
                      netCDF::NcVar& ypos_var,
                      int step, float time) {

    // save time coord
    time_var.putVar(std::vector<size_t>{static_cast<size_t>(step)}, time);
    // putVar needs the array indexing to be a vector of type size_t, that's why i have to create the vector and static_cast the ints

    // save xy positions per particle;
    for (int i = 0; i < particles.size(); i++) {
       xpos_var.putVar(std::vector<size_t>{static_cast<size_t>(step), static_cast<size_t>(i)}, particles[i].x);
       ypos_var.putVar(std::vector<size_t>{static_cast<size_t>(step), static_cast<size_t>(i)}, particles[i].y);
    }
}

std::array<double, 2> bilinear(const Particle& particle) {

}


void forward_euler (Particle& particle, const std::array<double, 2>& particle_uv, const float dt, const std::vector<double>& x_edges, const std::vector<double>& y_edges) {
    // simple forward_euler advection scheme with boundary reflection
    // assumes single reflection is enough
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

    particle.x = x2;
    particle.y = y2;
}
