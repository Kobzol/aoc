#include "mpi.h"
#include <sys/time.h>
#include <iostream>

double mysecond()
{
    struct timeval tp{};
    struct timezone tzp{};
    gettimeofday(&tp,&tzp);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const long count = 2 * 1000 * 1024 * 1024L;
    auto* mem = new double[count]();
    auto size = count * sizeof(double);

    if (rank == 0)
    {
        auto start = mysecond();
        MPI_Ssend(mem, count, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        auto duration = mysecond() - start;
        std::cout << "Transferred " << size << "bytes in " << duration << "seconds" << std::endl;
        std::cout << "Bandwidth: " << ((size  / duration) / (1024 * 1024)) << " Mib/s" << std::endl;
    }
    else
    {
        MPI_Status status;
        MPI_Recv(mem, count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    }

    delete[] mem;

    MPI_Finalize();

    return 0;
}
