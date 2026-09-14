#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>

// STRUCTS
// simple representation of particles. x, y = position coordinates
struct Particle { double x, y;};

// FUNCTIONS
// return vector of particles given two coordinates and number of particles N coords must be arrays of (x, y)
std::vector<Particle> from_line(const std::vector<double>& start_pos, const std::vector<double>& end_pos, int N);

#endif