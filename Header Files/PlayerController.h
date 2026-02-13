#ifndef A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H
#define A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H
#include "Camera.h"
#include "InputManager.h"
#include "Physics.h"
#include "World.h"

class MovementState;

class PlayerController {
private:
    void handleFlyingMovement(float deltaTime);
    void handlePlayerActions() const;
    void checkGodModeState();
    void applyPhysics(float deltaTime);

    // Player Static Variables
    const float playerHeight = 1.90f;
    const float playerWidth = 0.60f;
    const float playerCenter = playerHeight / 2.0f;
    const float playerEyeLevel = playerHeight - 0.15f;
    const float playerEyeLevelFromCenter = playerEyeLevel - playerCenter;
    glm::vec3 playerHalfExtent = glm::vec3(playerWidth / 2.0f, playerCenter, playerWidth / 2.0f);

    // Player States
    std::unique_ptr<MovementState> currentMoveState;

    public:
    Camera camera;
    InputManager& inputManager;
    World& world;

    // Physics Variables
    float verticalVelocity = 0.0f;
    const float gravity = -9.81f; // Standard Earth-gravity
    const float jumpForce = 5.0f;
    bool isGrounded = false;
    float groundLevel = 0.0f;
    float walkingSpeed = 5.0f;
    float sprintSpeed = 10.0f;
    float sneakingSpeed = 3.0f;
    float flyingSpeed = 20.0f;
    float flyingSprintSpeed = 100.0f;

    // Player Preference Variables
    float sensitivity = 0.1f;
    float FOVdeg = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    // Player Static Variables
    const float maxReachDistance = 5.0f;
    const glm::vec3 PlayerHeightVec3 = glm::vec3(0, playerHeight, 0);
    const glm::vec3 PlayerCenterVec3 = glm::vec3(0, playerCenter, 0);
    const glm::vec3 PlayerEyeVec3 = glm::vec3(0, playerEyeLevel, 0);
    const glm::vec3 PlayerEyeFromCenterVec3 = PlayerEyeVec3 - PlayerCenterVec3;

    // God Mode
    bool isGodModeActive = false;

    PlayerController(InputManager& inputManagerRef, World& worldRef, int screenWidth, int screenHeight);
    ~PlayerController();

    void update(float deltaTime);
    [[nodiscard]] glm::vec3 calculateMoveDir() const;
    void handleHorizontalMovement(float deltaTime, float speed, bool avoidEdges = false);
    [[nodiscard]] bool isGroundAt(glm::vec3 pos) const;
};

#endif //A_TURTLE_IN_THE_ZOO_PLAYERCONTROLLER_H