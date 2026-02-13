#ifndef A_TURTLE_IN_THE_ZOO_GAMESTATE_H
#define A_TURTLE_IN_THE_ZOO_GAMESTATE_H
#include "GameContext.h"

class GameState {
protected:
    GameContext& gameContext;

public:
    explicit GameState(GameContext& ctx) : gameContext(ctx) {}
    virtual ~GameState() = default;

    virtual void onEnter() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void onExit() = 0;
    virtual void drawUI() = 0;
};

#endif //A_TURTLE_IN_THE_ZOO_GAMESTATE_H