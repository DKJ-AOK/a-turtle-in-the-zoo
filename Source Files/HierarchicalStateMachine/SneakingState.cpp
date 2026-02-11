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
    // Hvis vi ikke trykker sneak længere, gå tilbage til Walking eller Idle
    if (!input.isActionActive(Action::SNEAK)) {
        return std::make_unique<WalkingState>();
    }
    return nullptr;
}

void SneakingState::update(PlayerController& player, const float deltaTime) {
    // Vi bruger en lavere hastighed, f.eks. 2.0f
    player.handleHorizontalMovement(deltaTime, player.sneakingSpeed);
}