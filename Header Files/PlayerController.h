//
// Created by Oetho on 29-01-2026.
//

#ifndef A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H
#define A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H
#include "Camera.h"

class PlayerController {
    public:
    Camera camera;
    InputManager& inputManager;

    PlayerController(InputManager& inputManagerRef, int screenWidth, int screenHeight)
    : camera(screenWidth, screenHeight, glm::vec3(0.0f, 10.0f, 5.0f)), inputManager(inputManagerRef) {};

    void update(float deltaTime);
    void CheckForMovement(float deltaTime);
};

#endif //A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H