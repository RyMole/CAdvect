#include <Kokkos_Core.hpp>

int main(int argc, char** argv) {
    Kokkos::initialize(argc, argv);
    {
        // allocate a 1d 'view' of integers
        Kokkos::View<int*> v("v", 5);
        // fill it with increasing vales 0->4
        Kokkos::parallel_for("fill", 5, KOKKOS_LAMBDA(int i) { v(i) = i; });
        // compute sum of v's elements
        int r;
        Kokkos::parallel_reduce(
            "accumulate", 5,
            KOKKOS_LAMBDA(int i, int& partial_r) { partial_r += v(i); }, r);
        // check result
        KOKKOS_ASSERT(r == 10);
    }
    Kokkos::printf("Goodbye World\n");
    Kokkos::finalize();
    return 0;
}