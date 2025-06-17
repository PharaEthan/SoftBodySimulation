#include "Body.hpp"
#include "Utils/Utils.hpp"

#include <map>
#include <utility>

namespace Exodia {

    class SoftBody : public Body {

        public:

            SoftBody(std::shared_ptr<Mesh> mesh, float mass, float stretchCompliance = 0.0001f, float bendCompliance = 0.2f) : Body(mesh, mass)
            {
                for (unsigned int i = 0; i < mesh->GetVertex().Indices.size(); i += 3) {
                    auto index1 = mesh->GetVertex().Indices[i];
                    auto index2 = mesh->GetVertex().Indices[i + 1];
                    auto index3 = mesh->GetVertex().Indices[i + 2];

                    auto p1 = _Particles[index1];
                    auto p2 = _Particles[index2];
                    auto p3 = _Particles[index3];

                    AddDistanceConstraint(std::make_shared<DistanceConstraint>(p1, p2, stretchCompliance));
                    AddDistanceConstraint(std::make_shared<DistanceConstraint>(p2, p3, stretchCompliance));
                    AddDistanceConstraint(std::make_shared<DistanceConstraint>(p3, p1, stretchCompliance));
                }

                for (unsigned int i = 0; i < _Particles.size(); i++) {
                    for (unsigned int j = i + 1; j < _Particles.size(); j++) {
                        if (glm::distance(_Particles[i]->Position, _Particles[j]->Position) < 0.001f)
                            AddDistanceConstraint(std::make_shared<DistanceConstraint>(_Particles[i], _Particles[j], 0.0f));
                    }
                }

                std::map<std::pair<unsigned int, unsigned int>, std::vector<unsigned int>> edgeToTriangles;

                for (unsigned int i = 0; i < mesh->GetVertex().Indices.size(); i += 3) {
                    auto index1 = mesh->GetVertex().Indices[i    ];
                    auto index2 = mesh->GetVertex().Indices[i + 1];
                    auto index3 = mesh->GetVertex().Indices[i + 2];

                    auto edge1 = std::pair<unsigned int, unsigned int>(std::min(index1, index2), std::max(index1, index2));
                    auto edge2 = std::pair<unsigned int, unsigned int>(std::min(index2, index3), std::max(index2, index3));
                    auto edge3 = std::pair<unsigned int, unsigned int>(std::min(index3, index1), std::max(index3, index1));

                    edgeToTriangles[edge1].push_back(i);
                    edgeToTriangles[edge2].push_back(i);
                    edgeToTriangles[edge3].push_back(i);
                }

                for (const auto& edgeToTrianglesPair : edgeToTriangles) {
                    auto edge      = edgeToTrianglesPair.first;
                    auto triangles = edgeToTrianglesPair.second;

                    if (triangles.size() != 2)
                        continue;
                    auto triangle1 = triangles[0];
                    auto triangle2 = triangles[1];

                    std::vector<unsigned int> notSharedVertices;

                    for (unsigned int i = 0; i < 3; i++) {
                        auto index = mesh->GetVertex().Indices[triangle1 + i];

                        if (index != edge.first && index != edge.second)
                            notSharedVertices.push_back(index);
                        index = mesh->GetVertex().Indices[triangle2 + i];

                        if (index != edge.first && index != edge.second)
                            notSharedVertices.push_back(index);
                    }

                    if (notSharedVertices.size() != 2)
                        continue;
                    AddBendConstraint(std::make_shared<FastBendConstraint>(_Particles[edge.first], _Particles[edge.second], _Particles[notSharedVertices[0]], _Particles[notSharedVertices[1]], bendCompliance));
                }

                if (Utils::IsMergedTriangulationClosed(mesh->GetVertex().Indices, mesh->GetVertex().Positions))
                    AddGlobalVolumeConstraint(std::make_shared<GlobalVolumeConstraint>(_Particles, mesh->GetVertex().Indices, 1.0f, 0.0f));
            }
    };
};
