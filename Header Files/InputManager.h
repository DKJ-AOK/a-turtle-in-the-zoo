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
};

class InputManager {
private:
    std::map<Action, std::vector<int>> keyBindings;
    std::map<int, bool> currentKeyState;
    std::map<int, bool> previousKeyState;

public:
    InputManager();

    bool isActionActive(Action action);
    bool isActionJustPressed(Action action);
    void unbindKey(Action action, int keyCode);
    void unbindAllKeyBindings(Action action);
    void bindKey(Action action, int keyCode);
    void update(GLFWwindow* window);
};

#endif //A_TURTLE_IN_THE_ZOO_INPUTMANAGER_H