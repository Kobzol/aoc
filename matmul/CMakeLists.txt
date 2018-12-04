set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -march=native -g -fno-tree-vectorize -fopenmp")

find_package(BLAS REQUIRED)

include(ExternalProject)

set(BENCHMARK_ROOT ${CMAKE_CURRENT_BINARY_DIR}/benchmark)

# download Google benchmark
ExternalProject_Add(gbenchmark
        PREFIX ${BENCHMARK_ROOT}
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG master
        CMAKE_ARGS -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
        INSTALL_COMMAND ""
        UPDATE_COMMAND ""
)
include_directories(${BENCHMARK_ROOT}/src/gbenchmark/include)
link_directories(${BENCHMARK_ROOT}/src/gbenchmark-build/src)

add_executable(matmul matmul.cpp)
target_link_libraries(matmul benchmark blas)
add_dependencies(matmul gbenchmark)
