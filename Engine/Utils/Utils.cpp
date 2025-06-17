#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
    #include <stb_image.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/vec4.hpp>

#include "Utils.hpp"

namespace Exodia {

    void LoadFileToBuffer(const char *filename, std::string &stringBuffer)
    {
        stringBuffer = "";

        std::ifstream reader(filename);

        if (reader.is_open()) {
            std::string lineBuffer;

            while (std::getline(reader, lineBuffer))
                stringBuffer += lineBuffer + "\n";
            reader.close();
        } else
            std::cerr << "Cloud not open " << filename << std::endl;
    }

    unsigned int LoadTextureFromFileToGPU(const char* filename)
    {
        int width;
        int height;
        int chanels;

        unsigned char *data = stbi_load(filename, &width, &height, &chanels, 0);

        unsigned int texID;

        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);

        if (chanels == 1)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        else if (chanels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (chanels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else
            std::cerr << "Error: Unknown number of components '" << chanels << "' for texture " << filename << std::endl;

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        return texID;
    }

    void Utils::Subdivide(std::vector<float>& positions, std::vector<int>& indices)
    {
        std::vector<int> newIndices;

        for (int i = 0; i < indices.size(); i += 3) {
            int index1 = indices[i    ];
            int index2 = indices[i + 1];
            int index3 = indices[i + 2];

            glm::vec3 v1 = glm::vec3(positions[index1 * 3], positions[index1 * 3 + 1], positions[index1 * 3 + 2]);
            glm::vec3 v2 = glm::vec3(positions[index2 * 3], positions[index2 * 3 + 1], positions[index2 * 3 + 2]);
            glm::vec3 v3 = glm::vec3(positions[index3 * 3], positions[index3 * 3 + 1], positions[index3 * 3 + 2]);

            glm::vec3 v12 = (v1 + v2) / 2.f;
            glm::vec3 v23 = (v2 + v3) / 2.f;
            glm::vec3 v31 = (v3 + v1) / 2.f;

            int index12 = positions.size() / 3;
            int index23 = positions.size() / 3 + 1;
            int index31 = positions.size() / 3 + 2;

            positions.push_back(v12.x);
            positions.push_back(v12.y);
            positions.push_back(v12.z);

            positions.push_back(v23.x);
            positions.push_back(v23.y);
            positions.push_back(v23.z);

            positions.push_back(v31.x);
            positions.push_back(v31.y);
            positions.push_back(v31.z);

            newIndices.push_back(index1);
            newIndices.push_back(index12);
            newIndices.push_back(index31);

            newIndices.push_back(index2);
            newIndices.push_back(index23);
            newIndices.push_back(index12);

            newIndices.push_back(index3);
            newIndices.push_back(index31);
            newIndices.push_back(index23);

            newIndices.push_back(index12);
            newIndices.push_back(index23);
            newIndices.push_back(index31);
        }

        indices = newIndices;
    }

    void Utils::MergeVertices(std::vector<float>& positions, std::vector<int>& indices)
    {
        std::vector<float> newPositions;
        std::vector<int>   newIndices;

        for (int i = 0; i < positions.size(); i += 3) {
            glm::vec3 vertex = { positions[i], positions[i + 1], positions[i + 2] };

            bool isDuplicated = false;
            int originalIndex = -1;

            for (int j = 0; j < positions.size() - 3; j += 3) {
                glm::vec3 otherVertex = { positions[j], positions[j + 1], positions[j + 2] };

                if (glm::distance(vertex, otherVertex) < 0.001f) {
                    isDuplicated  = true;
                    originalIndex = j;

                    break;
                }
            }

            if (!isDuplicated)
                continue;
            if (i / 3 == originalIndex / 3)
                continue;
            for (int j = 0; j < indices.size(); j++)
                if (indices[j] == i / 3)
                    indices[j] = originalIndex / 3;
            for (int j = 0; j < indices.size(); j++)
                if (indices[j] > i / 3)
                    indices[j]--;
            positions.erase(positions.begin() + i, positions.begin() + i + 3);

            i -= 3;
        }
    }

    bool Utils::RayTriangleIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 t0, glm::vec3 t1, glm::vec3 t2, float& t)
    {
        glm::vec3 edge1 = t1 - t0;
        glm::vec3 edge2 = t2 - t0;
        glm::vec3 h     = glm::cross(rayDirection, edge2);

        float a = glm::dot(edge1, h);

        if (a > -0.00001f && a < 0.00001f)
            return false;
        float f = 1.0f / a;

        glm::vec3 s = rayOrigin - t0;

        float u = f * glm::dot(s, h);

        if (u < 0.0f || u > 1.0f)
            return false;
        glm::vec3 q = glm::cross(s, edge1);

        float v = f * glm::dot(rayDirection, q);

        if (v < 0.0f || u + v > 1.0f)
            return false;
        t = f * glm::dot(edge2, q);

        if (t > 0.00001f)
            return true;
        return false;
    }

    bool Utils::IsTriangulationClosed(std::vector<int>& indices)
    {
        std::map<std::pair<int, int>, int> edgeCount;

        for (int i = 0; i < indices.size(); i += 3) {
            int v1 = indices[i    ];
            int v2 = indices[i + 1];
            int v3 = indices[i + 2];

            edgeCount[std::make_pair(std::min(v1, v2), std::max(v1, v2))]++;
            edgeCount[std::make_pair(std::min(v2, v3), std::max(v2, v3))]++;
            edgeCount[std::make_pair(std::min(v1, v3), std::max(v1, v3))]++;
        }

        return std::all_of(edgeCount.begin(), edgeCount.end(), [](std::pair<std::pair<int, int>, int> edge) {
            return edge.second == 2;
        });
    }

    bool Utils::IsMergedTriangulationClosed(std::vector<int>& indices, std::vector<float>& positions)
    {
        std::vector<int>   indicesCopy   = std::vector<int>(indices);
        std::vector<float> positionsCopy = std::vector<float>(positions);

        MergeVertices(positionsCopy, indicesCopy);

        return IsTriangulationClosed(indicesCopy);
    }

    PickResult Utils::PickWithRay(std::vector<int> &indices, std::vector<float> &positions, glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::mat4 worldMatrix)
    {
        std::vector<PickResult> results {};

        for (int i = 0; i < indices.size(); i += 3) {
            int index1 = indices[i    ];
            int index2 = indices[i + 1];
            int index3 = indices[i + 2];

            glm::vec3 v1 = glm::vec3(positions[index1 * 3], positions[index1 * 3 + 1], positions[index1 * 3 + 2]);
            glm::vec3 v2 = glm::vec3(positions[index2 * 3], positions[index2 * 3 + 1], positions[index2 * 3 + 2]);
            glm::vec3 v3 = glm::vec3(positions[index3 * 3], positions[index3 * 3 + 1], positions[index3 * 3 + 2]);

            glm::vec3 worldV1 = glm::vec3(worldMatrix * glm::vec4(v1, 1.0f));
            glm::vec3 worldV2 = glm::vec3(worldMatrix * glm::vec4(v2, 1.0f));
            glm::vec3 worldV3 = glm::vec3(worldMatrix * glm::vec4(v3, 1.0f));

            float t;

            if (RayTriangleIntersection(rayOrigin, rayDirection, worldV1, worldV2, worldV3, t)) {
                glm::vec3 normal = glm::cross(worldV2 - worldV1, worldV3 - worldV1);

                if (glm::dot(normal, rayDirection) >= 0)
                    continue;
                PickResult result;
 
                result.HasHit    = true;
                result.HitPoint  = rayOrigin + t * rayDirection;
                result.FaceIndex = i / 3;

                results.push_back(result);
            }
        }

        if (results.empty())
            return {};
        PickResult closestResult = results[0];

        for (int i = 1; i < results.size(); i++)
            if (glm::distance(rayOrigin, results[i].HitPoint) < glm::distance(rayOrigin, closestResult.HitPoint))
                closestResult = results[i];
        return closestResult;
    }
};
