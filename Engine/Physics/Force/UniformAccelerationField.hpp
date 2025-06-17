#pragma once

#include <glm/vec3.hpp>

namespace Exodia {

    class UniformAccelerationField {
    
        public:

            explicit UniformAccelerationField(glm::vec3 uniformAcceleration) : _Acceleration(uniformAcceleration) {};

        public:

            glm::vec3 ComputeAcceleration()
            {
                return _Acceleration;
            }

        private:

            glm::vec3 _Acceleration;
    };
};
