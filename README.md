# kokkos
Getting up and running with Kokkos.


# Getting Started

- Kokkos requires a c compiler. I have `gcc 21.0.0` (min 10.4.0).
- `CMake`. Min is far below current version (4.4.3)

# Hello World

Kokkos is not its own separate language. You write your code, including the Kokkos parts, in C++ and use text files to tell CMake how to compile the final program. Lets find out how to use it and follow the [Hello World](https://kokkos.org/kokkos-core-wiki/get-started/quick-start.html) from the docs.