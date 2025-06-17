#pragma once

#include "Constraint.hpp"
#include "Particle/Particle.hpp"

#include <glm/geometric.hpp>
#include <iostream>

namespace Exodia {

    class CollisionConstraint : public Constraint {

        public:

            CollisionConstraint(std::shared_ptr<Particle> q, std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3) : Constraint({ q, p1, p2, p3 }, 0.0f, INEQUALITY), _H(0.02f) {};

        public:
 
            float Evaluate() const override
            {
                glm::vec3 q  = _Particles[0]->PredictedPosition;
                glm::vec3 p1 = _Particles[1]->PredictedPosition;
                glm::vec3 p2 = _Particles[2]->PredictedPosition;
                glm::vec3 p3 = _Particles[3]->PredictedPosition;

                glm::vec3 n = glm::cross(p2 - p1, p3 - p1);

                if (glm::length(n) < 1e-3f)
                    return 0.0f;
                n = glm::normalize(n);

                return glm::dot(q - p1, n) - _H;
            }

        private:

            void ComputeGradient() override
            {
                glm::vec3 q  = _Particles[0]->PredictedPosition;
                glm::vec3 p1 = _Particles[1]->PredictedPosition;
                glm::vec3 p2 = _Particles[2]->PredictedPosition;
                glm::vec3 p3 = _Particles[3]->PredictedPosition;

                glm::vec3 p21 = p2 - p1;
                glm::vec3 p31 = p3 - p1;

                glm::vec3 n = glm::cross(p2 - p1, p3 - p1);
                
                if (glm::length(n) < 1e-3f) {
                    std::fill(_Gradient.begin(), _Gradient.end(), glm::vec3(0.0f));

                    return;
                }
                n = glm::normalize(n);

                glm::vec3 gradP2 = glm::cross(p21, p31);
                glm::vec3 gradP3 = glm::cross(p31, p21);

                glm::vec3 _qEigen = q - p1;

                _Gradient[0] = n;
                _Gradient[1] = gradP2 * _qEigen;
                _Gradient[2] = gradP3 * _qEigen;
                _Gradient[3] = -_Gradient[0] - _Gradient[1] - _Gradient[2];
            }

            void RecomputeTargetValue() override
            {
                _H = 0.02f;
            }

        private:

            float _H;
    };
};
