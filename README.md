# CAdvect
Simple Lagrangian particle advection solver written in c++. This is intended as a demonstration / portfolio project and has introduced me to numerical methods in c++ and implementation using CMake.

## Details
CAdvect is configured via a yaml configuration file. The [example configuration file](https://github.com/RyMole/CAdvect/blob/main/config.yml) given includes all the parameters that the file must contain. CAdvect initialises (static) u and v velocity fields representing a single gyre, the strength of the gyre is controlled by `Mag` variable. Particles are initialised with a uniform distribution along a line, defined by two points given in the config file. Data is written out to netcdf file.

CAdvect uses bilinear interpolation to determine the Eulerian velocity at the location of each particle. They are then advected by using a simple forward [Euler](https://en.wikipedia.org/wiki/Euler_method) advection scheme. This is about as basic an advection kernel as you can get and will accumulate errors and may not lead to stable solutions but is useful as a proof of concept. CAdvect could be further developed with alternative advection schemes (eg Runge-Kutta 4th Order) which could be dropped into the time stepping loop in place of the forward Euler.

### Structure
CAdvect.cpp  | Config parsing and main timestepping loop

Fields.cpp   | Generation of velocity fields and associated grid points

Paricles.cpp | Generaiton of particle initial positions

# Compiling
You will need to ensure you have a c++ (version 20) compiler and the `netcdf-cxx` and `yaml-cpp` external libraries. In this case they were installed naively using homebrew, no guarantees if installed via other methods.

Prepare the CMake build: `cmake -B build --fresh`

Build the executable: `cmake --build build`

# Running
From the base directory: `./build/CAdvect -i config.yml`

# Future Work
This is intended as a first step toward learning Kokkos and will later be developed to implement the Kokkos library.
`/HelloWorld` contains the Hello World implementation of the [Kokkos docs](https://kokkos.org/kokkos-core-wiki/get-started/quick-start.html). Useful to check that the Kokkos software requirements are up and running correctly. Not part of the advection solver.