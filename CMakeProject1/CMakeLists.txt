set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Linker flags
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
# set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always")

# Automatically include all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")

# Add executable
add_executable(main ${SOURCES})

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)

# Include directories
# include_directories(${OPENGL_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS})
target_include_directories(main PUBLIC ${Stb_INCLUDE_DIR})

# Link libraries
target_link_libraries(main PUBLIC OpenGL::GL glad::glad glfw glm::glm imgui::imgui)

# Set the build type (Optional)
# set(CMAKE_BUILD_TYPE Debug)
