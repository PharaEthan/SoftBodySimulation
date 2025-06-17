#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

#include "Engine.hpp"

namespace Exodia {

    Engine::Engine(const char* name, int windowWidth, int windowHeight)
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _Window = glfwCreateWindow(windowWidth, windowHeight, name, nullptr, nullptr);

        if (_Window == nullptr)
            throw std::runtime_error("Failed to create GLFW window");
        glfwSetErrorCallback([](int error, const char* description) {
            throw std::runtime_error(description);
        });

        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        OnKeyPress.Add([this](int key) {
            if (key == GLFW_KEY_ESCAPE)
				_IsRunning = false;
        });

        glfwMakeContextCurrent(_Window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize GLAD");
        glfwSetWindowUserPointer(_Window, this);
        glfwSetKeyCallback(_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));

            if (action == GLFW_PRESS) {
                engine->_KeyStates[key] = true;

                engine->OnKeyPress.NotifyObservers(key);
            } else if (action == GLFW_RELEASE) {
                engine->_KeyStates[key] = false;

                engine->OnKeyRelease.NotifyObservers(key);
            }
        });

        glfwSetScrollCallback(_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));

            engine->OnMouseScroll.NotifyObservers(xOffset, yOffset);
        });

        glfwSetCursorPosCallback(_Window, [](GLFWwindow* window, double xpos, double ypos) {
            Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));

            double mouseDX = xpos - engine->_MouseX;
            double mouseDY = ypos - engine->_MouseY;

            if (mouseDX != 0 || mouseDY != 0)
                engine->OnMouseMove.NotifyObservers(mouseDX, mouseDY);
            engine->_MouseX = xpos;
            engine->_MouseY = ypos;
        });

        glfwSetFramebufferSizeCallback(_Window, [](GLFWwindow* window, int width, int height) {
            Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
            
            glViewport(0, 0, width, height);

            engine->OnWindowResize.NotifyObservers(width, height);
        });

        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(_Window, true);
        ImGui_ImplOpenGL3_Init("#version 450");

        SetClearColor(0.4f, 0.6f, 0.6f, 1.0f);

        _LastFrameTime = GetElapsedSeconds();

        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glCullFace(GL_BACK);
    }

    Engine::~Engine()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(_Window);
    }

    void Engine::Run()
    {
		_IsRunning = true;

        int width;
        int height;

        glfwGetWindowSize(_Window, &width, &height);

        OnWindowResize.NotifyObservers(width, height);

        while (_IsRunning) {
            float newFrameTime = GetElapsedSeconds();

            _DeltaTime     = newFrameTime - _LastFrameTime;
            _LastFrameTime = newFrameTime;

            OnExecuteLoop.NotifyObservers();

            glfwPollEvents();
            glfwSwapBuffers(_Window);
        }

        glfwTerminate();
    }
};
