set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(false-sharing false-sharing.cpp)
