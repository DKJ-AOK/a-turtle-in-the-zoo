#ifndef A_TURTLE_IN_THE_ZOO_PLAYERSTATE_H
#define A_TURTLE_IN_THE_ZOO_PLAYERSTATE_H
#include <memory>

class InputManager;
class PlayerController;

class MovementState {
public:
    virtual ~MovementState() = default;
    virtual std::unique_ptr<MovementState> handleInput(PlayerController& player, InputManager& input) = 0;
    virtual void update(PlayerController& player, float deltaTime) = 0;
};

#endif //A_TURTLE_IN_THE_ZOO_PLAYERSTATE_H