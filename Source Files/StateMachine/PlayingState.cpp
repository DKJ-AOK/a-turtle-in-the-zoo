
#include "../../Header Files/StateMachine/PlayingState.h"
#include <iostream>
#include "../../Header Files/InputManager.h"
#include "../../Header Files/PlayerController.h"

void PlayingState::onEnter() {
    glfwSetInputMode(gameContext.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void PlayingState::update(const float deltaTime) {
    gameContext.inputManager->update(gameContext.window);
    gameContext.playerController->update(deltaTime);

    if (gameContext.inputManager->isActionJustPressed(Action::EXIT_GAME)) {
        std::cout << "Exit Game" << std::endl;
        glfwSetWindowShouldClose(gameContext.window, GLFW_TRUE);
    }

	gameContext.world->updateChunks(gameContext.playerController->camera.Position);
}

void PlayingState::onExit() {
    glfwSetInputMode(gameContext.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
