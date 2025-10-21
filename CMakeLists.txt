cmake_minimum_required(VERSION 3.16)
project(syncronous_database LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MPI REQUIRED)

file(GLOB_RECURSE SOURCES "*.c" "*.cpp")

add_executable(syncronous_database ${SOURCES})

# Linka MPI C e C++ — se só tiver C, o target MPI::MPI_C funciona, mas se tiver C++:
target_link_libraries(syncronous_database PRIVATE MPI::MPI_C MPI::MPI_CXX)
