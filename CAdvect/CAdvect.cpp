#include <array>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <yaml-cpp/yaml.h>
#include "Fields.h"
#include "Particles.h"

int main(int argc, char* argv[]) {

//  loop through args looking for the input file flag -i and assign to config file
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
        
        
    

    // timestep loop

    // load data

    // advect
}