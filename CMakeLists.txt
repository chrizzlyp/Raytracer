cmake_minimum_required(VERSION 3.10.0)
project(raytracer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(raytracer 
    src/main.cpp
    src/parser/xml-parser/scene_parser.cpp
    src/parser/xml-parser/utils/xml_parser_utils.cpp
    src/parser/xml-parser/utils/scene_parser_lights.cpp
    src/parser/xml-parser/utils/scene_parser_surface.cpp
    src/parser/obj-parser/object_parser.cpp
    src/scene/lights/utils/lights_io.cpp
    src/scene/surfaces/components/transform.cpp
    src/scene/surfaces/sphere.cpp
    src/scene/surfaces/mesh.cpp
    src/scene/surfaces/components/texture.cpp
    src/render/ray_generator.cpp
    src/render/renderer.cpp
    src/render/shader.cpp
    src/render/shadow_tester.cpp
    src/render/image-writer/image_writer.cpp
    assets/textures/texture_manager.cpp
    lib/xml-parser/tinyxml2.cpp
)

# header files do not need to be added to exceutables but directories here!
target_include_directories(raytracer PRIVATE 
    src
    lib/xml-parser
    lib/image-writer
    assets

)

target_compile_options(raytracer PRIVATE -Wall -Wextra -Wpedantic)


