#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <netcdf>
#include <yaml-cpp/yaml.h>
#include "Fields.h"
#include "Particles.h"


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
                std::cout << "test? " << config["Test"] << "\n";
            }

        }
    }
    if (!found_input) {throw std::runtime_error("WARNING: No config file found!");}


    // load field data
    std::cout << "Initialising u, v fields...\n";
    int nx = config["Field"]["Nx"].as<int>();
    int ny = config["Field"]["Ny"].as<int>();
    Field field = single_gyre(nx, ny,
                              config["Field"]["Mag"].as<int>(),
                              Direction::CounterClockwise);


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
        netCDF::NcDim partDim = ncFile.addDim("particle");

        // create and write coordinates
        netCDF::NcVar xVar = ncFile.addVar("x", netCDF::ncFloat, xDim);
        netCDF::NcVar yVar = ncFile.addVar("y", netCDF::ncFloat, yDim);
        netCDF::NcVar tVar = ncFile.addVar("time", netCDF::ncFloat, tDim);
        netCDF::NcVar partVar = ncFile.addVar("particle", netCDF::ncInt, partDim);

        //WRITE NOT YET IMPLEMENTED BECAUSE I DONT HAVE CELL CENTRES
        //xVar.putVar(x_centres);
        //yVar.putVar(y_centres);

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

        return 0;
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

    const int T


}

void write_particle_positions (const std::vector<Particle>& particles ) {}
