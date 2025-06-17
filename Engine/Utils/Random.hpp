#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/random.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>

namespace Exodia {

    class Random {

        public:

            static glm::vec3 RandomDirection()
            {
                float theta = glm::linearRand(0.0f, 2.0f * glm::pi<float>());
                float phi = glm::linearRand(0.0f, glm::pi<float>());

                return { glm::sin(phi) * glm::cos(theta), glm::cos(phi), glm::sin(phi) * glm::sin(theta) };
            }

            static glm::vec3 RandomColor()
            {
                return { glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f) };
            }
    };
};
