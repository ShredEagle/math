cmake_minimum_required(VERSION 3.16)
project(PackageTest CXX)

find_package(Math REQUIRED COMPONENTS math)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE ad::math)
