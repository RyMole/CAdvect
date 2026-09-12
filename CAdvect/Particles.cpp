#include "Particles.h"
#include <vector>
#include <array>


// initialise a vector of particles given two coordinates and number of particles N
// coords must be arrays of (x, y)
std::vector<Particle> from_line(const std::array<double, 2>& start_pos, const std::array<double, 2>& end_pos, int N){
    // solve gradient of line
    const double delta_x = end_pos[0] - start_pos[0];
    const double gradient = (end_pos[1] - start_pos[1]) / delta_x;

    // initialise output vector
    std::vector<Particle> particles(N);

    // fill particles
    for (int i=0; i<N; i++) {
        double dx = delta_x * i / (N - 1);
        Particle particle;
        particle.x = start_pos[0] + dx;
        particle.y = start_pos[1] + gradient * dx;
        particles[i] = particle;
    }

    return particles;
}

