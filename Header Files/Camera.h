//
// Created by danie on 25/01/2026.
//

#ifndef A_TURTLE_IN_THE_ZOO_CAMERA_H
#define A_TURTLE_IN_THE_ZOO_CAMERA_H
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "InputManager.h"
#include "shaderClass.h"
#include "World.h"

class Camera {
public:
    // Positioning
    glm::vec3 Position;
    glm::vec3 Forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up      = glm::vec3(0.0f, 1.f, 0.0f);
    glm::vec3 Right   = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Euler Angle for rotation
    float Yaw   = -90.0f; // Start pegende mod -Z
    float Pitch =  0.0f;

    // View/Projection
    glm::mat4 cameraMatrix = glm::mat4(1.0f);
    int width, height;

    // Settings
    float SprintSpeed = 10.0f;
    float WalkSpeed = 5.0f;
    float Speed = WalkSpeed;
    float Sensitivity = 1.0f;

    Camera(int width, int height, glm::vec3 position);

    // Updates Forward, Right og Up based on Yaw/Pitch
    void UpdateCameraVectors();

    // Calculates the final Matrix (Projection * View)
    void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane);

    // Sends matrix to shader
    void Matrix(Shader& shader, const char* uniform);

    // Receives data from the InputManager
    void HandleRotation(float moveDeltaX, float moveDeltaY);
    void HandleGroundMovement(Action action, float deltaTime, World& world, glm::vec3 playerHalfExtent);
    void HandleFlyingMovement(Action action, float deltaTime);

private:
    void HandleMovementXAxis(Action action, float velocity, float flatForwardX, float flatRightX, World& world, glm::vec3 playerHalfExtent);
    void HandleMovementZAxis(Action action, float velocity, float flatForwardZ, float flatRightZ, World& world, glm::vec3 playerHalfExtent);
};

#endif //A_TURTLE_IN_THE_ZOO_CAMERA_H