#pragma once

#include "Mesh/Mesh.hpp"
#include "Particle/Particle.hpp"
#include "Constraints/Constraint.hpp"
#include "Constraints/DistanceConstraint.hpp"
#include "Constraints/FixedConstraint.hpp"
#include "Constraints/CollisionConstraint.hpp"
#include "Constraints/FastBendConstraint.hpp"
#include "Constraints/VolumeConstraint.hpp"
#include "Constraints/DihedralBendConstraint.hpp"
#include "Constraints/GlobalVolumeConstraint.hpp"

namespace Exodia {

    class Body {

        public:

            Body(std::shared_ptr<Mesh> mesh, float mass) : _Mesh(mesh), _Mass(mass)
            {
                _Mesh->BakeRotationIntoGetVertex();
                _Mesh->BakeScalingIntoGetVertex();

                auto meshPosition = _Mesh->Transform()->Position;

                unsigned int nbParticles = mesh->GetVertex().Positions.size() / 3;

                _Particles.reserve(nbParticles);

                for (unsigned int i = 0; i < mesh->GetVertex().Positions.size(); i += 3) {
                    glm::vec3 particlePosition = { mesh->GetVertex().Positions[i], mesh->GetVertex().Positions[i + 1], mesh->GetVertex().Positions[i + 2] };

                    _Particles.push_back(std::make_shared<Particle>(mass / (float)nbParticles, particlePosition + meshPosition, i));
                }
            };

            void BuildParticleHierarchy(int nbLevels)
            {
                std::vector<std::vector<int>> triangulations;

                triangulations.push_back(_Mesh->GetVertex().Indices);

                std::vector<std::vector<int>> particleIndicesPerLevel;

                particleIndicesPerLevel.push_back(std::vector<int>());

                for (int i = 0; i < _Particles.size(); i++)
                    particleIndicesPerLevel[0].push_back(i);
                std::vector<std::vector<int>> closestCoarseVertexIndicesPerLevel;

                closestCoarseVertexIndicesPerLevel.push_back(std::vector<int>());

                for (int i = 0; i < _Particles.size(); i++)
                    closestCoarseVertexIndicesPerLevel[0].push_back(i);

                for (unsigned int level = 1; level <= nbLevels; level++) {
                    std::vector<int> coarseVertexIndices;
                    std::vector<int> closestCoarseVertexIndices;
                    std::vector<int> newTriangulation;
                    std::vector<int> previousTriangulation = triangulations[triangulations.size() - 1];

                    _Mesh->GetVertex().Subset(previousTriangulation, coarseVertexIndices, closestCoarseVertexIndices, newTriangulation);

                    triangulations.push_back(newTriangulation);
                    particleIndicesPerLevel.push_back(coarseVertexIndices);
                    closestCoarseVertexIndicesPerLevel.push_back(closestCoarseVertexIndices);
                }

                for (unsigned int level = 0; level < nbLevels; level++) {
                    if (level == 0) {
                        _DistanceConstraintsPerLevel.emplace_back(_DistanceConstraints);

                        continue;
                    }

                    _DistanceConstraintsPerLevel.emplace_back(_DistanceConstraintsPerLevel[level - 1]);

                    std::vector<std::shared_ptr<DistanceConstraint>> filteredConstraints{};

                    for (auto& constraint : _DistanceConstraintsPerLevel[level]) {
                        bool shouldBeKept = true;

                        auto constraintCopy = std::make_shared<DistanceConstraint>(*constraint);

                        for (auto& particle : constraintCopy->GetParticles()) {
                            unsigned long particleIndex = particle->PositionIndex / 3;

                            if (std::find(triangulations[level].begin(), triangulations[level].end(), particleIndex) != triangulations[level].end())
                                continue;
                            int closestParticleIndex = closestCoarseVertexIndicesPerLevel[level][particleIndex];

                            if (closestParticleIndex == -1) {
                                shouldBeKept = false;

                                continue;
                            }

                            constraintCopy->ReplaceParticle(particle, _Particles[closestParticleIndex]);
                        }

                        if (constraintCopy->GetParticles()[0] == constraintCopy->GetParticles()[1])
                            shouldBeKept = false;
                        if (shouldBeKept)
                            filteredConstraints.push_back(constraintCopy);
                    }

                    _DistanceConstraintsPerLevel[level] = filteredConstraints;
                }

                _TrianglesPerLevel       = triangulations;
                _ParticleIndicesPerLevel = particleIndicesPerLevel;
            }

            void UpdateVertex()
            {
                for (const auto& particle : _Particles) {
                    auto particleLocalPosition = particle->Position - Transform()->Position;

                    _Mesh->GetVertex().Positions[particle->PositionIndex    ] = particleLocalPosition.x;
                    _Mesh->GetVertex().Positions[particle->PositionIndex + 1] = particleLocalPosition.y;
                    _Mesh->GetVertex().Positions[particle->PositionIndex + 2] = particleLocalPosition.z;
                }

                _Mesh->GetVertex().ComputeNormals();

                _Mesh->SendVertexDataToGPU();
            }

            void Reset()
            {
                for (const auto& particle : _Particles)
                    particle->Reset();
            }

        public:

            std::vector<std::shared_ptr<Particle>>& GetParticles()
            {
                return _Particles;
            }

            std::shared_ptr<Mesh> GetMesh()
            {
                return _Mesh;
            }

            Transform *Transform()
            {
                return _Mesh->Transform();
            }

            int NumberOfParticles()
            {
                return (int)_Particles.size();
            }

            void AddFixedConstraint(std::shared_ptr<FixedConstraint> constraint)
            {
                _FixedConstraints.push_back(constraint);
            }

            void AddDistanceConstraint(std::shared_ptr<DistanceConstraint> constraint)
            {
                _DistanceConstraints.push_back(constraint);
            }

            void AddBendConstraint(std::shared_ptr<FastBendConstraint> constraint)
            {
                _FastBendConstraints.push_back(constraint);
            }

            void AddDihedralBendConstraint(std::shared_ptr<DihedralBendConstraint> constraint)
            {
                _DihedralBendConstraints.push_back(constraint);
            }

            void AddVolumeConstraint(std::shared_ptr<VolumeConstraint> constraint)
            {
                _VolumeConstraints.push_back(constraint);
            }

            void AddGlobalVolumeConstraint(std::shared_ptr<GlobalVolumeConstraint> constraint)
            {
                _GlobalVolumeConstraints.push_back(constraint);
            }

            void AddCollisionConstraint(std::shared_ptr<CollisionConstraint> constraint)
            {
                _CollisionConstraints.push_back(constraint);
            }

            std::vector<std::vector<std::shared_ptr<DistanceConstraint>>>& GetDistanceConstraintsPerLevel()
            {
                return _DistanceConstraintsPerLevel;
            }

            std::vector<std::shared_ptr<FixedConstraint>>& GetFixedConstraints()
            {
                return _FixedConstraints;
            }

            std::vector<std::shared_ptr<DistanceConstraint>>& GetDistanceConstraints()
            {
                return _DistanceConstraints;
            }

            std::vector<std::shared_ptr<FastBendConstraint>>& GetFastBendConstraints()
            {
                return _FastBendConstraints;
            }

            std::vector<std::shared_ptr<DihedralBendConstraint>>& GetDihedralBendConstraints()
            {
                return _DihedralBendConstraints;
            }

            std::vector<std::shared_ptr<VolumeConstraint>>& GetVolumeConstraints()
            {
                return _VolumeConstraints;
            }

            std::vector<std::shared_ptr<GlobalVolumeConstraint>>& GetGlobalVolumeConstraints()
            {
                return _GlobalVolumeConstraints;
            }

            std::vector<std::shared_ptr<CollisionConstraint>>& GetCollisionConstraints()
            {
                return _CollisionConstraints;
            }

            float GetMass() const
            {
                return _Mass;
            }

            std::vector<std::vector<GLint>>& GetTrianglesPerLevel()
            {
                return _TrianglesPerLevel;
            }

            std::vector<std::vector<GLint>>& GetParticleIndicesPerLevel()
            {
                return _ParticleIndicesPerLevel;
            }

            void SetCollisionLevel(int level)
            {
                if (level < 0 || level >= _DistanceConstraintsPerLevel.size())
                    throw std::runtime_error("Invalid collision level. Must be between 0 and " + std::to_string(_DistanceConstraintsPerLevel.size() - 1) + ".");
                _CollisionLevel = level;
            }

            int GetCollisionLevel()
            {
                return _CollisionLevel;
            }

        protected:

            float _Mass;

            std::shared_ptr<Mesh> _Mesh;
            std::vector<std::shared_ptr<Particle>> _Particles;

        private:

            std::vector<std::vector<std::shared_ptr<DistanceConstraint>>> _DistanceConstraintsPerLevel;

            std::vector<std::vector<int>> _TrianglesPerLevel;
            std::vector<std::vector<int>> _ParticleIndicesPerLevel;

            int _CollisionLevel = 0;

            std::vector<std::shared_ptr<FixedConstraint>>        _FixedConstraints;
            std::vector<std::shared_ptr<DistanceConstraint>>     _DistanceConstraints;
            std::vector<std::shared_ptr<FastBendConstraint>>     _FastBendConstraints;
            std::vector<std::shared_ptr<DihedralBendConstraint>> _DihedralBendConstraints;
            std::vector<std::shared_ptr<VolumeConstraint>>       _VolumeConstraints;
            std::vector<std::shared_ptr<GlobalVolumeConstraint>> _GlobalVolumeConstraints;
            std::vector<std::shared_ptr<CollisionConstraint>>    _CollisionConstraints;
    };
};
