cmake_minimum_required(VERSION 4.1)
project(a_turtle_in_the_zoo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# GLFW
add_subdirectory(vendor/glfw-3.4)

# GLAD
include_directories(vendor/glad/include)
add_library(GLAD vendor/glad/src/glad.c)

# Source Files
set(SOURCE_FILES "Source Files/main.cpp"
        "Header Files/VBO.h"
        "Source Files/VBO.cpp"
        "Header Files/VAO.h"
        "Header Files/EBO.h"
        "Source Files/VAO.cpp"
        "Source Files/EBO.cpp"
        "Source Files/shaderClass.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Linking
target_link_libraries(${PROJECT_NAME} glfw GLAD)
