//
// Created by Oetho on 11-02-2026.
//

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "../../Header Files/HierarchicalStateMachine/SprintingState.h"
#include "../../Header Files/HierarchicalStateMachine/IdleState.h"
#include "../../Header Files/HierarchicalStateMachine/SneakingState.h"
#include "../../Header Files/PlayerController.h"

std::unique_ptr<MovementState> handleInput(const PlayerController& player, InputManager& input) {
    const glm::vec3 moveDir = player.calculateMoveDir();

    // If movement stopped -> Switch to IdleState
    if (glm::length(moveDir) == 0.0f) {
        return std::make_unique<IdleState>();
    }

    // If Sneak is active switch to SneakingState
    if (input.isActionActive(Action::SNEAK)) {
        return std::make_unique<SneakingState>();
    }

    return nullptr; // Otherwise stay in this State
}

void update(PlayerController& player, const float deltaTime) {
    player.handleHorizontalMovement(deltaTime, player.sprintSpeed);
}