#ifndef A_TURTLE_IN_THE_ZOO_STATEMANAGER_H
#define A_TURTLE_IN_THE_ZOO_STATEMANAGER_H
#include <memory>

class GameState;
struct GameContext;
class StateManager {
private:
    std::unique_ptr<GameState> currentState;
    GameContext& gameContext;

    public:
    explicit StateManager(GameContext& ctx);

    void ChangeState(std::unique_ptr<GameState> newState);
    void Update(float deltaTime) const;
    void DrawUI() const;
};

#endif //A_TURTLE_IN_THE_ZOO_STATEMANAGER_H