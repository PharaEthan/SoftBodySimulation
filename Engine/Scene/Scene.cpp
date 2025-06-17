#include <glad/glad.h>

#include "Scene.hpp"
#include "Renderer/Materials/DepthMaterial.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Exodia {

    Scene::Scene(std::shared_ptr<Engine> engine) : _Engine(engine)
    {
        _Engine->OnWindowResize.Add([this](int width, int height) {
            _ActiveCamera->SetAspectRatio((float)width / (float)height);
        });
    }

    void Scene::AddMesh(std::shared_ptr<Mesh> mesh)
    {
        _Meshes.push_back(mesh);
    }

    void Scene::RemoveMesh(std::shared_ptr<Mesh> mesh)
    {
        _Meshes.erase(std::remove(_Meshes.begin(), _Meshes.end(), mesh), _Meshes.end());
    }

    void Scene::AddPointLight(std::shared_ptr<PointLight> light)
    {
        _PointLights.push_back(light);
    }

    void Scene::AddDirectionalLight(std::shared_ptr<DirectionalLight> light)
    {
        _DirectionalLights.push_back(light);
    }

    void Scene::AddPostProcess(std::shared_ptr<PostProcessing> postProcess)
    {
        _PostProcesses.push_back(postProcess);
        
        _Engine->OnWindowResize.Add([postProcess](int width, int height) {
            postProcess->Resize(width, height);
        });
    }

    void Scene::AddShadowRenderer(std::shared_ptr<ShadowRenderer> shadowRenderer)
    {
        _ShadowRenderers.push_back(shadowRenderer);
    }

    void Scene::Render()
    {
        OnBeforeRender.NotifyObservers();

        for (const auto& shadowRenderer : _ShadowRenderers)
            shadowRenderer->Render();
        if (!_PostProcesses.empty())
            glBindFramebuffer(GL_FRAMEBUFFER, _PostProcesses[0]->GetFBO());
        else
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto& drawable : _Meshes)
            drawable->Render(_ActiveCamera->GetProjectionViewMatrix());
        if (!_PostProcesses.empty()) {
            for (int i = 0; i < _PostProcesses.size() - 1; i++)
                _PostProcesses[i]->RenderTo(_PostProcesses[i + 1]->GetFBO());
            _PostProcesses[_PostProcesses.size() - 1]->RenderToScreen();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        OnRenderGui.NotifyObservers();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        OnAfterRender.NotifyObservers();
    }

    void Scene::CompileShaders()
    {
        for (auto mesh : _Meshes)
            mesh->GetMaterial()->GetShader()->Compile();
    }

    std::pair<std::shared_ptr<Mesh>, PickResult> Scene::PickWithRay(glm::vec3 rayOrigin, glm::vec3 rayDirection)
    {
        std::vector<std::shared_ptr<Mesh>> meshes;

        for (const auto& mesh : _Meshes) {
            if (!mesh->Enabled)
                continue;
            if (!mesh->IsPickingEnabled())
                continue;
            if (!mesh->GetAABB()->IntersectsRay(rayOrigin, rayDirection))
                continue;
			meshes.push_back(mesh);
        }

		std::sort(meshes.begin(), meshes.end(), [&](const std::shared_ptr<Mesh>& a, const std::shared_ptr<Mesh>& b) {
			return glm::distance(a->GetAABB()->GetCenter(), rayOrigin) < glm::distance(b->GetAABB()->GetCenter(), rayOrigin);
		});

		for (const auto& mesh : meshes) {
			PickResult pickResult = mesh->Pick(rayOrigin, rayDirection);

            if (pickResult.HasHit)
				return { mesh, pickResult };
		}

		return { nullptr, {} };
    }
};
