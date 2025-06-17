#pragma once

#include "DistanceConstraint.hpp"

#include <glm/geometric.hpp>

namespace Exodia {

    /**
    * @brief https://carmencincotti.com/2022-09-05/the-most-performant-bending-constraint-of-xpbd/ Implementation.
    */
    class FastBendConstraint : public DistanceConstraint {

        public:

            FastBendConstraint(std::shared_ptr<Particle> p0, std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3, float compliance) : DistanceConstraint(p2, p3, compliance) {};
    };
};
