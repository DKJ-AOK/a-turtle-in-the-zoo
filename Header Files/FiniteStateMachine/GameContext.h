#ifndef A_TURTLE_IN_THE_ZOO_GAMECONTEXT_H
#define A_TURTLE_IN_THE_ZOO_GAMECONTEXT_H
#include <glfw/glfw3.h>

class InputManager;
class World;
struct GLFWwindow;
class PlayerController;

struct GameContext {
    GLFWwindow* window;
    InputManager* inputManager;
    World* world;
    PlayerController* playerController;
};


#endif //A_TURTLE_IN_THE_ZOO_GAMECONTEXT_H