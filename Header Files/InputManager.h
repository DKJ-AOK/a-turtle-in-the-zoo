//
// Created by Oetho on 29-01-2026.
//

#ifndef A_TURTLE_IN_THE_ZOO_INPUTMANAGER_H
#define A_TURTLE_IN_THE_ZOO_INPUTMANAGER_H
#include <map>
#include <vector>
#include <GLFW/glfw3.h>

enum Action {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP,
    INTERACT,
    HIT,
    PLACE,
    EXIT_GAME,
};

enum InputType {
    KEYBOARD,
    MOUSE_BUTTON,
};

struct InputBinding {
    InputType type;
    int id;

    bool operator<(const InputBinding& other) const {
        if (type != other.type) return type < other.type;
        return id < other.id;
    }
};

class InputManager {
private:
    std::map<Action, std::vector<InputBinding>> keyBindings;
    std::map<InputBinding, bool> currentKeyStates;
    std::map<InputBinding, bool> previousKeyStates;

    double lastX, lastY;
    float mouseDeltaX = 0.0f, mouseDeltaY = 0.0f;
    bool firstMouse = true;

public:
    InputManager(double screenWidth, double screenHeight);

    bool isActionActive(Action action);
    bool isActionJustPressed(Action action);
    void unbindAllKeyBindings(Action action);
    void bindKey(Action action, InputType inputType, int keyCode);
    void update(GLFWwindow* window);
    [[nodiscard]] float getMouseDeltaX(float sensitivity = 0.1f) const;
    [[nodiscard]] float getMouseDeltaY(float sensitivity = 0.1f) const;
};

#endif //A_TURTLE_IN_THE_ZOO_INPUTMANAGER_H