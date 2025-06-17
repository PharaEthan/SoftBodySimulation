#pragma once

#include "Constraint.hpp"

#include <glm/geometric.hpp>
#include <iostream>

namespace Exodia {

    class GlobalVolumeConstraint : public Constraint {

        public:

            GlobalVolumeConstraint(std::vector<std::shared_ptr<Particle>> particles, std::vector<int> indices, float pressure, float compliance) : Constraint(particles, compliance, EQUALITY), _Pressure(pressure), _Indices(indices)
            {
                float volume = 0;

                for (int i = 0; i < _Indices.size(); i += 3) {
                    glm::vec3 t0 = _Particles[_Indices[i    ]]->Position;
                    glm::vec3 t1 = _Particles[_Indices[i + 1]]->Position;
                    glm::vec3 t2 = _Particles[_Indices[i + 2]]->Position;

                    volume += glm::dot(t0, glm::cross(t1, t2)) / 6.0f;
                }

                _RestVolume = volume;
            }

        public:

            float Evaluate() const override
            {
                float volume = 0;

                for (int i = 0; i < _Indices.size(); i += 3) {
                    glm::vec3 t0 = _Particles[_Indices[i    ]]->PredictedPosition;
                    glm::vec3 t1 = _Particles[_Indices[i + 1]]->PredictedPosition;
                    glm::vec3 t2 = _Particles[_Indices[i + 2]]->PredictedPosition;

                    volume += glm::dot(t0, glm::cross(t1, t2)) / 6.0f;
                }

                return volume - _RestVolume * _Pressure;
            }

        private:
        
            void ComputeGradient() override
            {
                std::fill(_Gradient.begin(), _Gradient.end(), glm::vec3(0.0f));

                for (int i = 0; i < _Indices.size(); i += 3) {
                    glm::vec3 p0 = _Particles[_Indices[i    ]]->PredictedPosition;
                    glm::vec3 p1 = _Particles[_Indices[i + 1]]->PredictedPosition;
                    glm::vec3 p2 = _Particles[_Indices[i + 2]]->PredictedPosition;

                    glm::vec3 g1 = glm::cross(p1, p2) / 6.0f;
                    glm::vec3 g2 = glm::cross(p2, p0) / 6.0f;
                    glm::vec3 g3 = glm::cross(p0, p1) / 6.0f;

                    _Gradient[_Indices[i    ]] += g1;
                    _Gradient[_Indices[i + 1]] += g2;
                    _Gradient[_Indices[i + 2]] += g3;
                }
            }

            void RecomputeTargetValue() override
            {
                float volume = 0;

                for (int i = 0; i < _Indices.size(); i += 3) {
                    glm::vec3 t0 = _Particles[_Indices[i    ]]->PredictedPosition;
                    glm::vec3 t1 = _Particles[_Indices[i + 1]]->PredictedPosition;
                    glm::vec3 t2 = _Particles[_Indices[i + 2]]->PredictedPosition;

                    volume += glm::dot(t0, glm::cross(t1, t2)) / 6.0f;
                }

                _RestVolume = volume;
            }

        public:

            void SetPressure(float pressure)
            {
                _Pressure = pressure;
            }

            float GetPressure() const
            {
                return _Pressure;
            }

        private:

            float _RestVolume {};
            float _Pressure   {};

            std::vector<int> _Indices;
    };
};
