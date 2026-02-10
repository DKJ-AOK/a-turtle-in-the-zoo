#include <glad/gl.h>
#include <iostream>

#include "../../Header Files/StateMachine/PlayingState.h"
#include "../../Header Files/InputManager.h"
#include "../../Header Files/PlayerController.h"
#include "../../Header Files/VBO.h"

void PlayingState::onEnter() {
    glfwSetInputMode(gameContext.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    constexpr float crosshairVertices[] = {
        // En vandret linje og en lodret linje omkring (0,0)
        -0.02f, 0.0f, 0.02f, 0.0f, // Vandret
        0.0f, -0.03f, 0.0f, 0.03f // Lodret (lidt længere pga. aspect ratio)
    };

    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);

    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    constexpr float selectionBoxVertices[] = {
        // Front face
        0.0f, 0.0f, 0.0f, // 0: Bund-Venstre-Bag
        1.0f, 0.0f, 0.0f, // 1: Bund-Højre-Bag
        1.0f, 1.0f, 0.0f, // 2: Top-Højre-Bag
        0.0f, 1.0f, 0.0f, // 3: Top-Venstre-Bag
        0.0f, 0.0f, 1.0f, // 4: Bund-Venstre-Front
        1.0f, 0.0f, 1.0f, // 5: Bund-Højre-Front
        1.0f, 1.0f, 1.0f, // 6: Top-Højre-Front
        0.0f, 1.0f, 1.0f // 7: Top-Venstre-Front
    };

    unsigned int selectionBoxIndices[] = {
        0, 1, 1, 2, 2, 3, 3, 0, // Bag-firkant
        4, 5, 5, 6, 6, 7, 7, 4, // Front-firkant
        0, 4, 1, 5, 2, 6, 3, 7 // Forbindere
    };

    glGenVertexArrays(1, &selectionBoxVAO);
    glGenBuffers(1, &selectionBoxVBO);
    glGenBuffers(1, &selectionBoxEBO); // Tilføj denne variabel til din header

    glBindVertexArray(selectionBoxVAO);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, selectionBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(selectionBoxVertices), selectionBoxVertices, GL_STATIC_DRAW);

    // EBO (VIGTIGT for glDrawElements)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selectionBoxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(selectionBoxIndices), selectionBoxIndices, GL_STATIC_DRAW);

    // Vertex Attribs: 3 floats (X, Y, Z), ikke 2!
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void PlayingState::update(const float deltaTime) {
    gameContext.inputManager->update(gameContext.window);
    gameContext.playerController->update(deltaTime);

    glm::vec3 eyePos = gameContext.playerController->camera.Position + glm::vec3(0, 1.75f, 0);
    highlight = gameContext.world->raycast(eyePos, gameContext.playerController->camera.Forward, 5.0f);

    if (gameContext.inputManager->isActionJustPressed(Action::EXIT_GAME)) {
        std::cout << "Exit Game" << std::endl;
        glfwSetWindowShouldClose(gameContext.window, GLFW_TRUE);
    }

    gameContext.world->updateChunks(gameContext.playerController->camera.Position);
}

void PlayingState::onExit() {
    glfwSetInputMode(gameContext.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void PlayingState::drawUI() {
    if (highlight.hit) {
        renderSelectionBox(highlight.blockPos);
    }

    glDisable(GL_DEPTH_TEST); // Sørg for at det ikke forsvinder bag blokke

    crosshairShader.Activate();
    glBindVertexArray(crosshairVAO);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 4);

    glEnable(GL_DEPTH_TEST); // Tænd for dybde igen til næste frame
}

void PlayingState::renderSelectionBox(const glm::ivec3 blockPos) {
    selectionBoxShader.Activate();

    // Set your camera matrix
    glUniformMatrix4fv(glGetUniformLocation(selectionBoxShader.ID, "camMatrix"), 1, GL_FALSE,
                       glm::value_ptr(gameContext.playerController->camera.cameraMatrix));

    auto model = glm::mat4(1.0f);
    const auto snapPos = glm::vec3(
        static_cast<float>(blockPos.x),
        static_cast<float>(blockPos.y),
        static_cast<float>(blockPos.z)
    );

    std::cout << "x: " + std::to_string(blockPos.x)
            << " " << "y: " + std::to_string(blockPos.y)
            << " " << "z: " + std::to_string(blockPos.z) << std::endl;

    // 1. Move to the block corner returned by raycast
    model = glm::translate(model, snapPos);

    // 2. Scale slightly up from the corner (e.g., 1.002)
    // This ensures the box is just a tiny bit larger than the block
    model = glm::scale(model, glm::vec3(1.002f));

    glUniformMatrix4fv(glGetUniformLocation(selectionBoxShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(selectionBoxVAO);
    glLineWidth(2.0f);

    // Use GL_LINES to draw the wireframe
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
