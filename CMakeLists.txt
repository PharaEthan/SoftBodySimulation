cmake_minimum_required(VERSION 3.12)

project(computer_graphics LANGUAGES CXX VERSION 1.0.1)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(VCPKG_TARGET_TRIPLET "x64-windows-static")

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

# Dependencies (find_package vcpkg)
include(Dependencies)

# Project

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES

	# Engine source files

    Engine/Components/Transform.cpp

    Engine/Renderer/Camera/Camera.cpp
    Engine/Renderer/Texture/Texture.cpp
    Engine/Renderer/Materials/Material.cpp
    Engine/Renderer/Materials/PBRMaterial.cpp
    Engine/Renderer/Shader/Shader.cpp
    Engine/Renderer/ShadowRenderer.cpp
    Engine/Renderer/PostProcessing.cpp

    Engine/Mesh/Mesh.cpp
    Engine/Mesh/Primitives/Cube.cpp
    Engine/Mesh/Primitives/Sphere.cpp
    Engine/Mesh/Primitives/Quad.cpp

    Engine/Box/AABB.cpp

    Engine/Utils/Utils.cpp

    Engine/Scene/Scene.cpp

    Engine/Engine.cpp
    Engine/Settings.cpp

    # Future client source files

    Client/OrbitCamera.cpp
    Client/SoftBodyApp.cpp

    # Change the main file to run different simulations
    # Main.cpp  => Empty template for free sandbox using my engine.
    # Main1.cpp => Simulation 1 using my engine.
    # Main2.cpp => Simulation 2 using my engine.
    # Main3.cpp => Simulation 3 using my engine.
    Main1.cpp
)

set(INCLUDE_DIRS
    "./"

    Engine/
    Engine/Box/
    Engine/Components/
    Engine/Mesh/
    Engine/Renderer/
    Engine/Scene/
    Engine/Utils/
    Engine/Physics/
    Engine/Ray/

    Client/
)

include_directories(${INCLUDE_DIRS})

add_executable(computer_graphics ${SOURCES})

target_link_libraries(computer_graphics PRIVATE
    imgui::imgui
    glfw
    glad::glad
    glm::glm
    ${Stb_INCLUDE_DIR}
    legacy_stdio_definitions.lib
)
