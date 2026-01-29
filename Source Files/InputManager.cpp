//
// Created by Oetho on 29-01-2026.
//

#include "../Header Files/InputManager.h"

#include <algorithm>
#include <ranges>
#include <GLFW/glfw3.h>


InputManager::InputManager() {
    keyBindings[Action::MOVE_FORWARD] = {GLFW_KEY_W, GLFW_KEY_UP};
    keyBindings[Action::MOVE_BACKWARD] = {GLFW_KEY_S, GLFW_KEY_DOWN};
    keyBindings[Action::MOVE_LEFT] = {GLFW_KEY_A, GLFW_KEY_LEFT};
    keyBindings[Action::MOVE_RIGHT] = {GLFW_KEY_D, GLFW_KEY_RIGHT};
    keyBindings[Action::JUMP] = {GLFW_KEY_SPACE};
    keyBindings[Action::HIT] = {GLFW_MOUSE_BUTTON_LEFT};
    keyBindings[Action::INTERACT] = {GLFW_KEY_F};
    keyBindings[Action::PLACE] = {GLFW_MOUSE_BUTTON_RIGHT};
}

void InputManager::bindKey(Action action, int keyCode) {
    keyBindings[action].push_back(keyCode);
}

void InputManager::unbindKey(Action action, int keyCode) {
    auto it = keyBindings.find(action);
    if (it != keyBindings.end()) {
        auto &keyList = it->second;
        keyList.erase(std::remove(keyList.begin(), keyList.end(), keyCode), keyList.end());
    }
}

void InputManager::unbindAllKeyBindings(Action action) {
    if (keyBindings.contains(action))
        keyBindings[action].clear();
}

void InputManager::update(GLFWwindow *window) {
    previousKeyState = currentKeyState;

    for (const auto &keys: keyBindings | std::views::values) {
        for (int key: keys) {
            currentKeyState[key] = (glfwGetKey(window, key) == GLFW_PRESS);
        }
    }
}

bool InputManager::isActionActive(Action action) {
    for (int key: keyBindings[action]) {
        if (currentKeyState[key]) return true;
    }
    return false;
}

bool InputManager::isActionJustPressed(Action action) {
    for (int key : keyBindings[action]) {
        if (currentKeyState[key] && !previousKeyState[key]) return true;
    }
    return false;
}
