# For an introduction to CMake, see
# http://www.cmake.org/cmake/help/cmake_tutorial.html (at least steps 1 and 2)

project (objViewer)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
file(GLOB_RECURSE SOURCE_FILES *.c *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h *.hpp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(objViewer
        ${HEADER_FILES}
        ${SOURCE_FILES})

target_include_directories(objViewer
        PUBLIC ${OPENGL_INCLUDE_DIR}
        PUBLIC ${GLEW_INCLUDE_DIRS}
        PUBLIC ${GLM_INCLUDE_DIR}
        PUBLIC ${GLFW_INCLUDE_DIRS})

target_link_libraries(objViewer
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${GLM_LIBRARY}
        ${GLFW_LIBRARIES}
        glfw)

include_directories(.)

ADD_COMPILE_DEFINITIONS(NDEBUG VERBOSITY_LOW VERBOSITY_HIGH)

file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})