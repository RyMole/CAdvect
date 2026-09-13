
#include "Fields.h"
#include <vector>
#include <cmath>
#include <numbers>


std::vector<double> half_wave(int n) {
    // return half a cos wave stored as vector of length n
    std::vector<double> vec(n);
    for (int x = 0; x < n; x++) {
        vec[x] = std::cos(std::numbers::pi * x / (n - 1));
    }
    return vec;
}

std::vector<double> single_gyre_component(int nx, int ny, double mag) {
    // return (nx, ny) vector of single velocity component of a single gyre
    std::vector<double> field(nx * ny);
    std::vector<double> vec = half_wave(nx);
    for (int row = 0; row < (ny); row++) {
        for (int col = 0; col < (nx); col++) {
            field[(row * nx) + col] = mag * vec[col];
        }
    }
    return field;
}

std::vector<double> uniform_field(int nx, int ny, double mag) {
    // return (nx, ny) vector uniform field with strength mag
    std::vector<double> field(nx * ny, mag);
    return field;
}

Field single_gyre(int nx, int ny, double mag, Direction dir) {
    // return Field structure of single gyre (with u and v components)
    std::vector<double> u_field = single_gyre_component(ny, nx, mag);
    std::vector<double> v_field = single_gyre_component(nx, ny, mag);
    std::vector<double> u_field_rotated(nx * ny);

    // rotate u field 90 clockwise and correct EW direction
    // ny = 5, nx = 10
    for (int row = 0; row < nx; row++) {
        for (int col = 0; col < ny; col++) {
            int new_row = col;
            int new_col = nx - row - 1;
           /*std::cout << "original (" << row << ", " << col << ") goes to (" << new_row << ", " << new_col;
            std::cout << ") old: " << (row * ny) + col << " new: " << (new_row * nx) + new_col << ")\n";*/
            u_field_rotated[(new_row * nx) + new_col] = -u_field[(row * ny) + col];
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


std::vector<double> generate_gridpoints (const double lim0, const double lim1, const int n, const GridType type) {
    std::vector<double> points;
    const double dx = (lim1 - lim0) / n;

    switch (type) {
        case GridType::Centres:
            for (int i = 0; i < n; i++ ) {
                    points.push_back((lim0 + dx / 2) + i * dx);
                }
            break;

        case GridType::Edges:
            for (int i = 0; i < (n + 1); i++ ) {
                points.push_back(lim0 + i * dx);
            }
            break;
    }


    return points;
}