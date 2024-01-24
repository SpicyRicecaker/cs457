cmake_minimum_required(VERSION 3.10)
project(GraphicsProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Linker flags
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always")

# Automatically include all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")

# Add executable
add_executable(main ${SOURCES})

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

# Link libraries
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})

# Set the build type (Optional)
set(CMAKE_BUILD_TYPE Debug)
