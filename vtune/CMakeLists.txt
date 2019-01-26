SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_subdirectory(routing-algorithms)

add_executable(branch-misprediction branch-misprediction.cpp)
add_executable(memory-bound memory-bound.cpp)

find_package(HDF5 COMPONENTS "CXX" REQUIRED)

include_directories(routing-algorithms)
include_directories(routing-algorithms/include)

add_executable(routing-bench routing-bench.cpp)
target_link_libraries(routing-bench routing)
target_include_directories(routing-bench PRIVATE ${HDF5_INCLUDE_DIRS})
