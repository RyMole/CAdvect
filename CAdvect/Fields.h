#ifndef FIELDS_H
#define FIELDS_H

#include <vector>

enum class Direction { Clockwise, CounterClockwise };
enum class GridType { Centres, Edges };

// velocity field with u and v components
struct Field { std::vector<double> u; std::vector<double> v;};

// uniform field of strength var
std::vector<double> uniform_field(int nx, int ny, double var);

// Field representing single gyre with strength mag, cc or cw
// circulation dependent on dir(ection)
Field single_gyre(int nx, int ny, double mag, Direction dir);

// Grid Point Calculator
std::vector<double> generate_gridpoints (const double lim0, const double lim1, const int n, const GridType type);

#endif