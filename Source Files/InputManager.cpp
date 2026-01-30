//
// Created by Oetho on 29-01-2026.
//

#include "../Header Files/InputManager.h"
#include <ranges>
#include <GLFW/glfw3.h>


InputManager::InputManager(double screenWidth, double screenHeight) {
    lastX = screenWidth / 2, lastY = screenHeight / 2;

    bindKey(MOVE_FORWARD, KEYBOARD, GLFW_KEY_W);
    bindKey(MOVE_BACKWARD, KEYBOARD, GLFW_KEY_S);
    bindKey(MOVE_LEFT, KEYBOARD, GLFW_KEY_A);
    bindKey(MOVE_RIGHT, KEYBOARD, GLFW_KEY_D);
    bindKey(SPRINT, KEYBOARD, GLFW_KEY_LEFT_SHIFT);
    bindKey(JUMP, KEYBOARD, GLFW_KEY_SPACE);
    bindKey(INTERACT, KEYBOARD, GLFW_KEY_F);
    bindKey(HIT, MOUSE_BUTTON, GLFW_MOUSE_BUTTON_LEFT);
    bindKey(PLACE, MOUSE_BUTTON, GLFW_MOUSE_BUTTON_RIGHT);

    bindKey(EXIT_GAME, KEYBOARD, GLFW_KEY_ESCAPE);
    bindKey(GOD_MODE, KEYBOARD, GLFW_KEY_GRAVE_ACCENT);
}

void InputManager::bindKey(Action action, InputType inputType, int keyCode) {
    keyBindings[action].push_back({inputType, keyCode});
}

void InputManager::unbindAllKeyBindings(Action action) {
    if (keyBindings.contains(action))
        keyBindings[action].clear();
}

void InputManager::update(GLFWwindow *window) {
    // KEY INPUTS
    previousKeyStates = currentKeyStates;

    for (const auto &bindingList: keyBindings | std::views::values) {
        for (const auto& binding : bindingList) {
            bool pressed = false;
            if (binding.type == InputType::KEYBOARD) {
                pressed = (glfwGetKey(window, binding.id) == GLFW_PRESS);
            } else if (binding.type == InputType::MOUSE_BUTTON) {
                pressed = (glfwGetMouseButton(window, binding.id) == GLFW_PRESS);
            }
            currentKeyStates[binding] = pressed;
        }
    }

    // MOUSE MOVEMENTS
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    mouseDeltaX = static_cast<float>(xpos - lastX);
    mouseDeltaY = static_cast<float>(lastY - ypos);

    lastX = xpos;
    lastY = ypos;
}

bool InputManager::isActionActive(Action action) {
    for (const auto& binding: keyBindings[action]) {
        if (currentKeyStates[binding]) return true;
    }
    return false;
}

bool InputManager::isActionJustPressed(Action action) {
    for (const auto& binding : keyBindings[action]) {
        if (currentKeyStates[binding] && !previousKeyStates[binding]) return true;
    }
    return false;
}

float InputManager::getMouseDeltaX(float sensitivity) const { return mouseDeltaX * sensitivity; }
float InputManager::getMouseDeltaY(float sensitivity) const { return mouseDeltaY * sensitivity; }
