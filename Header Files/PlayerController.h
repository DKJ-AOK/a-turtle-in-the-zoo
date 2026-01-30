#ifndef A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H
#define A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H
#include "Camera.h"

class PlayerController {
    public:
    Camera camera;
    InputManager& inputManager;

    // Physics Variables
    float verticalVelocity = 0.0f;
    const float gravity = -9.81f; // Standard Earth-gravity
    const float jumpForce = 5.0f;
    bool isGrounded = false;
    float groundLevel = 0.0f; // Temporary until collision is added.

    // God Mode
    bool isGodModeActive = false;

    PlayerController(InputManager& inputManagerRef, int screenWidth, int screenHeight)
    : camera(screenWidth, screenHeight, glm::vec3(0.0f, 30.0f, 5.0f)), inputManager(inputManagerRef) {};

    void update(float deltaTime);

private:
    void handleGroundMovement(float deltaTime);
    void handleFlyingMovement(float deltaTime);
    void applyPhysics(float deltaTime);
};

#endif //A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H