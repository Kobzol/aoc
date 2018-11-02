find_package(MPI REQUIRED)

set(CMAKE_CXX_STANDARD 14)

add_executable(bandwidth bandwidth.cpp)
target_include_directories(bandwidth PRIVATE ${MPI_INCLUDE_PATH})
target_link_libraries(bandwidth ${MPI_C_LIBRARIES})
