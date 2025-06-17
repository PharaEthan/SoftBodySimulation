#pragma once

#include "Constraint.hpp"

#include <glm/geometric.hpp>

namespace Exodia {

    class VolumeConstraint : public Constraint {

        public:

            VolumeConstraint(std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3, std::shared_ptr<Particle> p4, float restVolume, float compliance) : Constraint({ p1, p2, p3, p4 }, compliance, EQUALITY), _RestVolume(restVolume) {};

        public:

            float Evaluate() const override
            {
                glm::vec3 p1 = _Particles[0]->PredictedPosition;
                glm::vec3 p2 = _Particles[1]->PredictedPosition;
                glm::vec3 p3 = _Particles[2]->PredictedPosition;
                glm::vec3 p4 = _Particles[3]->PredictedPosition;

                glm::vec3 cross = glm::cross(p2 - p1, p3 - p1) / 6.0f;

                return glm::dot(cross, (p4 - p1)) - _RestVolume;
            }

    private:

        void ComputeGradient() override
        {
            glm::vec3 p0 = _Particles[0]->PredictedPosition;
            glm::vec3 p1 = _Particles[1]->PredictedPosition;
            glm::vec3 p2 = _Particles[2]->PredictedPosition;
            glm::vec3 p3 = _Particles[3]->PredictedPosition;

            glm::vec3 g1 = glm::cross(p2 - p0, p3 - p0) / 6.0f;
            glm::vec3 g2 = glm::cross(p3 - p0, p1 - p0) / 6.0f;
            glm::vec3 g3 = glm::cross(p1 - p0, p2 - p0) / 6.0f;
            glm::vec3 g0 = -g1 - g2 - g3;

            _Gradient[0] = g0;
            _Gradient[1] = g1;
            _Gradient[2] = g2;
            _Gradient[3] = g3;
        }

        void RecomputeTargetValue() override
        {
            glm::vec3 p1 = _Particles[0]->PredictedPosition;
            glm::vec3 p2 = _Particles[1]->PredictedPosition;
            glm::vec3 p3 = _Particles[2]->PredictedPosition;
            glm::vec3 p4 = _Particles[3]->PredictedPosition;

            glm::vec3 cross = glm::cross(p2 - p1, p3 - p1) / 6.0f;

            _RestVolume = glm::dot(cross, (p4 - p1));
        }

        private:

            float _RestVolume;
    };
};
