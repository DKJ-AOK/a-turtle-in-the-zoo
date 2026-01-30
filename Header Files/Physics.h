//
// Created by Oetho on 30-01-2026.
//

#ifndef A_TURTLE_IN_THE_ZOO_PHYSICS_H
#define A_TURTLE_IN_THE_ZOO_PHYSICS_H
#include <glm/vec3.hpp>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    static AABB fromCenter(glm::vec3 center, glm::vec3 halfExtents) {
        return { center - halfExtents, center + halfExtents };
    }
};

#endif //A_TURTLE_IN_THE_ZOO_PHYSICS_H