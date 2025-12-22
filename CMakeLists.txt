cmake_minimum_required(VERSION 3.10.0)
project(raytracer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(raytracer 
    src/main.cpp
    src/parser/scene_parser.cpp
    src/parser/xml_parser_utils.cpp
    lib/xml-parser/tinyxml2.cpp
)

# header files do not need to be added to exceutables but directories here!
target_include_directories(raytracer PRIVATE 
    src
    src/math
    src/scene
    src/parser
    lib/xml-parser

)

target_compile_options(raytracer PRIVATE -Wall -Wextra -Wpedantic)


