cmake_minimum_required(VERSION 4.1)
project(a_turtle_in_the_zoo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glfw3 REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)
find_package(PerlinNoise)

# Source Files
set(SOURCE_FILES "Source Files/main.cpp"
        "Header Files/VBO.h"
        "Source Files/VBO.cpp"
        "Header Files/VAO.h"
        "Header Files/EBO.h"
        "Source Files/VAO.cpp"
        "Source Files/EBO.cpp"
        "Source Files/shaderClass.cpp"
        "Source Files/stb.cpp"
        "Header Files/Texture.h"
        "Source Files/Texture.cpp"
        "Header Files/Camera.h"
        "Source Files/Camera.cpp"
        "Header Files/Mesh.h"
        "Source Files/Mesh.cpp"
        "Header Files/Chunk.h"
        "Source Files/Chunk.cpp"
        "Header Files/World.h"
        "Source Files/World.cpp"
        "Header Files/InputManager.h"
        "Source Files/InputManager.cpp"
        "Header Files/PlayerController.h"
        "Source Files/PlayerController.cpp"
        "Header Files/Physics.h"
        "Header Files/FiniteStateMachine/GameState.h"
        "Header Files/FiniteStateMachine/StateManager.h"
        "Source Files/FiniteStateMachine/StateManager.cpp"
        "Header Files/FiniteStateMachine/PlayingState.h"
        "Source Files/FiniteStateMachine/PlayingState.cpp"
        "Header Files/FiniteStateMachine/GameContext.h"
        "Header Files/HierarchicalStateMachine/MovementState.h"
        "Header Files/HierarchicalStateMachine/SprintingState.h"
        "Source Files/HierarchicalStateMachine/SprintingState.cpp"
        "Header Files/HierarchicalStateMachine/IdleState.h"
        "Header Files/HierarchicalStateMachine/SneakingState.h"
        "Header Files/HierarchicalStateMachine/WalkingState.h"
        "Source Files/HierarchicalStateMachine/IdleState.cpp"
        "Source Files/HierarchicalStateMachine/WalkingState.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Linking
target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw           # GLFW is sometimes just 'glfw' or 'glfw3' depending on the package
        glad::glad
        glm::glm
        stb::stb
        siv::PerlinNoise)
