#ifndef A_TURTLE_IN_THE_ZOO_PLAYINGSTATE_H
#define A_TURTLE_IN_THE_ZOO_PLAYINGSTATE_H
#include "GameState.h"

class PlayingState : public GameState {
public:
    explicit PlayingState(GameContext& ctx) : GameState(ctx) {}

    void onEnter() override;
    void update(float deltaTime) override;
    void onExit() override;
};

#endif //A_TURTLE_IN_THE_ZOO_PLAYINGSTATE_H