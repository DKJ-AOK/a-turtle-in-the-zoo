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
    camera.UpdateMatrix(45.0f, 0.1f, 200.0f);
}

void PlayerController::applyPhysics(float deltaTime) {
    // 1. JUMP LOGIC (Vi tjekker på værdien fra SIDSTE frame)
    if (inputManager.isActionJustPressed(Action::JUMP) && isGrounded) {
        verticalVelocity = jumpForce;
        isGrounded = false;
    }

    // 2. TYNGDEKRAFT
    if (!isGrounded) {
        verticalVelocity += gravity * deltaTime;
    }

    // 3. BEVÆGELSE
    camera.Position.y += verticalVelocity * deltaTime;

    // 4. ÆGTE KOLLISION (Brug den rigtige størrelse her!)
    AABB actualBox = AABB::fromCenter(camera.Position, playerHalfExtent);
    AABB cubeBox;

    if (world.checkCollision(actualBox, cubeBox)) {
        if (verticalVelocity < 0) { // Falder ned i gulv
            camera.Position.y = cubeBox.max.y + playerHalfExtent.y + 0.001f;
            verticalVelocity = 0.0f;
            isGrounded = true;
        }
        else if (verticalVelocity > 0) { // Rammer loftet
            camera.Position.y = cubeBox.min.y - playerHalfExtent.y - 0.001f;
            verticalVelocity = 0.0f;
        }
    } else {
        // 5. GROUND PROBE (Kun hvis vi ikke allerede har fundet en kollision)
        // Her tjekker vi om vi er "tæt nok" på jorden til at kunne hoppe
        AABB groundProbe = actualBox;
        groundProbe.min.y -= 0.05f;

        if (world.checkCollision(groundProbe, cubeBox)) {
            isGrounded = true;
            // Vi nulstiller ikke hastigheden her, så tyngdekraften
            // stadig trækker os helt ned til "ægte" kollision
        } else {
            isGrounded = false;
        }
    }

    // 6. EMERGENCY GROUND LEVEL
    if (camera.Position.y <= groundLevel) {
        camera.Position.y = groundLevel + playerHalfExtent.y;
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
