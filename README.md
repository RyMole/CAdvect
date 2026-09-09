# kokkos
Getting up and running with Kokkos.


# Getting Started
- Kokkos requires a c compiler. I have `gcc 21.0.0` (min 10.4.0).
- `CMake` Min is far below current version (4.4.3)

## Hello World
Kokkos is an extension library for C++. You write your code, including the Kokkos parts, in C++ and use CMake how to compile the final program. To be sure that the installation of all requirements is working correctly, one can follow the [Hello World](https://kokkos.org/kokkos-core-wiki/get-started/quick-start.html) program from the kokkos docs.

## Configuring Kokkos
The recommended way to use kokkos is as an external libarary. Configure an install locally and then when you want to use it, declare it in your CMake file and point to the right location. For my current use case, I am installing directly from homebrew and will update if there are any difficulties or peculiarities to the installation.