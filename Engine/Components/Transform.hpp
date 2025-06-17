#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace Exodia {

    class Transform {

        public:

            Transform() = default;

        public:

            void Reset();

            void Translate(glm::vec3 displacement);

            glm::mat4 ComputeWorldMatrix();
            glm::mat4 ComputeNormalMatrix();
            glm::mat4 ComputeRotationMatrix();

        public:

            void SetScale(float scale)
            {
                Scale = { scale, scale, scale };
            }

            glm::vec3 GetAbsolutePosition()
            {
                if (Parent == nullptr)
                    return Position;
                return Position + Parent->GetAbsolutePosition();
            }

            glm::vec3 GetForwardDirection()
            {
                glm::vec3 localForward = { 0.0f, 0.0f, -1.0f };

                return glm::normalize(glm::vec3(ComputeWorldMatrix() * glm::vec4(localForward, 0.0f)));
            }

            glm::vec3 GetUpwardDirection()
            {
                glm::vec3 localUp = { 0.0f, 1.0f, 0.0f };

				return glm::normalize(glm::vec3(ComputeWorldMatrix() * glm::vec4(localUp, 0.0f)));
            }

            glm::vec3 GetLeftDirection()
            {
				glm::vec3 localLeft = { -1.0f, 0.0f, 0.0f };

				return glm::normalize(glm::vec3(ComputeWorldMatrix() * glm::vec4(localLeft, 0.0f)));
			}

        public:
        
            glm::vec3 Position = glm::vec3(0.0);
            glm::vec3 Scale    = glm::vec3(1.0);
            glm::vec3 Rotation = glm::vec3(0.0);

            Transform *Parent = nullptr;
    };
};
