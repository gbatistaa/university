cmake_minimum_required(VERSION 3.16)
project(syncronous_database)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(syncronous_database ${SOURCES})
