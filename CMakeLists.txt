cmake_minimum_required(VERSION 2.8)
project(aok)

set(CMAKE_CXX_STANDARD 14)

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

add_subdirectory(cache-associativity)
add_subdirectory(cache-hierarchy)
add_subdirectory(false-sharing)
add_subdirectory(matmul)
add_subdirectory(net-bandwidth)
add_subdirectory(numactl)
add_subdirectory(register-rename)
