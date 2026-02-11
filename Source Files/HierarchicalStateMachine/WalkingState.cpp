//
// Created by Oetho on 11-02-2026.
//

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "../../Header Files/HierarchicalStateMachine/WalkingState.h"
#include "../../Header Files/HierarchicalStateMachine/IdleState.h"
#include "../../Header Files/HierarchicalStateMachine/SprintingState.h"
#include "../../Header Files/HierarchicalStateMachine/SneakingState.h"
#include "../../Header Files/PlayerController.h"

std::unique_ptr<MovementState> WalkingState::handleInput(PlayerController& player, InputManager& input) {
    glm::vec3 moveDir = player.calculateMoveDir();

    // 1. If we stop moving -> Idle
    if (glm::length(moveDir) == 0.0f) {
        return std::make_unique<IdleState>();
    }

    // 2. If we press Sprint Action Keybind -> Sprinting
    if (input.isActionActive(Action::SPRINT)) {
        return std::make_unique<SprintingState>();
    }

    // 3. If we press Sneak Action Keybind -> Sneaking
    if (input.isActionActive(Action::SNEAK)) {
        return std::make_unique<SneakingState>();
    }

    return nullptr; // Otherwise stay in Walking
}

void WalkingState::update(PlayerController& player, const float deltaTime) {
    // Use walking speed
    player.handleHorizontalMovement(deltaTime, player.walkingSpeed);
}