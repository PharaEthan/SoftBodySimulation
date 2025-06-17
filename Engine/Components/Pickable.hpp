#pragma once

#include "Ray/PickResult.hpp"

namespace Exodia {

    class Pickable {

        public:

            virtual PickResult Pick(glm::vec3 rayOrigin, glm::vec3 rayDirection) = 0;

        public:

			virtual void SetPickingEnabled(bool enabled) = 0;
			virtual bool IsPickingEnabled() const = 0;
    };
};
