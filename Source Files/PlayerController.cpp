#include <iostream>
#include "../Header Files/PlayerController.h"
#include "../Header Files/Chunk.h"
#include "../Header Files/Physics.h"

void PlayerController::update(const float deltaTime) {
    // 1. Rotation (Mouse)
    camera.HandleRotation(inputManager.getMouseDeltaX() * sensitivity, inputManager.getMouseDeltaY() * sensitivity);

    // 2. Check God Mode State
    checkGodModeState();

    // 3. Horizontal Movement (WASD)
    if (isGodModeActive)
        handleFlyingMovement(deltaTime);
    else
        handleGroundMovement(deltaTime);

    // 4. Vertical Movement (Gravity & Jump)
    if (!isGodModeActive)
        applyPhysics(deltaTime);

    // 5. Matrix Update
    camera.UpdateMatrix(FOVdeg, nearPlane, farPlane);

    // 6. Player Actions
    handlePlayerActions();
}

void PlayerController::checkGodModeState() {
    if (inputManager.isActionJustPressed(Action::GOD_MODE)) {
        if (isGodModeActive) {
            std::cout << "God Mode Deactivated" << std::endl;
            isGodModeActive = false;
        } else {
            std::cout << "God Mode Activated" << std::endl;
            isGodModeActive = true;
        }
    }
}

void PlayerController::applyPhysics(const float deltaTime) {
    // 1. JUMP LOGIC
    if (inputManager.isActionJustPressed(Action::JUMP) && isGrounded) {
        verticalVelocity = jumpForce;
        isGrounded = false;
    }

    // 2. GRAVITY
    if (!isGrounded) {
        verticalVelocity += gravity * deltaTime;
    }

    // 3. MOVEMENT
    camera.Position.y += verticalVelocity * deltaTime;

    // 4. REAL COLLISION
    const AABB actualBox = AABB::fromCenter(camera.Position + PlayerCenterVec3, playerHalfExtent);

    if (AABB cubeBox; world.checkCollision(actualBox, cubeBox)) {
        if (verticalVelocity < 0) { // Falder ned i gulv
            camera.Position.y = cubeBox.max.y;
            verticalVelocity = 0.0f;
            isGrounded = true;
        }
        else if (verticalVelocity > 0) { // Rammer loftet
            camera.Position.y = cubeBox.min.y - playerCenter - 0.001f;
            verticalVelocity = 0.0f;
        }
    } else {
        // 5. GROUND PROBE (Kun hvis vi ikke allerede har fundet en kollision)
        // Her tjekker vi om vi er "tæt nok" på jorden til at kunne hoppe
        AABB groundProbe = actualBox;
        groundProbe.min.y -= 0.05f;

        if (world.checkCollision(groundProbe, cubeBox)) {
            isGrounded = true;
            // Vi nulstiller ikke hastigheden her, så tyngdekraften
            // stadig trækker os helt ned til "ægte" kollision
        } else {
            isGrounded = false;
        }
    }

    // 6. EMERGENCY GROUND LEVEL
    if (camera.Position.y <= groundLevel) {
        camera.Position.y = groundLevel;
        verticalVelocity = 0.0f;
        isGrounded = true;
    }
}

void PlayerController::handleGroundMovement(float deltaTime) {
    // 1. Find wanted direction
    glm::vec3 moveDir(0.0f);
    glm::vec3 forward = glm::normalize(glm::vec3(camera.Forward.x, 0.0f, camera.Forward.z));
    glm::vec3 right = camera.Right;

    if (inputManager.isActionActive(Action::MOVE_FORWARD))  moveDir += forward;
    if (inputManager.isActionActive(Action::MOVE_BACKWARD)) moveDir -= forward;
    if (inputManager.isActionActive(Action::MOVE_LEFT))     moveDir -= right;
    if (inputManager.isActionActive(Action::MOVE_RIGHT))    moveDir += right;

    if (glm::length(moveDir) > 0.0f) {
        float currentSpeed = inputManager.isActionActive(Action::SPRINT) ? sprintSpeed : walkingSpeed;
        glm::vec3 velocity = glm::normalize(moveDir) * currentSpeed * deltaTime;

        // 2. X-movement with collision
        camera.Position.x += velocity.x;
        AABB boxX = AABB::fromCenter(camera.Position + PlayerCenterVec3, playerHalfExtent);
        AABB hitBox;
        if (world.checkCollision(boxX, hitBox)) {
            if (velocity.x > 0) camera.Position.x = hitBox.min.x - playerHalfExtent.x - 0.01f;
            else camera.Position.x = hitBox.max.x + playerHalfExtent.x + 0.01f;
        }

        // 3. Z-movement with collision
        camera.Position.z += velocity.z;
        AABB boxZ = AABB::fromCenter(camera.Position + PlayerCenterVec3, playerHalfExtent);
        if (world.checkCollision(boxZ, hitBox)) {
            if (velocity.z > 0) camera.Position.z = hitBox.min.z - playerHalfExtent.z - 0.01f;
            else camera.Position.z = hitBox.max.z + playerHalfExtent.z + 0.01f;
        }
    }
}

void PlayerController::handleFlyingMovement(const float deltaTime) {
    const float currentSpeed = inputManager.isActionActive(Action::SPRINT) ? flyingSprintSpeed : flyingSpeed;
    const float velocity = currentSpeed * deltaTime;
    const glm::vec3 forward = camera.Forward * velocity;
    const glm::vec3 right = camera.Right * velocity;

    if (inputManager.isActionActive(Action::MOVE_FORWARD))  camera.Position += forward;
    if (inputManager.isActionActive(Action::MOVE_BACKWARD)) camera.Position -= forward;
    if (inputManager.isActionActive(Action::MOVE_LEFT))     camera.Position -= right;
    if (inputManager.isActionActive(Action::MOVE_RIGHT))    camera.Position += right;
}

void PlayerController::handlePlayerActions() const {
    if (inputManager.isActionJustPressed(Action::HIT)) {
        std::cout << "Trying To Hit a Solid Block" << std::endl;
        auto rayOrigin = camera.Position + PlayerEyeVec3;
        auto raycastResult = world.raycast(rayOrigin, camera.Forward, maxReachDistance);
        if (raycastResult.hit) {
            auto removedBlock = world.removeBlockAtWorldPosition(raycastResult.blockPos);
            std::cout << "RemovedBlock of Type: " + std::to_string(removedBlock) << std::endl;
        } else
            std::cout << "No Solid Block in Range" << std::endl;
    }

    if (inputManager.isActionJustPressed(Action::PLACE)) {
        std::cout << "Trying to Place a Solid Block" << std::endl;
        auto rayOrigin = camera.Position + PlayerEyeVec3;
        auto raycastResult = world.raycast(rayOrigin, camera.Forward, maxReachDistance);
        if (raycastResult.hit) {
            glm::ivec3 placePos = raycastResult.blockPos + raycastResult.normal;
            auto blockTypeToPlace = BlockType::DIRT;
            world.addBlockAtWorldPosition(placePos, blockTypeToPlace);
            std::cout << "Placed Block of Type: " + std::to_string(blockTypeToPlace) << std::endl;
        } else
            std::cout << "No Solid Block in Range to Place besides" << std::endl;
    }

    if (inputManager.isActionJustPressed(Action::INTERACT))
        std::cout << "Interact" << std::endl;
}
