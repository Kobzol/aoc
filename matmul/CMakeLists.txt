set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -march=native -g -fno-tree-vectorize -fopenmp")

find_package(BLAS REQUIRED)

add_executable(matmul matmul.cpp)
target_link_libraries(matmul benchmark blas)
add_dependencies(matmul gbenchmark)
