//
// Created by Oetho on 29-01-2026.
//

#include "../Header Files/PlayerController.h"

void PlayerController::update(float deltaTime) {
    camera.HandleRotation(inputManager.getMouseDeltaX(), inputManager.getMouseDeltaY());
    CheckForMovement(deltaTime);
    camera.UpdateMatrix(45.0f, 0.1f, 100.0f);
}

void PlayerController::CheckForMovement(float deltaTime) {
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

    if (inputManager.isActionJustPressed(Action::JUMP))
        std::cout << "Jump" << std::endl;

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
