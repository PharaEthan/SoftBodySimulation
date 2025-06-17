#include "Camera.hpp"

namespace Exodia {

    Camera::Camera(std::shared_ptr<Engine> engine) : Transform(), _Engine(engine)
    {
        int width;
        int height;

        glfwGetWindowSize(_Engine->GetWindow(), &width, &height);

        SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));

        _ProjectionMatrix = ComputeProjectionMatrix();
    }

    void Camera::Update()
    {
        _ProjectionMatrix     = ComputeProjectionMatrix();
        _ViewMatrix           = ComputeViewMatrix();
        _ProjectionViewMatrix = _ProjectionMatrix * _ViewMatrix;
    }

    glm::mat4 Camera::ComputeProjectionMatrix() const
    {
        return glm::perspective(glm::radians(_FOV), _AspectRatio, _Near, _Far);
    }
};
