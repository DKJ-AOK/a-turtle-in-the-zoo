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

std::unique_ptr<MovementState> SneakingState::handleInput(PlayerController& player, InputManager& input) {
    const glm::vec3 moveDir = player.calculateMoveDir();

    // If Sneak Action keybind is not press, return to Walking if moving otherwise Idle
    if (!input.isActionActive(Action::SNEAK)) {
        if (glm::length(moveDir) > 0.0f) return std::make_unique<WalkingState>();
        return std::make_unique<IdleState>();
    }

    // If no movement, return Idle
    if (glm::length(moveDir) == 0.0f) {
        return std::make_unique<IdleState>();
    }

    return nullptr;
}

void SneakingState::update(PlayerController& player, const float deltaTime) {
    constexpr auto avoidEdges = true;
    player.handleHorizontalMovement(deltaTime, player.sneakingSpeed, avoidEdges);
}