#include "../Header Files/InputManager.h"

#include <algorithm>
#include <ranges>
#include <GLFW/glfw3.h>


InputManager::InputManager(const double screenWidth, const double screenHeight) {
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

void InputManager::bindKey(const Action action, const InputType inputType, const int keyCode) {
    const InputBinding binding = {inputType, keyCode};
    keyBindings[action].push_back(binding);
    currentKeyStates[binding] = false;
}

void InputManager::unbindAllKeyBindings(const Action action) {
    if (keyBindings.contains(action))
        keyBindings[action].clear();
}

void InputManager::update(GLFWwindow *window) {
    // KEY INPUTS
    previousKeyStates = currentKeyStates;

    for (auto& [binding, state] : currentKeyStates) {
        if (binding.type == InputType::KEYBOARD) {
            state = (glfwGetKey(window, binding.id) == GLFW_PRESS);
        } else {
            state = (glfwGetMouseButton(window, binding.id) == GLFW_PRESS);
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

bool InputManager::isActionActive(const Action action) {
    const auto it = keyBindings.find(action);
    if (it == keyBindings.end()) return false;

    return std::ranges::any_of(it->second, [this](const auto& binding) {
            return currentKeyStates[binding];
        });
}

bool InputManager::isActionJustPressed(const Action action) {
    const auto it = keyBindings.find(action);
    if (it == keyBindings.end()) return false;

    return std::ranges::any_of(it->second, [this](const auto& binding) {
        return currentKeyStates[binding] && !previousKeyStates[binding];
    });
}

float InputManager::getMouseDeltaX() const { return mouseDeltaX; }
float InputManager::getMouseDeltaY() const { return mouseDeltaY; }
