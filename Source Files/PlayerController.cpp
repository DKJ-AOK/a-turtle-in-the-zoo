//
// Created by Oetho on 29-01-2026.
//

#include "../Header Files/PlayerController.h"

void PlayerController::update(float deltaTime) {
    // 1. Rotation (Mouse)
    camera.HandleRotation(inputManager.getMouseDeltaX(), inputManager.getMouseDeltaY());

    // 2. Horizontal Movement (WASD)
    handleGroundMovement(deltaTime);

    // 3. Vertical Movement (Gravity & Jump)
    applyPhysics(deltaTime);

    // 4. Matrix Update
    camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
}

void PlayerController::applyPhysics(float deltaTime) {
    // Add Gravity to the vertical speed
    if (!isGrounded)
        verticalVelocity += gravity * deltaTime;
    else
        verticalVelocity = 0.0f;

    // Jump Logic
    if (inputManager.isActionJustPressed(Action::JUMP) && isGrounded) {
        std::cout << "Jump" << std::endl;
        verticalVelocity = jumpForce;
        isGrounded = false;
    }

    // Update position based on speed
    camera.Position.y += verticalVelocity * deltaTime;

    // Simple ground-check
    if (camera.Position.y <= groundLevel) {
        camera.Position.y = groundLevel;
        isGrounded = true;
    }
}

void PlayerController::handleGroundMovement(float deltaTime) {
    if (inputManager.isActionActive(Action::MOVE_FORWARD)) {
        camera.HandleMovement(MOVE_FORWARD, deltaTime);
    }

    if (inputManager.isActionActive(Action::MOVE_BACKWARD)) {
        camera.HandleMovement(MOVE_BACKWARD, deltaTime);
    }

    if (inputManager.isActionActive(Action::MOVE_LEFT)) {
        camera.HandleMovement(MOVE_LEFT, deltaTime);
    }

    if (inputManager.isActionActive(Action::MOVE_RIGHT)) {
        camera.HandleMovement(MOVE_RIGHT, deltaTime);
    }

    if (inputManager.isActionActive(Action::SPRINT))
        camera.HandleMovement(SPRINT, deltaTime);
    else
        camera.HandleMovement(WALK, deltaTime);

    if (inputManager.isActionJustPressed(Action::HIT))
        std::cout << "Hit" << std::endl;

    if (inputManager.isActionJustPressed(Action::PLACE))
        std::cout << "Place" << std::endl;

    if (inputManager.isActionJustPressed(Action::INTERACT))
        std::cout << "Interact" << std::endl;
}
