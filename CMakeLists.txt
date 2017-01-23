project(fluidSim)
cmake_minimum_required(VERSION 2.8)

# Useful CMake options for Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "-std=c++0x")
cmake_policy(SET CMP0004 OLD)


# Search desired Qt packages
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OPENGL_LIBRARY} ${SDL2_LIBRARIES} ${GLEW_LIBRARIES})

#specify include directory
include_directories(INCLUDE_DIRS ${INCLUDE_DIRS} include)

set( project_sources
   fluidSim.cpp
   camera.cpp
   Mesh.cpp
   ShaderProgram.hpp
)

add_executable(${PROJECT_NAME} fluidSim.cpp camera.cpp Mesh.cpp)

target_link_libraries(${PROJECT_NAME} ${LIBS})

#Now copy shaders to build directory
add_custom_target(copy-runtime-files ALL
    COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}
    DEPENDS ${MY_TARGET})
