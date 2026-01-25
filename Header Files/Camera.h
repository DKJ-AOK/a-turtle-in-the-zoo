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

#include "shaderClass.h"

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    // Prevents the camera from jumping around when first clicking left click
    bool firstClick = true;

    int width;
    int height;

    float speed = 0.01f;
    float sensitivity = 100.0f;

    Camera(int width, int height, glm::vec3 position);

    void UpdateMatrix(float FOV, float nearPlane, float farPlane);
    void Matrix(Shader& shader, const char* uniform);
    void Inputs(GLFWwindow* window);
};

#endif //A_TURTLE_IN_THE_ZOO_CAMERA_H