#ifndef     PARTICLES_H
#define PARTICLES_H

#include <vector>
#include <array>


struct Particle { double x, y;};

// return vector of particles given two coordinates and number of particles N
// coords must be arrays of (x, y)
std::vector<Particle> from_line(const std::array<double, 2>& start_pos, const std::array<double, 2>& end_pos, int N);

#endif