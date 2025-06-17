#pragma once

#include "Constraint.hpp"

#include <glm/geometric.hpp>

namespace Exodia {

    class DistanceConstraint : public Constraint {
    
        public:

            DistanceConstraint(std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, float compliance) : Constraint({ p1, p2 }, compliance, EQUALITY)
            {
                _RestLength = glm::length(p1->Position - p2->Position);
            }

            DistanceConstraint(const DistanceConstraint& other) : Constraint(other)
            {
                _RestLength = other._RestLength;
            }

            float Evaluate() const override
            {
                return glm::length(_Particles[0]->PredictedPosition - _Particles[1]->PredictedPosition) - _RestLength;
            }

        private:

            void ComputeGradient() override
            {
                glm::vec3 p1 = _Particles[0]->PredictedPosition;
                glm::vec3 p2 = _Particles[1]->PredictedPosition;

                glm::vec3 g1 = glm::normalize(p1 - p2);
                glm::vec3 g2 = -g1;

                _Gradient[0] = g1;
                _Gradient[1] = g2;
            }

            void RecomputeTargetValue() override
            {
                _RestLength = glm::length(_Particles[0]->Position - _Particles[1]->Position);
            }

        private:

            float _RestLength {};
    };
};
