#include "OrbitCamera.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>

using namespace Exodia;

OrbitCamera::OrbitCamera(std::shared_ptr<Engine> engine) : Exodia::Camera(engine), _Target(0.0f), _Radius(20.0f), _Theta(3.14f / 2.0f), _Phi(0.0f)
{
    engine->OnMouseScroll.Add([this](double xOffset, double yOffset) {
        auto &io = ImGui::GetIO();

        if(io.WantCaptureMouse || io.WantCaptureKeyboard)
            return;
        if(_Engine->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            return;
        float scrollOffset = (float)yOffset / 5.0f;

        Zoom(scrollOffset);
    });

    engine->OnMouseMove.Add([this](double mouseDX, double mouseDY) {
        auto &io = ImGui::GetIO();

        if(io.WantCaptureMouse || io.WantCaptureKeyboard)
            return;
        if (!_Engine->IsMousePressed())
            return;
        if (_Engine->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            return;
        if(_Engine->IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
            RotatePhi(  (float)(-mouseDX) / 200.0f);
            RotateTheta((float)(-mouseDY) / 200.0f);
        } else {
            glm::vec3 forward = glm::normalize(_Target - Position);
            glm::vec3 right   = glm::normalize(glm::cross(forward, GetUpwardDirection()));

            _Target -= right                * (float)mouseDX / 100.0f;
            _Target += GetUpwardDirection() * (float)mouseDY / 100.0f;
        }
    });
}

glm::mat4 OrbitCamera::ComputeViewMatrix()
{
    return glm::lookAt(Position, _Target, GetUpwardDirection());
}

void OrbitCamera::Update()
{
    float speed = 0.01f;

    auto &io = ImGui::GetIO();

    if(!io.WantCaptureMouse && !io.WantCaptureKeyboard) {
        if (_Engine->IsKeyPressed(GLFW_KEY_UP))
            _Theta = std::max(_Theta - speed, 0.14f);
        if (_Engine->IsKeyPressed(GLFW_KEY_DOWN))
            _Theta = std::min(_Theta + speed, 3.00f);
        if (_Engine->IsKeyPressed(GLFW_KEY_LEFT))
            _Phi -= speed;
        if (_Engine->IsKeyPressed(GLFW_KEY_RIGHT))
            _Phi += speed;
    }

    Position = {
        _Target.x + _Radius * std::sin(_Theta) * std::sin(_Phi),
        _Target.y + _Radius * std::cos(_Theta),
        _Target.z + _Radius * std::sin(_Theta) * std::cos(_Phi)
    };

    Camera::Update();
}

void OrbitCamera::Zoom(float amount)
{
    _Radius = std::max(_Radius - amount, _MinRadius);
}
