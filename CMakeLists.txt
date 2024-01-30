# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.27)

# Set the project name
project(test)

# Set the name of the executable
set(EXEC "test")

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 23)

# Include directories for header files
include_directories(includes)

# Glob source files in the src directory
file(GLOB SOURCES src/*.cpp)

# Set the output directory for the built executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Create the executable from the specified source files
add_executable(${EXEC} ${SOURCES})

# Find the SDL2 framework library
find_library(SDL2_LIBRARY SDL2)

# Find the path to the SDL.h header file
find_path(SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES SDL2)

# Check if SDL2 library was found
if(NOT SDL2_LIBRARY)
    message(FATAL_ERROR "SDL2 library not found.")
endif()

# Check if SDL.h include directory was found
if(NOT SDL2_INCLUDE_DIR)
    message(FATAL_ERROR "SDL2 include directory not found.")
endif()

# Link the executable against the SDL2 framework
target_link_libraries(${EXEC} PRIVATE ${SDL2_LIBRARY})

# Include the SDL.h header files
target_include_directories(${EXEC} PRIVATE ${SDL2_INCLUDE_DIR})
