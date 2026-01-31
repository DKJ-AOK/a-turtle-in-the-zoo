#include "../../Header Files/StateMachine/StateManager.h"
#include "../../Header Files/StateMachine/GameState.h" // Nu inkluderer vi den her, hvor den skal bruges
#include "../../Header Files/StateMachine/GameContext.h"

StateManager::StateManager(GameContext& ctx) : gameContext(ctx) {}

void StateManager::ChangeState(std::unique_ptr<GameState> newState) {
    if (currentState) currentState->onExit();
    currentState = std::move(newState);
    currentState->onEnter();
}

void StateManager::Update(const float deltaTime) {
    if (currentState) currentState->update(deltaTime);
}
