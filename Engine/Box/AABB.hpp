#pragma once

#include "Mesh/Vertex.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace Exodia {

    class AABB {

        public:

            static AABB *Intersection(const AABB *a, const AABB *b);

        public:

            AABB();
            AABB(glm::vec3 min, glm::vec3 max);

        public:

            bool Intersects(const AABB& other) const;

            bool IntersectsTriangle(glm::vec3 t0, glm::vec3 t1, glm::vec3 t2) const;

            bool IntersectsRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) const;

            bool Contains(const glm::vec3 &point) const;

            void UpdateWithVertex(Vertex& vertex, glm::mat4 worldMatrix);

            void Expand(const glm::vec3& point);
            void Expand(const AABB& other);
            void Expand(float amount);

        public:

            glm::vec3 GetCenter() const
            {
                return (_Min + _Max) * 0.5f;
            }

            glm::vec3 GetSize() const
            {
                return _Max - _Min;
            }

            void Set(const glm::vec3 &min, const glm::vec3 &max)
            {
                _Min = min;
                _Max = max;
            }

            float GetVolume() const
            {
                glm::vec3 s = GetSize();

                return s.x * s.y * s.z;
            }

        private:

            glm::vec3 _Min;
            glm::vec3 _Max;
    };
};
