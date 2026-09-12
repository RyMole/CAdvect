#ifndef FIELDS_H
#define FIELDS_H

#include <vector>

enum class Direction { Clockwise, CounterClockwise };

// velocity field with u and v components
struct Field { std::vector<double> u; std::vector<double> v;};

// uniform field of strength var
std::vector<double> uniform_field(int nx, int ny, double var);

// Field representing single gyre with strength mag, cc or cw
// circulation dependent on dir(ection)
Field single_gyre(int nx, int ny, double mag, Direction dir);

#endif