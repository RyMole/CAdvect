#include <iostream>
#include <filesystem>
#include "Fields.h"

int main(int argc, char* argv[]) {

    std::cout << "There were " << argc << " command line arguments:" << "\n";

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
    auto extension = filepath.extension();
    std::cout << extension << "\n";
    std::cout << extension.c_str() << "\n";
    std::cout << (extension == ".nc") << "\n";
    
    // load data
    std::cout << "trying to initialise a field... \n\n";

    int nx = 10;
    int ny = 10;
    double mag = 10.;
    Field field = single_gyre(nx, ny, mag, Direction::CounterClockwise);

    for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
        std::cout << field.v[(row * nx) + col] << ", ";
        }
    std::cout << "\n";
    }

    // initialise particles

    // timestep loop

    // load data

    // advect
}