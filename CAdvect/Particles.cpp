#include "Particles.h"
#include <vector>
#include <stdexcept>


// Function to return a vector of N particles distributed uniformly across a line defined by start
// and end positions. Position coords must be arrays of (x, y)
std::vector<Particle> from_line(const std::vector<double>& start_pos, const std::vector<double>& end_pos, int N){
    // ensure start and end points differ
    if ( start_pos == end_pos ) {
        throw std::runtime_error("Start and end points must be different!");
    }
    // ensure start and end points look the right shape
    if ((start_pos.size() !=2) || (end_pos.size() != 2)) {
        throw std::runtime_error("Start and end points must be length 2!");
    }

    const double delta_x = end_pos[0] - start_pos[0];
    const double delta_y = end_pos[1] - start_pos[1];

    // initialise output vector
    std::vector<Particle> particles(N);

    // generate particles
    if ( N == 1 ) {
        // handle n = 1 particle edge case returning mean position of start and end points
        particles[0] = Particle{ 0.5 * (start_pos[0] + end_pos[0]), 0.5 * (start_pos[1] + end_pos[1])};
    } else {
        // loop over n particles each shifted by appropriate fraction of delta_x, delta_y
        for (int i=0; i<N; i++) {
            Particle particle;
            particle.x = start_pos[0] + (static_cast<double>(i) / (N - 1)) * delta_x;
            particle.y = start_pos[1] + (static_cast<double>(i) / (N - 1)) * delta_y;
            particles[i] = particle;
        }
    }
    return particles;
}

