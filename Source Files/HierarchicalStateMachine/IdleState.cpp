//
// Created by Oetho on 11-02-2026.
//
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "../../Header Files/HierarchicalStateMachine/IdleState.h"
#include "../../Header Files/HierarchicalStateMachine/SprintingState.h"
#include "../../Header Files/HierarchicalStateMachine/SneakingState.h"
#include "../../Header Files/HierarchicalStateMachine/WalkingState.h"
#include "../../Header Files/PlayerController.h"


std::unique_ptr<MovementState> IdleState::handleInput(PlayerController& player, InputManager& input) {
    const glm::vec3 moveDir = player.calculateMoveDir();

    // If we start to move
    if (glm::length(moveDir) > 0.0f) {
        // Check whether we should start directly in sprint or sneak mode.
        if (input.isActionActive(Action::SPRINT)) {
            return std::make_unique<SprintingState>();
        }
        if (input.isActionActive(Action::SNEAK)) {
            return std::make_unique<SneakingState>();
        }
        // Otherwise just walk
        return std::make_unique<WalkingState>();
    }

    return nullptr; // Bliv i Idle
}

void IdleState::update(PlayerController& player, float deltaTime) {
    // In Idle, we stand still. We still call movement with 0 speed
    // to ensure that the collision system still keeps us out of walls
    player.handleHorizontalMovement(deltaTime, 0.0f);
}
