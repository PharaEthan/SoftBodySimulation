#pragma once

#include "Utils/Observable.hpp"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/vec2.hpp>
#include <map>

namespace Exodia {

    class Engine {

        public:

            Engine(const char *name, int windowWidth = 1400, int windowHeight = 720);

            ~Engine();

        public:

            void Run();

        public:

            void SetCursorEnabled(bool enabled)
            {
                glfwSetInputMode(_Window, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            }

            bool IsMousePressed() const
            {
                return glfwGetMouseButton(_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            }

            bool IsKeyPressed(int key) const
            {
                return _KeyStates.find(key) != _KeyStates.end() && _KeyStates.at(key);
            }

            GLFWwindow *GetWindow() const
            {
                return _Window;
            }

            void GetWindowSize(int* width, int* height)
            {
                glfwGetWindowSize(_Window, width, height);
            }

            float GetElapsedSeconds() const
            {
                return (float)glfwGetTime();
            }

            float GetDeltaSeconds() const
            {
                return _DeltaTime;
            }

            void SetClearColor(float r, float g, float b, float a)
            {
                glClearColor(r, g, b, a);
            }

            glm::vec2 GetMousePosition()
            {
                double x;
                double y;

                glfwGetCursorPos(_Window, &x, &y);

                return { x, y };
            }

            glm::vec2 GetWindowSize()
            {
                int width;
                int height;

                glfwGetWindowSize(_Window, &width, &height);

                return { width, height };
            }
    
        public:

            Observable<> OnExecuteLoop {};

            Observable<int, int> OnWindowResize {};

            Observable<int> OnKeyPress   {};
            Observable<int> OnKeyRelease {};

            Observable<double, double> OnMouseScroll {};
            Observable<double, double> OnMouseMove   {};

        private:

            bool _IsRunning = false;

            double _MouseX {};
            double _MouseY {};

            float _LastFrameTime {};
            float _DeltaTime     {};

            std::map<int, bool> _KeyStates {};

            GLFWwindow *_Window;
    };
};
