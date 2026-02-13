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
    // If Sneak Action keybind is not press, return to Walking
    if (!input.isActionActive(Action::SNEAK)) {
        return std::make_unique<WalkingState>();
    }
    return nullptr;
}

void SneakingState::update(PlayerController& player, const float deltaTime) {
    // Use sneaking speed
    player.handleHorizontalMovement(deltaTime, player.sneakingSpeed);
}