#include <iostream>
#include <filesystem>
#include "Fields.h"
#include "Particles.h"

int main(int argc, char* argv[]) {

//  loop through args looking for the input file flag -i and assign to path
    std::filesystem::path filepath;
    bool found_input = false;

    std::cout << "Looking for input file..." << "\n";
    for (int i = 1; i < argc; ++i){
        auto flag = argv[i];
        if (flag[0] == '-' && i + 1 < argc){
            auto val = argv[i + 1];

            if (flag[1] == 'i'){
                std::cout << "Found it: " << val << " !\n";
                found_input = true;
                filepath = val;
            } else {
                std::cout << "Hmm..." << val << "... thats not it.\n";
            }
        }
    }

    if (!found_input) { std::cerr << "WARNING: No input file found!\n";}
    if (!filepath.empty() && !std::filesystem::exists(filepath)) {
        std::cerr << "WARNING: " << filepath << " does not exist!\n";
    }
    if (found_input) {
        auto extension = filepath.extension();
        std::cout << extension << "\n";
        std::cout << extension.c_str() << "\n";
        std::cout << (extension == ".nc") << "\n";
    }
    
    // load field data
    std::cout << "Initialising u, v fields...\n";

    int nx = 10;
    int ny = 10;
    double mag = 10.;
    Field field = single_gyre(nx, ny, mag, Direction::CounterClockwise);

    /*
    for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
        std::cout << field.v[(row * nx) + col] << ", ";
        }
    std::cout << "\n";

   
    }
     */

    // initialise particles
    std::cout << "Initialising particles... ";
    const std::array<double, 2> start_pos = {0.5, 0.1};
    const std::array<double, 2> end_pos = {0.5, 0.9};
    const int N = 25;
    std::vector<Particle> particles = from_line(start_pos, end_pos, N);

    /*
    std::cout << "Initialised the following particles:\n";
    for (int i=0; i < N; i++) {
        std::cout << i + 1 << "\t";
        Particle particle = particles[i];
        std::cout << "(" << particle.x << ", " << particle.y << ")\n";
    }
    */

    std::cout << "Initialised " << particles.size() << " particles:\n";
        
        
    

    // timestep loop

    // load data

    // advect
}