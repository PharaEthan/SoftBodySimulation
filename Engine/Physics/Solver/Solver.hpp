#pragma once

#include "Mesh/Mesh.hpp"
#include "Particle/Particle.hpp"
#include "Constraints/Constraint.hpp"
#include "Bodies/Body.hpp"
#include "Constraints/CollisionConstraint.hpp"
#include "Force/UniformAccelerationField.hpp"

#include <vector>
#include <iostream>

namespace Exodia {

    class Solver {

        public:

            Solver() = default;

            ~Solver() = default;

    public:

        void AddBody(std::shared_ptr<Body> body)
        {
            if (body->GetMass() > 0)
                body->BuildParticleHierarchy(3);
            else
                body->BuildParticleHierarchy(1);
            _Bodies.push_back(body);
        }

        void RemoveBody(std::shared_ptr<Body> body)
        {
            _Bodies.erase(std::remove(_Bodies.begin(), _Bodies.end(), body), _Bodies.end());
        }

        void AddField(std::shared_ptr<UniformAccelerationField> field)
        {
            _Fields.push_back(field);
        }

		void RemoveField(std::shared_ptr<UniformAccelerationField> field)
		{
			_Fields.erase(std::remove(_Fields.begin(), _Fields.end(), field), _Fields.end());
		}

        void Reset()
        {
            for (const auto& body : _Bodies)
                body->Reset();
        }

        void Solve(float deltaTime)
        {
            OnBeforeSolve.NotifyObservers();

            for (const auto& body : _Bodies) {
                if (!body->GetMesh()->Enabled)
                    continue;
                for (const auto& particle : body->GetParticles()) {
                    for (const auto& field : _Fields) {

                        // F = m * a
                        particle->ExternalForces.push_back(field->ComputeAcceleration() * particle->Mass);
                    }
                }
            }

            float subTimeStep = deltaTime / (float)_Iterations;

            for (unsigned int i = 0; i < _Iterations; i++) {
                for (const auto& body : _Bodies) {
                    if (!body->GetMesh()->Enabled)
                        continue;
                    for (const auto& particle : body->GetParticles()) {
                        // a = F / m
                        // v = a * t
                        // v = t * F / m
                        particle->Velocity += subTimeStep * particle->InverseMass * particle->ResultingExternalForce();

                        particle->Velocity *= 0.999; // Damping, TODO: make it as a parameter
                    }
                }

                for (const auto& body : _Bodies) {
                    for (const auto& collisionConstraint : body->GetCollisionConstraints()) {
                        glm::vec3 t1 = collisionConstraint->GetParticles()[1]->Position;
                        glm::vec3 t2 = collisionConstraint->GetParticles()[2]->Position;
                        glm::vec3 t3 = collisionConstraint->GetParticles()[3]->Position;

                        glm::vec3 normal = glm::normalize(glm::cross(t2 - t1, t3 - t1));

                        for (const auto& particle : collisionConstraint->GetParticles()) {
                            glm::vec3 tangent = particle->Velocity - glm::dot(particle->Velocity, normal) * normal;

                            particle->Velocity = particle->Velocity - 0.05f * tangent;
                        }
                    }
                }

                for (const auto& body : _Bodies) {
                    if (!body->GetMesh()->Enabled)
                        continue;
                    for (const auto& particle : body->GetParticles())
                        particle->PredictedPosition = particle->Position + subTimeStep * particle->Velocity;
                }

                for (const auto& body : _Bodies) {
                    if (!body->GetMesh()->Enabled)
                        continue;
                    body->GetCollisionConstraints().clear();
                }

                for (int k_body = 0; k_body < _Bodies.size(); k_body++) {
                    auto body = _Bodies[k_body];

                    if (!body->GetMesh()->Enabled)
                        continue;
                    glm::mat4 bodyWorld = body->GetMesh()->Transform()->ComputeWorldMatrix();

                    for (int k_otherBody = k_body + 1; k_otherBody < _Bodies.size(); k_otherBody++) {
                        auto otherBody = _Bodies[k_otherBody];

                        if (!otherBody->GetMesh()->Enabled)
                            continue;
                        glm::mat4 otherBodyWorld = otherBody->GetMesh()->Transform()->ComputeWorldMatrix();

                        AABB *intersection = AABB::Intersection(body->GetMesh()->GetAABB(), otherBody->GetMesh()->GetAABB());

                        if (intersection == nullptr)
                            continue;
                        std::vector<std::shared_ptr<Particle>> bodyParticlesInIntersection;

                        for (const auto particleIndex : body->GetParticleIndicesPerLevel()[body->GetCollisionLevel()]) {
                            auto particle = body->GetParticles()[particleIndex];

                            if (!intersection->Contains(particle->PredictedPosition))
                                continue;
                            bodyParticlesInIntersection.push_back(particle);
                        }

                        std::vector<std::shared_ptr<Particle>> otherBodyParticlesInIntersection;

                        for (const auto particleIndex : otherBody->GetParticleIndicesPerLevel()[otherBody->GetCollisionLevel()]) {
                            auto particle = otherBody->GetParticles()[particleIndex];

                            if (!intersection->Contains(particle->PredictedPosition))
                                continue;
                            otherBodyParticlesInIntersection.push_back(particle);
                        }

                        std::vector<std::vector<GLint>> bodyTrianglesInIntersection;
                        std::vector<GLint> bodyIndices = body->GetTrianglesPerLevel()[body->GetCollisionLevel()];

                        for (unsigned int k = 0; k < bodyIndices.size(); k += 3) {
                            glm::vec3 t0 = body->GetParticles()[bodyIndices[k    ]]->PredictedPosition;
                            glm::vec3 t1 = body->GetParticles()[bodyIndices[k + 1]]->PredictedPosition;
                            glm::vec3 t2 = body->GetParticles()[bodyIndices[k + 2]]->PredictedPosition;

                            if (!intersection->IntersectsTriangle(t0, t1, t2))
                                continue;
                            bodyTrianglesInIntersection.push_back({ bodyIndices[k], bodyIndices[k + 1], bodyIndices[k + 2] });
                        }

                        std::vector<std::vector<GLint>> otherBodyTrianglesInIntersection;
                        std::vector<GLint> otherBodyIndices = otherBody->GetTrianglesPerLevel()[otherBody->GetCollisionLevel()];

                        for (unsigned int k = 0; k < otherBodyIndices.size(); k += 3) {
                            glm::vec3 t0 = otherBody->GetParticles()[otherBodyIndices[k    ]]->PredictedPosition;
                            glm::vec3 t1 = otherBody->GetParticles()[otherBodyIndices[k + 1]]->PredictedPosition;
                            glm::vec3 t2 = otherBody->GetParticles()[otherBodyIndices[k + 2]]->PredictedPosition;

                            if (!intersection->IntersectsTriangle(t0, t1, t2))
                                continue;
                            otherBodyTrianglesInIntersection.push_back({ otherBodyIndices[k], otherBodyIndices[k + 1], otherBodyIndices[k + 2] });
                        }

                        for (const auto& particle : otherBodyParticlesInIntersection) {
                            for (const auto& triangle : bodyTrianglesInIntersection) {
                                float t;

                                glm::vec3 particleNormal = {
                                    otherBody->GetMesh()->GetVertex().Normals[particle->PositionIndex],
                                    otherBody->GetMesh()->GetVertex().Normals[particle->PositionIndex + 1],
                                    otherBody->GetMesh()->GetVertex().Normals[particle->PositionIndex + 2]
                                };

                                particleNormal = glm::normalize(glm::vec3(otherBodyWorld * glm::vec4(particleNormal, 0.0f)));

                                if (particle->Mass != 0 && !Utils::RayTriangleIntersection(particle->PredictedPosition - particleNormal * 0.1f, particleNormal, body->GetParticles()[triangle[0]]->PredictedPosition, body->GetParticles()[triangle[1]]->PredictedPosition, body->GetParticles()[triangle[2]]->PredictedPosition, t))
                                    continue;
                                if (t > 0.2f)
                                    continue;

                                auto collisionConstraint = std::make_shared<CollisionConstraint>(particle, body->GetParticles()[triangle[0]], body->GetParticles()[triangle[1]], body->GetParticles()[triangle[2]]);

                                otherBody->AddCollisionConstraint(collisionConstraint);
                            }
                        }

                        for (const auto& particle : bodyParticlesInIntersection) {
                            for (const auto& triangle : otherBodyTrianglesInIntersection) {
                                float t;

                                glm::vec3 particleNormal = {
                                    body->GetMesh()->GetVertex().Normals[particle->PositionIndex    ],
                                    body->GetMesh()->GetVertex().Normals[particle->PositionIndex + 1],
                                    body->GetMesh()->GetVertex().Normals[particle->PositionIndex + 2]
                                };

                                particleNormal = glm::normalize(glm::vec3(bodyWorld * glm::vec4(particleNormal, 0.0f)));

                                if (particle->Mass != 0 && !Utils::RayTriangleIntersection(particle->PredictedPosition - particleNormal * 0.1f, particleNormal, otherBody->GetParticles()[triangle[0]]->PredictedPosition, otherBody->GetParticles()[triangle[1]]->PredictedPosition, otherBody->GetParticles()[triangle[2]]->PredictedPosition, t))
                                    continue;
                                if (t > 0.2f)
                                    continue;

                                auto collisionConstraint = std::make_shared<CollisionConstraint>(particle, otherBody->GetParticles()[triangle[0]], otherBody->GetParticles()[triangle[1]], otherBody->GetParticles()[triangle[2]]);

                                body->AddCollisionConstraint(collisionConstraint);
                            }
                        }

                        delete intersection;
                    }
                }

                for (const auto& body : _Bodies) {
                    if (!body->GetMesh()->Enabled)
                        continue;
                    for (int level = body->GetDistanceConstraintsPerLevel().size() - 1; level >= 0; level--) {
                        auto distanceConstraints = body->GetDistanceConstraintsPerLevel()[level];

                        for (const auto& distanceConstraint : distanceConstraints)
                            distanceConstraint->Solve(subTimeStep);
                    }

                    for (const auto& fastBendConstraint : body->GetFastBendConstraints())
                        fastBendConstraint->Solve(subTimeStep);
                    for (const auto& diheralConstraint : body->GetDihedralBendConstraints())
                        diheralConstraint->Solve(subTimeStep);
                    for (const auto& volumeConstraint : body->GetVolumeConstraints())
                        volumeConstraint->Solve(subTimeStep);
                    for (const auto& volumeConstraint : body->GetGlobalVolumeConstraints())
                        volumeConstraint->Solve(subTimeStep);
                    for (const auto& collisionConstraint : body->GetCollisionConstraints())
                        collisionConstraint->Solve(subTimeStep);
                    for (const auto& fixedConstraint : body->GetFixedConstraints())
                        fixedConstraint->Solve(subTimeStep);
                }

                for (const auto& body : _Bodies) {
                    if (!body->GetMesh()->Enabled)
                        continue;
                    for (const auto& particle : body->GetParticles()) {
                        particle->Velocity = (particle->PredictedPosition - particle->Position) / subTimeStep;
                        particle->Position =  particle->PredictedPosition;
                    }
                }
            }

            for (const auto& body : _Bodies) {
                if (!body->GetMesh()->Enabled)
                    continue;
                for (const auto& particle : body->GetParticles())
                    particle->ExternalForces.clear();
                body->UpdateVertex();
            }

            OnAfterSolve.NotifyObservers();
        }

        public:

            std::vector<std::shared_ptr<Body>>& GetBodies()
            {
                return _Bodies;
            }


            int GetNumberOfGetParticles()
            {
                int nbParticles = 0;

                for (const auto& body : _Bodies)
                    nbParticles += body->NumberOfParticles();
                return nbParticles;
            }

            void SetIterations(int iterations)
            {
                _Iterations = iterations;
            }

        public:

            Observable<> OnBeforeSolve;
            Observable<> OnAfterSolve;
  
        private:

            int _Iterations = 4;

            std::vector<std::shared_ptr<Body>>                     _Bodies;
            std::vector<std::shared_ptr<UniformAccelerationField>> _Fields;
    };
};
