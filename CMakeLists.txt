cmake_minimum_required(VERSION 3.10.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(raytracer src/main.cpp)

target_compile_options(raytracer PRIVATE -Wall -Wextra -Wpedantic)


