#include "../Header Files/Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
    : Position(position), width(width), height(height) {
    UpdateCameraVectors();
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

void Camera::UpdateMatrix(const float FOVdeg, const float nearPlane, const float farPlane) {
    // Initializes matrices since otherwise they will be the null matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
    constexpr float eyeOffset = 0.8f;
    const glm::vec3 eyePos = Position + glm::vec3(0.0f, eyeOffset, 0.0f);

    view = glm::lookAt(eyePos, eyePos + Forward, Up);
    // Adds perspective to the scene
    projection = glm::perspective(glm::radians(FOVdeg), static_cast<float>(width) / height, nearPlane, farPlane);

    // Sets new camera matrix
    cameraMatrix = projection * view;
}

void Camera::Matrix(const Shader &shader, const char *uniform) {
    // Exports camera matrix
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::HandleRotation(const float moveDeltaX, const float moveDeltaY) {
    Yaw   += moveDeltaX;
    Pitch += moveDeltaY;

    // Clamp Pitch to avoid Gimbal Lock
    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    UpdateCameraVectors();
}