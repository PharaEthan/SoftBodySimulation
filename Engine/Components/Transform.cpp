#include "Transform.hpp"

#include <glm/ext.hpp>

namespace Exodia {

    void Transform::Reset()
    {
		Position = { 0.0f, 0.0f, 0.0f };
		Rotation = { 0.0f, 0.0f, 0.0f };
		Scale    = { 1.0f, 1.0f, 1.0f };
    }

    void Transform::Translate(glm::vec3 displacement)
    {
		Position += displacement;
    }

    glm::mat4 Transform::ComputeWorldMatrix()
    {
        glm::mat4 world = glm::mat4(1.0f);
        
        if (Parent != nullptr)
            world = world * Parent->ComputeWorldMatrix();
        world = glm::translate(world, Position);
        world = glm::scale(world    , Scale);
        world = glm::rotate(world   , Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        world = glm::rotate(world   , Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        world = glm::rotate(world   , Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        return world;
    }

    glm::mat4 Transform::ComputeNormalMatrix()
    {
        return glm::transpose(glm::inverse(ComputeWorldMatrix()));
    }

    glm::mat4 Transform::ComputeRotationMatrix()
    {
        glm::mat4 rotation = glm::mat4(1.0f);

        rotation = glm::rotate(rotation, Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        rotation = glm::rotate(rotation, Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        rotation = glm::rotate(rotation, Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        return rotation;
    }

};

