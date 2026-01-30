#include "../Header Files/Camera.h"

#include "../Header Files/Physics.h"

Camera::Camera(int width, int height, glm::vec3 position) {
    Camera::width = width;
    Camera::height = height;
    Position = position;
}

void Camera::UpdateCameraVectors() {
    // Calculate the new Forward-vektor from the updated Euler-angles
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Forward = glm::normalize(front);

    // Calculate Right and Up vectors (Important for movement)
    // Uses a 'World Up' (0, 1, 0) to find Right through a Cross Product
    Right = glm::normalize(glm::cross(Forward, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Forward));
}

void Camera::UpdateMatrix(float FOV, float nearPlane, float farPlane) {
    // Initializes matrices since otherwise they will be the null matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
    float eyeOffset = 0.8f;
    glm::vec3 eyePos = Position + glm::vec3(0.0f, eyeOffset, 0.0f);

    view = glm::lookAt(eyePos, eyePos + Forward, Up);
    // Adds perspective to the scene
    projection = glm::perspective(glm::radians(FOV), static_cast<float>(width) / height, nearPlane, farPlane);

    // Sets new camera matrix
    cameraMatrix = projection * view;
}

void Camera::Matrix(Shader &shader, const char *uniform) {
    // Exports camera matrix
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::HandleRotation(float moveDeltaX, float moveDeltaY) {
    Yaw   += moveDeltaX * Sensitivity;
    Pitch += moveDeltaY * Sensitivity;

    // Clamp Pitch to avoid Gimbal Lock
    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    UpdateCameraVectors();
}

void Camera::HandleGroundMovement(Action action, float deltaTime, World& world, glm::vec3 playerHalfExtent) {
    float velocity = Speed * deltaTime;

    // For landscape-movement (NOT IN USE YET)
    glm::vec3 flatForward = glm::normalize(glm::vec3(Forward.x, 0.0f, Forward.z));
    glm::vec3 flatRight = glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));
    auto previousPosition = Position;

    if (action == Action::SPRINT)
        Speed = SprintSpeed;
    if (action == Action::WALK)
        Speed = WalkSpeed;

    HandleMovementXAxis(action, velocity, flatForward.x, flatRight.x, world, playerHalfExtent);
    HandleMovementZAxis(action, velocity, flatForward.z, flatRight.z, world, playerHalfExtent);
}

void Camera::HandleMovementXAxis(Action action, float velocity, float flatForwardX, float flatRightX, World &world, glm::vec3 playerHalfExtent) {
    auto previousPositionX = Position.x;

    if (action == Action::MOVE_FORWARD)
        Position.x += flatForwardX * velocity;
    if (action == Action::MOVE_BACKWARD)
        Position.x -= flatForwardX * velocity;
    if (action == Action::MOVE_LEFT)
        Position.x -= flatRightX * velocity;
    if (action == Action::MOVE_RIGHT)
        Position.x += flatRightX * velocity;

    AABB playerBox = AABB::fromCenter(Position, playerHalfExtent);
    AABB cubeBox;
    if (world.checkCollision(playerBox, cubeBox))
        Position.x = previousPositionX;
}

void Camera::HandleMovementZAxis(Action action, float velocity, float flatForwardZ, float flatRightZ, World &world, glm::vec3 playerHalfExtent) {
    auto previousPositionZ = Position.z;

    if (action == Action::MOVE_FORWARD)
        Position.z += flatForwardZ * velocity;
    if (action == Action::MOVE_BACKWARD)
        Position.z -= flatForwardZ * velocity;
    if (action == Action::MOVE_LEFT)
        Position.z -= flatRightZ * velocity;
    if (action == Action::MOVE_RIGHT)
        Position.z += flatRightZ * velocity;

    AABB playerBox = AABB::fromCenter(Position, playerHalfExtent);
    AABB cubeBox;
    if (world.checkCollision(playerBox, cubeBox))
        Position.z = previousPositionZ;
}

void Camera::HandleFlyingMovement(Action action, float deltaTime) {
    float velocity = Speed * deltaTime;

    if (action == Action::MOVE_FORWARD)
        Position += Forward * velocity;
    if (action == Action::MOVE_BACKWARD)
        Position -= Forward * velocity;
    if (action == Action::MOVE_LEFT)
        Position -= Right * velocity;
    if (action == Action::MOVE_RIGHT)
        Position += Right * velocity;
    if (action == Action::SPRINT)
        Speed = SprintSpeed;
    if (action == Action::WALK)
        Speed = WalkSpeed;
}