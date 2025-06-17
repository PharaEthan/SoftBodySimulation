#pragma once

#include "Constraint.hpp"
#include "Particle/Particle.hpp"

#include <glm/geometric.hpp>
#include <iostream>

namespace Exodia {

    class FixedConstraint : public Constraint {

        public:

            FixedConstraint(std::shared_ptr<Particle> p) : Constraint({ p }, 0.0f, EQUALITY), _TargetPosition(p->Position) {};

            float Evaluate() const override
            {
                return glm::length(_Particles[0]->PredictedPosition - _TargetPosition);
            }

        private:

            void ComputeGradient() override
            {
                glm::vec3 p1 = _Particles[0]->PredictedPosition;
                glm::vec3 p2 = _TargetPosition;

                glm::vec3 g1 = glm::normalize(p1 - p2);

                _Gradient[0] = g1;
            }

            void RecomputeTargetValue() override
            {
                _TargetPosition = _Particles[0]->PredictedPosition;
            }

        private:

            glm::vec3 _TargetPosition;
    };
};
