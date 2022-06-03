cmake_minimum_required(VERSION 3.22)
project(libSF)

set(CMAKE_CXX_STANDARD 20)

add_library(libSF STATIC SF.cpp SF.h)

add_executable(Test main.cpp)

target_link_libraries(Test libSF)
