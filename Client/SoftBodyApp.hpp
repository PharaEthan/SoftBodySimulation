#pragma once

#include "Exodia.hpp"
#include "Client/OrbitCamera.hpp"

#include <memory>

using namespace Exodia;

class SoftBodySimulationApp {

    public:

        SoftBodySimulationApp();

    public:

        void Run();

    private:
    
        void InitEngine();
        void InitScene();
        void InitRenderer();
        void InitUI();
        void InitCallbacks();
        void InitPhysics();
        void UpdatePhysics(float deltaTime);
        void UpdateLighting();
        void HandleDragging();

    public:

		std::shared_ptr<Engine> GetEngine() const
        {
            return _Engine;
        }
		
        std::shared_ptr<Scene>  GetScene() const
        {
            return _Scene;
        }
		
        std::shared_ptr<OrbitCamera> GetCamera() const
        {
            return _Camera;
        }
		
        std::shared_ptr<ShadowRenderer> GetRenderer() const
        {
            return _Renderer;
        }

        std::shared_ptr<PostProcessing> GetColorCorrection() const
        {
            return _ColorCorrection;
        }

		Solver &GetSolver()
        {
            return _Solver;
        }

    private:

        std::shared_ptr<Engine>           _Engine;
        std::shared_ptr<Scene>            _Scene;
        std::shared_ptr<OrbitCamera>      _Camera;
        std::shared_ptr<DirectionalLight> _Sun;
        std::shared_ptr<ShadowRenderer>   _Renderer;
        std::shared_ptr<PostProcessing>   _ColorCorrection;

        Solver _Solver;

        // Interaction
        bool  _Play                = false;
        bool  _FixedTimeStep       = true;
        bool  _IsDragging          = false;
		bool  _HasGravity          = true;
        int   _DraggedFace         = -1;
        float _DragDistance        = 0.0f;
        float _CurrentBodyPressure = 1.0f;

        std::shared_ptr<Body> _SelectedBody = nullptr;

        std::shared_ptr<UniformAccelerationField> _GravityField = nullptr;
};
