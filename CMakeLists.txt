cmake_minimum_required(VERSION 3.16)
project(syncronous_database)

set(CMAKE_CXX_STANDARD 23)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})
link_directories(${MPI_LIBRARY_DIRS})

file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(syncronous_database ${SOURCES})
target_link_libraries(syncronous_database ${MPI_LIBRARIES})
