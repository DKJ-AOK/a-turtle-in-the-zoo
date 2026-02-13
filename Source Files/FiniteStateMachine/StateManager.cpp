#include "../../Header Files/FiniteStateMachine/StateManager.h"
#include "../../Header Files/FiniteStateMachine/GameState.h" // Nu inkluderer vi den her, hvor den skal bruges
#include "../../Header Files/FiniteStateMachine/GameContext.h"

StateManager::StateManager(GameContext& ctx) : gameContext(ctx) {}

void StateManager::ChangeState(std::unique_ptr<GameState> newState) {
    if (currentState) currentState->onExit();
    currentState = std::move(newState);
    currentState->onEnter();
}

void StateManager::Update(const float deltaTime) const {
    if (currentState) currentState->update(deltaTime);
}

void StateManager::DrawUI() const {
    if (currentState) currentState->drawUI();
}
