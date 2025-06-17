#include <glad/glad.h>

#include "AABB.hpp"

namespace Exodia {

    AABB *AABB::Intersection(const AABB *a, const AABB *b)
    {
        if (!a->Intersects(*b))
            return nullptr;
        glm::vec3 min = glm::max(a->_Min, b->_Min);
        glm::vec3 max = glm::min(a->_Max, b->_Max);

        return new AABB(min, max);
    }

	AABB::AABB() : _Min(std::numeric_limits<float>::max()), _Max(-_Min) {};

	AABB::AABB(glm::vec3 min, glm::vec3 max) : _Min(min), _Max(max) {};

    bool AABB::Intersects(const AABB &other) const
    {
        return (_Min.x <= other._Max.x && _Max.x >= other._Min.x) && (_Min.y <= other._Max.y && _Max.y >= other._Min.y) && (_Min.z <= other._Max.z && _Max.z >= other._Min.z);
    }

    bool AABB::IntersectsTriangle(glm::vec3 t0, glm::vec3 t1, glm::vec3 t2) const
    {
        AABB triangleAABB {};

        triangleAABB.Expand(t0);
        triangleAABB.Expand(t1);
        triangleAABB.Expand(t2);

        return Intersects(triangleAABB);
    }

    bool AABB::IntersectsRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) const
    {
        float tmin = (this->_Min.x - rayOrigin.x) / rayDirection.x;
        float tmax = (this->_Max.x - rayOrigin.x) / rayDirection.x;

        if (tmin > tmax)
            std::swap(tmin, tmax);
        float tymin = (this->_Min.y - rayOrigin.y) / rayDirection.y;
        float tymax = (this->_Max.y - rayOrigin.y) / rayDirection.y;

        if (tymin > tymax)
            std::swap(tymin, tymax);
        if ((tmin > tymax) || (tymin > tmax))
            return false;
        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;
        float tzmin = (this->_Min.z - rayOrigin.z) / rayDirection.z;
        float tzmax = (this->_Max.z - rayOrigin.z) / rayDirection.z;

        if (tzmin > tzmax)
            std::swap(tzmin, tzmax);
        return (tmin <= tzmax) && (tzmin <= tmax);
    }

    bool AABB::Contains(const glm::vec3& point) const
    {
        return (point.x >= _Min.x && point.x <= _Max.x) && (point.y >= _Min.y && point.y <= _Max.y) && (point.z >= _Min.z && point.z <= _Max.z);
    }

    void AABB::UpdateWithVertex(Vertex& vertex, glm::mat4 worldMatrix)
    {
        std::vector<GLfloat> positions = vertex.Positions;

        _Min = glm::vec3(std::numeric_limits<float>::max());
        _Max = -_Min;

        for (int i = 0; i < positions.size(); i += 3) {
            glm::vec4 position            = { positions[i], positions[i + 1], positions[i + 2], 1.0f };
            glm::vec4 transformedPosition = worldMatrix * position;

            Expand(glm::vec3(transformedPosition));
        }

        Expand(0.2f);
    }

    void AABB::Expand(const glm::vec3& point)
    {
        _Min = glm::min(_Min, point);
        _Max = glm::max(_Max, point);
    }

    void AABB::Expand(const AABB& other)
    {
        _Min = glm::min(_Min, other._Min);
        _Max = glm::max(_Max, other._Max);
    }

    void AABB::Expand(float amount)
    {
        _Min -= glm::vec3(amount);
        _Max += glm::vec3(amount);
    }

};
