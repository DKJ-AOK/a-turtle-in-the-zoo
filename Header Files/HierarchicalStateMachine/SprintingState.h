//
// Created by Oetho on 11-02-2026.
//

#ifndef A_TURTLE_IN_THE_ZOO_SPRINTINGSTATE_H
#define A_TURTLE_IN_THE_ZOO_SPRINTINGSTATE_H
#include "MovementState.h"

class IdleState;
class SneakingState;

class SprintingState : public MovementState {
    public:
    std::unique_ptr<MovementState> handleInput(PlayerController& player, InputManager& input) override;
    void update(PlayerController& player, float deltaTime) override;
};

#endif //A_TURTLE_IN_THE_ZOO_SPRINTINGSTATE_H