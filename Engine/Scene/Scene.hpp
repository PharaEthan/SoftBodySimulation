#pragma once

#include "Engine.hpp"
#include "Renderer/Camera/Camera.hpp"
#include "Renderer/Light/Light.hpp"
#include "Renderer/PostProcessing.hpp"
#include "Renderer/ShadowRenderer.hpp"
#include "Mesh/Mesh.hpp"

#include <memory>

namespace Exodia {

    class Scene {

        public:
        
            explicit Scene(std::shared_ptr<Engine> engine);

        public:

            void AddMesh(std::shared_ptr<Mesh> mesh);
            void RemoveMesh(std::shared_ptr<Mesh> mesh);

            void AddPointLight(std::shared_ptr<PointLight> light);
            void AddDirectionalLight(std::shared_ptr<DirectionalLight> light);
            void AddPostProcess(std::shared_ptr<PostProcessing> postProcess);
            void AddShadowRenderer(std::shared_ptr<ShadowRenderer> shadowRenderer);

            void Render();

            void CompileShaders();

            std::pair<std::shared_ptr<Mesh>, PickResult> PickWithRay(glm::vec3 rayOrigin, glm::vec3 rayDirection);

        public:

            void SetActiveCamera(std::shared_ptr<Camera> camera)
            {
				_ActiveCamera = camera;
            }

            std::shared_ptr<Camera> GetActiveCamera()
            {
                return _ActiveCamera;
            }

            std::vector<std::shared_ptr<PointLight>>* GetPointLights()
            {
                return &_PointLights;
            }

            std::vector<std::shared_ptr<DirectionalLight>>* GetDirectionalLights()
            {
                return &_DirectionalLights;
            }

            std::vector<std::shared_ptr<Mesh>> GetMeshes()
            {
                return _Meshes;
            }

        public:

            Observable<> OnBeforeRender {};
            Observable<> OnAfterRender  {};

            Observable<> OnRenderGui {};

        private:

            std::shared_ptr<Engine> _Engine;

            std::vector<std::shared_ptr<PointLight>>       _PointLights       {};
            std::vector<std::shared_ptr<DirectionalLight>> _DirectionalLights {};

            std::vector<std::shared_ptr<Mesh>> _Meshes {};

            std::vector<std::shared_ptr<ShadowRenderer>> _ShadowRenderers {};
            std::vector<std::shared_ptr<PostProcessing>> _PostProcesses   {};

            std::shared_ptr<Camera> _ActiveCamera = nullptr;
    };
};
