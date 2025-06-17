#pragma once

#include "Transform.hpp"
#include "Engine.hpp"

#include <glm/ext.hpp>
#include <memory>

namespace Exodia {

    class Camera : public Transform {

        public:

            explicit Camera(std::shared_ptr<Engine> engine);

            virtual void Update();

        public:

            virtual glm::mat4 ComputeViewMatrix() = 0;

        public:

            glm::mat4 ComputeProjectionMatrix() const;

        public:

            float GetFov() const
            {
                return _FOV;
            }

            void SetFoV(const float fov)
            {
                _FOV = fov;
            }

            float GetAspectRatio() const
            {
                return _AspectRatio;
            }

            void SetAspectRatio(const float aspect)
            {
                _AspectRatio = aspect;
            }

            float GetNear() const
            {
                return _Near;
            }

            void SetNear(const float near)
            {
                _Near = near;
            }

            float GetFar() const
            {
                return _Far;
            }

            void SetFar(const float far)
            {
                _Far = far;
            }

            glm::mat4 GetViewMatrix()
            {
                return _ViewMatrix;
            }

            glm::mat4 GetProjectionMatrix()
            {
                return _ProjectionMatrix;
            }

            glm::mat4 GetProjectionViewMatrix()
            {
                return _ProjectionViewMatrix;
            }

            glm::vec3 GetRayFromMouse(float mouseX, float mouseY, float windowWidth, float windowHeight)
            {
                glm::vec3 rayOrigin         = Position;
                glm::vec4 mouseClipPosition = {
                           (mouseX / windowWidth ) * 2.0f - 1.0f,
                    (1.0f - mouseY / windowHeight) * 2.0f - 1.0f,
                     0.0f,
                     1.0f
                };
                glm::vec4 mouseWorldPosition = glm::inverse(_ViewMatrix) * glm::inverse(_ProjectionMatrix) * mouseClipPosition;

                mouseWorldPosition /= mouseWorldPosition.w;

                return glm::normalize(glm::vec3(mouseWorldPosition) - rayOrigin);
            }

        protected:

            std::shared_ptr<Engine> _Engine;

            glm::mat4 _ViewMatrix           {};
            glm::mat4 _ProjectionMatrix     {};
            glm::mat4 _ProjectionViewMatrix {};

            float _FOV         = 45.f;
            float _AspectRatio = 1.0f;
            float _Near        = 0.1f;
            float _Far         = 100.f;
        };
};
