# For an introduction to CMake, see
# http://www.cmake.org/cmake/help/cmake_tutorial.html (at least steps 1 and 2)

project (pgk)

cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

#file(GLOB_RECURSE SOURCE_FILES *.c *.cpp)
#file(GLOB_RECURSE HEADER_FILES *.h *.hpp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# make commons library available for other projects
add_subdirectory(src/commons)

# links to all targets
link_libraries(
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${GLM_LIBRARY}
        ${GLFW_LIBRARIES}
        glfw)

# should add include directories to all targets ???
include_directories(
        PUBLIC ${OPENGL_INCLUDE_DIR}
        PUBLIC ${GLEW_INCLUDE_DIRS}
        PUBLIC ${GLM_INCLUDE_DIR}
        PUBLIC ${GLFW_INCLUDE_DIRS})

# create target for a project
add_subdirectory(src/aquarium)

add_subdirectory(src/z2)

add_subdirectory(src/segmentsGame)

set_target_properties(aquarium commons PROPERTIES EXCLUDE_FROM_ALL true)

# adds compilation definitions
ADD_COMPILE_DEFINITIONS(NDEBUG VERBOSITY_LOW)
target_compile_definitions(aquarium PUBLIC NDEBUG=VERBOSITY_LOW)

#file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})