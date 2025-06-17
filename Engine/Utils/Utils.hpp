#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include <iostream>
#include <glm/gtc/random.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <algorithm>

#include "Ray/PickResult.hpp"

namespace Exodia {

    void LoadFileToBuffer(const char *filename, std::string &stringBuffer);

    unsigned int LoadTextureFromFileToGPU(const char *filename);

    class Utils {

        public:

        static void Subdivide(std::vector<float>& positions, std::vector<int>& indices);

        static void MergeVertices(std::vector<float>& positions, std::vector<int>& indices);

        static bool RayTriangleIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 t0, glm::vec3 t1, glm::vec3 t2, float& t);

        static bool IsTriangulationClosed(std::vector<int>& indices);

        static bool IsMergedTriangulationClosed(std::vector<int>& indices, std::vector<float>& positions);

        static PickResult PickWithRay(std::vector<int> &indices, std::vector<float> &positions, glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::mat4 worldMatrix);
    };
};
