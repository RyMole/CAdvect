#ifndef FIELDS_H
#define FIELDS_H

#include <vector>

enum class Direction { Clockwise, CounterClockwise };

struct Field { std::vector<double> u; std::vector<double> v;};

std::vector<double> uniform_field(int nx, int ny, double var);

Field single_gyre(int nx, int ny, double mag, Direction dir);

#endif