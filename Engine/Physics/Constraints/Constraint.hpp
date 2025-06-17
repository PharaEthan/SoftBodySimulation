#pragma once

#include <functional>
#include <utility>
#include <memory>
#include <glm/glm.hpp>
#include <iostream>

#include "Particle/Particle.hpp"

namespace Exodia {

    enum ConstraintType {
        INEQUALITY,
        EQUALITY
    };

    class Constraint {

        public:

            Constraint(std::vector<std::shared_ptr<Particle>> particles, float compliance, ConstraintType type) : _Cardinality(particles.size()), _Particles(std::move(particles)), _Compliance(compliance), _Type(type)
            {
                _Gradient = std::vector<glm::vec3>(_Cardinality, glm::vec3(0.0f));
            }

            Constraint(const Constraint& other) : _Cardinality(other._Particles.size()), _Particles(other._Particles), _Compliance(other._Compliance), _Type(other._Type)
            {
                _Gradient = std::vector<glm::vec3>(_Cardinality, glm::vec3(0.0f));
            }

        public:

            virtual float Evaluate() const = 0;
    
        protected:

            virtual void ComputeGradient() = 0;
            virtual void RecomputeTargetValue() = 0;

        public:

            void Solve(float deltaTime)
            {
                if (IsSatisfied())
                    return;
                ComputeGradient();

                float xpbdFactor      = _Compliance / (deltaTime * deltaTime);
                float constraintValue = Evaluate();
                float numerator       = -constraintValue - xpbdFactor * _Lambda;
                float denominator     = xpbdFactor;

                for (unsigned int i = 0; i < _Particles.size(); i++)
                    denominator += _Particles[i]->InverseMass * glm::dot(_Gradient[i], _Gradient[i]);
                float deltaLambda = 0.0;

                if (denominator < 1e-6)
                    deltaLambda = 0.0;
                else
                    deltaLambda = numerator / denominator;
                if (std::isnan(deltaLambda))
                    deltaLambda = 0.0;
                for (unsigned int i = 0; i < _Particles.size(); i++) {
                    if (denominator < 1e-6)
                        continue;
                    _Particles[i]->PredictedPosition += (-constraintValue / denominator) * _Particles[i]->InverseMass * _Gradient[i];
                }

                _Lambda += deltaLambda;
            }

            void ReplaceParticle(std::shared_ptr<Particle> oldParticle, std::shared_ptr<Particle> newParticle)
            {
                for (auto &particle : _Particles) {
                    if (particle != oldParticle)
                        continue;
                    particle = newParticle;
                }

                RecomputeTargetValue();
            }

        public:

            std::vector<std::shared_ptr<Particle>> GetParticles() const
            {
                return _Particles;
            }

            void SetParticles(std::vector<std::shared_ptr<Particle>> particles)
            {
                _Particles = std::move(particles);
            }

            void SetCompliance(float compliance)
            {
                _Compliance = compliance;
            }

            float GetCompliance() const
            {
                return _Compliance;
            }

        protected:

            bool IsSatisfied() const
            {
                switch (_Type) {
                    case INEQUALITY:
                        return Evaluate() >= 0;
                    case EQUALITY:
                        return fabsf(Evaluate()) <= 1e-6;
                }

                throw std::runtime_error("Constraint type not handled");
            }

        protected:

            unsigned int _Cardinality {};

            std::vector<std::shared_ptr<Particle>> _Particles;

            float _Compliance = 0.0f;

            float _Lambda {};

            ConstraintType _Type;

            std::vector<glm::vec3> _Gradient;
    };
};
