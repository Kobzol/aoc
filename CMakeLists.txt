cmake_minimum_required(VERSION 2.8)
project(aok)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(matmul)
add_subdirectory(numactl)
add_subdirectory(net-bandwidth)
add_subdirectory(cache-hierarchy)
add_subdirectory(cache-associativity)
add_subdirectory(false-sharing)
