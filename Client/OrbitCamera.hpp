#pragma once

#include "Renderer/Camera/Camera.hpp"
#include "Engine.hpp"

#include <cmath>

using namespace Exodia;

class OrbitCamera : public Camera {

    public:

        explicit OrbitCamera(std::shared_ptr<Engine> engine);

    public:

        glm::mat4 ComputeViewMatrix() override;

        void Update() override;

        void Zoom(float amount);

    public:

        void SetTarget(glm::vec3 target)
        {
            _Target = target;
        }

        void SetRadius(float radius)
        {
			_Radius = radius;
        }

        void SetMinRadius(float minRadius)
        {
			_MinRadius = minRadius;
        }

        void RotateTheta(float angle)
        {
            _Theta = std::max(std::min(_Theta + angle, 3.0f), 0.14f);
        }

        void RotatePhi(float angle)
        {
			_Phi += angle;
        }

    private:

        glm::vec3 _Target;

        float _Radius;
        float _MinRadius;
        float _Theta;
        float _Phi;
};
