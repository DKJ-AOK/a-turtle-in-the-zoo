//
// Created by Oetho on 29-01-2026.
//

#include "../Header Files/PlayerController.h"

#include "../Header Files/Physics.h"

void PlayerController::update(float deltaTime) {
    // 1. Rotation (Mouse)
    camera.HandleRotation(inputManager.getMouseDeltaX(), inputManager.getMouseDeltaY());

    // 2. Horizontal Movement (WASD)
    if (isGodModeActive)
        handleFlyingMovement(deltaTime);
    else
        handleGroundMovement(deltaTime);

    // 3. Vertical Movement (Gravity & Jump)
    if (!isGodModeActive)
        applyPhysics(deltaTime);

    // 4. Matrix Update
    camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
}

void PlayerController::applyPhysics(float deltaTime) {
    // Add Gravity to the vertical speed
    if (!isGrounded)
        verticalVelocity += gravity * deltaTime;

    // Update position based on speed
    auto previousY = camera.Position.y;
    camera.Position.y += verticalVelocity * deltaTime;

    // Generate Box on new Position
    AABB playerBox = AABB::fromCenter(camera.Position, playerHalfExtent);

    // Check for Collision
    isGrounded = false;


    if (world.checkCollision(playerBox)) {

        if (verticalVelocity < 0) {
            camera.Position.y = previousY;
            verticalVelocity = 0.0f;
            isGrounded = true;
        } else {
            // If Hitting the ceiling
            camera.Position.y = previousY;
            verticalVelocity = 0.0f;
        }
    }

    // Jump Logic
    if (inputManager.isActionJustPressed(Action::JUMP) && isGrounded) {
        std::cout << "Jump" << std::endl;
        verticalVelocity = jumpForce;
        isGrounded = false;
    }

    // Simple ground-check
    if (camera.Position.y <= groundLevel) {
        camera.Position.y = groundLevel;
        verticalVelocity = 0.0f;
        isGrounded = true;
    }
}

void PlayerController::handleGroundMovement(float deltaTime) {
    if (inputManager.isActionJustPressed(Action::GOD_MODE)) {
        std::cout << "God Mode Activated" << std::endl;
        isGodModeActive = true;

        camera.SprintSpeed = 100.0f;
    }

    if (inputManager.isActionActive(Action::MOVE_FORWARD)) {
        camera.HandleGroundMovement(MOVE_FORWARD, deltaTime, world, playerHalfExtent);
    }

    if (inputManager.isActionActive(Action::MOVE_BACKWARD)) {
        camera.HandleGroundMovement(MOVE_BACKWARD, deltaTime, world, playerHalfExtent);
    }

    if (inputManager.isActionActive(Action::MOVE_LEFT)) {
        camera.HandleGroundMovement(MOVE_LEFT, deltaTime, world, playerHalfExtent);
    }

    if (inputManager.isActionActive(Action::MOVE_RIGHT)) {
        camera.HandleGroundMovement(MOVE_RIGHT, deltaTime, world, playerHalfExtent);
    }

    if (inputManager.isActionActive(Action::SPRINT))
        camera.HandleGroundMovement(SPRINT, deltaTime, world, playerHalfExtent);
    else
        camera.HandleGroundMovement(WALK, deltaTime, world, playerHalfExtent);

    if (inputManager.isActionJustPressed(Action::HIT))
        std::cout << "Hit" << std::endl;

    if (inputManager.isActionJustPressed(Action::PLACE))
        std::cout << "Place" << std::endl;

    if (inputManager.isActionJustPressed(Action::INTERACT))
        std::cout << "Interact" << std::endl;
}

void PlayerController::handleFlyingMovement(float deltaTime) {
    if (inputManager.isActionJustPressed(Action::GOD_MODE)) {
        std::cout << "God Mode Deactivated" << std::endl;
        isGodModeActive = false;

        camera.SprintSpeed = 10.0f;
    }

    if (inputManager.isActionActive(Action::MOVE_FORWARD)) {
        camera.HandleFlyingMovement(MOVE_FORWARD, deltaTime);
    }

    if (inputManager.isActionActive(Action::MOVE_BACKWARD)) {
        camera.HandleFlyingMovement(MOVE_BACKWARD, deltaTime);
    }

    if (inputManager.isActionActive(Action::MOVE_LEFT)) {
        camera.HandleFlyingMovement(MOVE_LEFT, deltaTime);
    }

    if (inputManager.isActionActive(Action::MOVE_RIGHT)) {
        camera.HandleFlyingMovement(MOVE_RIGHT, deltaTime);
    }

    if (inputManager.isActionActive(Action::SPRINT))
        camera.HandleFlyingMovement(SPRINT, deltaTime);
    else
        camera.HandleFlyingMovement(WALK, deltaTime);
}
