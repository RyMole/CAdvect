
#include "Fields.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <numbers>


std::vector<double> half_wave(int n) {
    std::vector<double> vec(n);
    for (int x = 0; x < n; x++) {
        vec[x] = std::cos(std::numbers::pi * x / (n - 1));
    }
    return vec;
}

std::vector<double> single_gyre_component(int nx, int ny, double mag) {
    std::vector<double> field(nx * ny);
    std::vector<double> vec = half_wave(nx);
    for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
            field[(row * nx) + col] = vec[col];
        }
    }
    return field;
}

std::vector<double> uniform_field(int nx, int ny, double mag) {
    std::vector<double> field(nx * ny, mag);
    return field;
}

Field single_gyre(int nx, int ny, double mag, Direction dir) {
    std::vector<double> u_field = single_gyre_component(ny, nx, mag);
    std::vector<double> v_field = single_gyre_component(nx, ny, mag);
    std::vector<double> u_field_rotated(nx * ny, 5);

    // rotate u field 90 clockwise
    // ny = 5, nx = 10
    for (int row = 0; row < nx; row++) {
        for (int col = 0; col < ny; col++) {
            int new_row = col;
            int new_col = nx - row - 1;
           /*std::cout << "original (" << row << ", " << col << ") goes to (" << new_row << ", " << new_col;
            std::cout << ") old: " << (row * ny) + col << " new: " << (new_row * nx) + new_col << ")\n";*/
            u_field_rotated[(new_row * nx) + new_col] = u_field[(row * ny) + col];
        }
    }

    if (dir == Direction::CounterClockwise) {
        // reverse rotation direction
        for (int row = 0; row < ny; row++) {
            for (int col = 0; col < nx; col++) {
                u_field_rotated[(row * nx) + col] = - u_field_rotated[(row * nx) + col];
                v_field[(row * nx) + col] = - v_field[(row * nx) + col];
            }
        }
    }

    return {u_field_rotated, v_field};
}

/*
int main() {
    int nx = 10;
    int ny = 5;
    Field field = single_gyre(nx, ny, 2, Direction::Clockwise);

        for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
            std::cout << field.v[(row * nx) + col] << " ";
        }
        std::cout << "\n";
    }

    field = single_gyre(nx, ny, 2, Direction::CounterClockwise);
    
    
        for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
            std::cout << field.v[(row * nx) + col] << " ";
        }
        std::cout << "\n";
    }
    
    
    return 0;
}
*/
/** 
int main() {
    int nx = 10;
    int ny = 5;
    std::vector<double> test_field = uniform_field(nx, ny, 2.3);

    
    for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
            std::cout << test_field[(row * nx) + col] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";
    std::cout << "\n";


    // std::vector<double> test = single_gyre_component(nx, ny, 1);

    for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
            std::cout << test[(row * nx) + col] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";
    std::cout << "\n";


    Field field = single_gyre(nx, ny, 2, Direction::Clockwise);

    for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
            std::cout << field.u[(row * nx) + col] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";

        for (int row = 0; row < (ny); row++) {
            for (int col = 0; col < (nx); col++) {
                std::cout << field.v[(row * nx) + col] << " ";
         }
        std::cout << "\n";
    }

    return 0;
}
*/