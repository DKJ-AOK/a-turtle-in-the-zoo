#ifndef A_TURTLE_IN_THE_ZOO_PLAYINGSTATE_H
#define A_TURTLE_IN_THE_ZOO_PLAYINGSTATE_H
#include <glm/vec3.hpp>

#include "../../Header Files/shaderClass.h"
#include "GameState.h"
#include "../../Header Files/Physics.h"

class PlayingState : public GameState {
public:
    explicit PlayingState(GameContext& ctx) :
    GameState(ctx),
    crosshairShader("../Resource Files/Shaders/crosshair.vert", "../Resource Files/Shaders/crosshair.frag"),
    selectionBoxShader("../Resource Files/Shaders/selection.vert", "../Resource Files/Shaders/selection.frag"){}

    void onEnter() override;
    void update(float deltaTime) override;
    void onExit() override;
    void drawUI() override;

private:
    void renderSelectionBox(glm::ivec3 blockPos);
    int modelLoc;
    int camMatrixLoc;
    unsigned int crosshairVAO, crosshairVBO, crosshairEBO;
    unsigned int selectionBoxVAO, selectionBoxVBO, selectionBoxEBO;
    Shader crosshairShader, selectionBoxShader;
    RaycastResult highlight;

};

#endif //A_TURTLE_IN_THE_ZOO_PLAYINGSTATE_H