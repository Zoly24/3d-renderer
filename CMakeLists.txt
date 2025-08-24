cmake_minimum_required(VERSION 3.16)
project(renderer C)

# Always build in Debug mode
set(CMAKE_BUILD_TYPE Debug)

# Debug Flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall -Wextra")

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Find SDL3 (uses SDL3Config.cmake)
find_package(SDL3 REQUIRED)

# Source files
set(SOURCE_FILES
    src/main.c
    src/triangle.c
    src/camera.c
    src/obj_reader.c
    src/geometry.c
    src/line.c
    src/model.c
    src/transform.c
    src/input.c
)

# Create executable
add_executable(renderer ${SOURCE_FILES})

# Include your own headers
target_include_directories(renderer PRIVATE src)

# Link SDL3 (modern target) — this includes headers and libs
target_link_libraries(renderer PRIVATE SDL3::SDL3 m)
