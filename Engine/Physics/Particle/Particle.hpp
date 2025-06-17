#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace Exodia {

    struct Particle {

        float Mass;
        float InverseMass;

        glm::vec3 InitialPosition   {};
        glm::vec3 Position          {};
        glm::vec3 PredictedPosition {};

        glm::vec3 Velocity {};

        unsigned long PositionIndex;

        std::vector<glm::vec3> ExternalForces {};

        Particle(float mass, glm::vec3 initialPosition, unsigned long positionIndex) : Mass(mass), InverseMass(mass == 0 ? 0 : 1 / mass), InitialPosition(initialPosition), Position(initialPosition), PositionIndex(positionIndex) {};

        glm::vec3 ResultingExternalForce()
        {
            glm::vec3 result = glm::vec3(0, 0, 0);

            for (auto force : ExternalForces)
                result += force;
            return result;
        }

        void Reset()
        {
            Position = InitialPosition;
            Velocity = glm::vec3(0, 0, 0);
        }
    };
};
