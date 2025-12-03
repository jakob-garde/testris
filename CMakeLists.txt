cmake_minimum_required(VERSION 3.13.0)
project(testris VERSION 0.1.0)


# linux:
if (UNIX)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw)


# Windows:
else()
message("WIN32 configuration")

# glfw / glew
include_directories(lib/include/)
link_directories(lib/)

add_executable(${PROJECT_NAME} WIN32 main.cpp)
target_link_libraries(${PROJECT_NAME} glfw3)
target_link_libraries(${PROJECT_NAME} glew32s)
target_link_libraries(${PROJECT_NAME} OpenGL32)


endif()
