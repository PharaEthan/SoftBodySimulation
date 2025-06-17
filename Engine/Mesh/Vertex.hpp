#pragma once

#include "Utils/Utils.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

namespace Exodia {

    struct Vertex {

        std::vector<float> Positions {};
        std::vector<float> Normals   {};
        std::vector<float> UVs       {};
        std::vector<float> Colors    {};
        std::vector<int>   Indices   {};

        void ComputeNormals()
        {
            if (Normals.empty())
                Normals.resize(Positions.size());
            for (int i = 0; i < Indices.size(); i += 3) {
                int index1 = Indices[i    ];
                int index2 = Indices[i + 1];
                int index3 = Indices[i + 2];

                glm::vec3 v1 = { Positions[index1 * 3], Positions[index1 * 3 + 1], Positions[index1 * 3 + 2] };
                glm::vec3 v2 = { Positions[index2 * 3], Positions[index2 * 3 + 1], Positions[index2 * 3 + 2] };
                glm::vec3 v3 = { Positions[index3 * 3], Positions[index3 * 3 + 1], Positions[index3 * 3 + 2] };

                glm::vec3 normal = glm::cross(v2 - v1, v3 - v1);

                Normals[index1 * 3    ] += normal.x;
                Normals[index1 * 3 + 1] += normal.y;
                Normals[index1 * 3 + 2] += normal.z;

                Normals[index2 * 3    ] += normal.x;
                Normals[index2 * 3 + 1] += normal.y;
                Normals[index2 * 3 + 2] += normal.z;

                Normals[index3 * 3    ] += normal.x;
                Normals[index3 * 3 + 1] += normal.y;
                Normals[index3 * 3 + 2] += normal.z;
            }

            for (int i = 0; i < Normals.size(); i += 3) {
                glm::vec3 normal = { Normals[i], Normals[i + 1], Normals[i + 2] };

                normal = glm::normalize(normal);

                Normals[i    ] = normal.x;
                Normals[i + 1] = normal.y;
                Normals[i + 2] = normal.z;
            }
        }

        void Subset(std::vector<int> &originalTriangleIndices, std::vector<int> &coarseVertexIndices, std::vector<int> &closestCoarseVertexIndices, std::vector<int> &prunedTriangleIndicesSubset)
        {
            unsigned long nbTotalVertices = Positions.size() / 3;
            int k = 2;

            std::vector<bool> markedAsCoarse(nbTotalVertices, false);

            for (int index: originalTriangleIndices)
                markedAsCoarse[index] = true;
            std::vector<std::vector<int>> neighbors(nbTotalVertices, std::vector<int>());

            for (unsigned int i = 0; i < originalTriangleIndices.size(); i += 3) {
                int index0 = originalTriangleIndices[i    ];
                int index1 = originalTriangleIndices[i + 1];
                int index2 = originalTriangleIndices[i + 2];

                neighbors[index0].push_back(index1);
                neighbors[index0].push_back(index2);

                neighbors[index1].push_back(index0);
                neighbors[index1].push_back(index2);

                neighbors[index2].push_back(index0);
                neighbors[index2].push_back(index1);
            }
            std::vector<unsigned int> nbCoarseNeighbors(nbTotalVertices, 0);

            for (unsigned int i = 0; i < nbTotalVertices; i++) {
                for (auto neighbor: neighbors[i]) {
                    if (markedAsCoarse[neighbor])
                        nbCoarseNeighbors[i] += 1;
                }
            }

            for (unsigned int i = 0; i < nbTotalVertices; i++) {
                if (nbCoarseNeighbors[i] < k)
                    continue;
                bool fineNeighborsCondition = true;

                for (auto neighbor: neighbors[i]) {
                    if (markedAsCoarse[neighbor])
                        continue;
                    if (nbCoarseNeighbors[neighbor] <= k) {
                        fineNeighborsCondition = false;

                        break;
                    }
                }

                if (!fineNeighborsCondition)
                    continue;
                markedAsCoarse[i] = false;

                for (auto neighbor: neighbors[i])
                    nbCoarseNeighbors[neighbor] -= 1;
            }

            for (int i = 0; i < nbTotalVertices; i++) {
                if (markedAsCoarse[i])
                    coarseVertexIndices.push_back(i);
            }
            closestCoarseVertexIndices.resize(nbTotalVertices, -1);

            for (int i = 0; i < nbTotalVertices; i++) {
                if (markedAsCoarse[i]) {
                    closestCoarseVertexIndices[i] = i;

                    continue;
                }
                float minDistance = 1e3;
                int   closest     = -1;

                glm::vec3 position = { Positions[i], Positions[i + 1], Positions[i + 2] };

                for (auto neighbor: neighbors[i]) {
                    if (markedAsCoarse[neighbor]) {
                        glm::vec3 neighborPosition = { Positions[neighbor], Positions[neighbor + 1], Positions[neighbor + 2] };

                        float distance = glm::distance(position, neighborPosition);

                        if (distance >= minDistance)
                            continue;
                        minDistance = distance;
                        closest     = neighbor;
                    }
                }

                closestCoarseVertexIndices[i] = closest;
            }
            std::vector<int> rawTriangleIndicesSubset;

            for (int index: Indices)
                rawTriangleIndicesSubset.push_back(closestCoarseVertexIndices[index]);
            for (unsigned int i = 0; i < rawTriangleIndicesSubset.size(); i += 3) {
                int index0 = rawTriangleIndicesSubset[i    ];
                int index1 = rawTriangleIndicesSubset[i + 1];
                int index2 = rawTriangleIndicesSubset[i + 2];

                if (index0 == -1 || index1 == -1 || index2 == -1)
                    continue;
                if (index0 == index1 || index1 == index2 || index0 == index2)
                    continue;
                prunedTriangleIndicesSubset.push_back(index0);
                prunedTriangleIndicesSubset.push_back(index1);
                prunedTriangleIndicesSubset.push_back(index2);
            }
        }
    };
};
