#include "SoftBodyApp.hpp"
#include "SceneFactory.hpp"

#define GLM_ENABLE_EXPERIMENTAL
	#include <glm/glm.hpp>
	#include <glm/gtx/quaternion.hpp>
#include <imgui.h>

using namespace Exodia;

SoftBodySimulationApp::SoftBodySimulationApp()
{
	InitEngine();
	InitScene();
	InitRenderer();
	InitUI();
	InitCallbacks();
	InitPhysics();
}

void SoftBodySimulationApp::Run()
{
	if (!_Engine)
		return;
	_Engine->Run();
}

void SoftBodySimulationApp::InitEngine()
{
	_Engine = std::make_shared<Engine>("Computer Graphics II - Soft Body Simulation");
}

void SoftBodySimulationApp::InitScene()
{
	_Scene = std::make_shared<Scene>(_Engine);
}

void SoftBodySimulationApp::InitRenderer()
{
	_Sun      = std::make_shared<DirectionalLight>("Sun", glm::vec3(-1.0f, 1.0f, 0.0f));
	_Renderer = std::make_shared<ShadowRenderer>(_Sun, 4096, 4096);

	_Scene->AddDirectionalLight(_Sun);
	_Scene->AddShadowRenderer(_Renderer);

	{ // -- Camera Initialization --
		_Camera = std::make_shared<OrbitCamera>(_Engine);

		_Camera->SetRadius(20.0f);
		_Camera->SetTarget({ 0.0f, 5.0f, 0.0f });
		_Camera->RotateTheta(-3.14f / 4.0f);
		_Camera->RotatePhi(3.14f / 3.0f);

		_Scene->SetActiveCamera(_Camera);
	}

	{ // -- Color Correction (Post-processing) Initialization --
		_ColorCorrection = std::make_shared<PostProcessing>("../../Assets/Shaders/ColorCorrection", _Engine);

		_ColorCorrection->OnBeforeRender.Add([&]() {
			_ColorCorrection->GetShader()->SetFloat("gamma", 1.0f);
			_ColorCorrection->GetShader()->SetFloat("exposure", 1.0f);
			_ColorCorrection->GetShader()->SetFloat("contrast", 1.1f);
			_ColorCorrection->GetShader()->SetFloat("saturation", 1.1f);
			_ColorCorrection->GetShader()->SetFloat("brightness", 0.0f);
		});

		_Scene->AddPostProcess(_ColorCorrection);
	}

	SceneFactory::CreateGround(_Scene, _Renderer, _Solver);
}

void SoftBodySimulationApp::InitUI()
{
	_Scene->OnRenderGui.Add([&] {
		ImGui::Begin("Editor Panels");

		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("Number of particles: %d", _Solver.GetNumberOfGetParticles());

		int nbCollisionConstraints = 0;

		for (const auto &body : _Solver.GetBodies())
			nbCollisionConstraints += body->GetCollisionConstraints().size();
		ImGui::Text("Number of collision constraints: %d", nbCollisionConstraints);

		ImGui::Checkbox("Fixed time step", &_FixedTimeStep);

		if (_Play && ImGui::Button("Stop"))
			_Play = false;
		if (!_Play && ImGui::Button("Play"))
			_Play = true;
		ImGui::SameLine();

		if (ImGui::Button("Step"))
			_Solver.Solve(1.0f / 60.0f);
		ImGui::SameLine();

		if (ImGui::Button("Reset"))
			_Solver.Reset();
		if (_SelectedBody != nullptr) {
			bool wireframe = _SelectedBody->GetMesh()->GetMaterial()->IsWireframe();

			if (ImGui::Checkbox("Wireframe", &wireframe))
				_SelectedBody->GetMesh()->GetMaterial()->SetWireframe(wireframe);
		}

		if (_SelectedBody != nullptr && _SelectedBody->GetGlobalVolumeConstraints().size() > 0) {
			if (ImGui::SliderFloat("Current Pressure", &_CurrentBodyPressure, 0.0f, 10.0f))
				_SelectedBody->GetGlobalVolumeConstraints()[0]->SetPressure(_CurrentBodyPressure);
		}

		if (ImGui::Checkbox("Gravity", &_HasGravity)) {
			if (_HasGravity) {
				_GravityField = std::make_shared<UniformAccelerationField>(glm::vec3(0.0f, -9.81f, 0.0f));

				_Solver.AddField(_GravityField);
			} else {
				_Solver.RemoveField(_GravityField);
			}
		}

		ImGui::End();
	});
}

void SoftBodySimulationApp::InitCallbacks()
{
	// -- Scene Callbacks --
	_Scene->OnBeforeRender.Add([&]() {
		float deltaTime = _Engine->GetDeltaSeconds();

		HandleDragging();
		UpdateLighting();
		UpdatePhysics(deltaTime);

		_Camera->Update();
	});

	// -- Engine Callbacks --
	_Engine->OnExecuteLoop.Add([&]() {
		_Scene->Render();
	});
}

void SoftBodySimulationApp::InitPhysics()
{
	_GravityField = std::make_shared<UniformAccelerationField>(glm::vec3(0.0f, -9.81f, 0.0f));

	_Solver.AddField(_GravityField);

	_HasGravity = true;
}

void SoftBodySimulationApp::UpdatePhysics(float deltaTime)
{
	if (!_Play)
		return;
	_Solver.Solve(_FixedTimeStep ? 1.0f / 60.0f : deltaTime);
}

void SoftBodySimulationApp::UpdateLighting()
{
	if (!_Sun)
		return;
	float theta = 0.1f * _Engine->GetElapsedSeconds() + glm::pi<float>();

	glm::vec3 newLightDirection = glm::normalize(glm::vec3(cosf(theta), 1.0f, sinf(theta)));

	_Sun->SetDirection(newLightDirection);
}

void SoftBodySimulationApp::HandleDragging()
{
	if (!_Engine->IsMousePressed()) {
		_IsDragging = false;
	} else if (_Engine->IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		glm::vec3 rayOrigin     = _Camera->Position;
		glm::vec2 mousePosition = _Engine->GetMousePosition();
		glm::vec2 windowSize    = _Engine->GetWindowSize();
		glm::vec3 rayDirection  = _Camera->GetRayFromMouse(mousePosition.x, mousePosition.y, windowSize.x, windowSize.y);

		if (!_IsDragging) {
			auto pickResult = _Scene->PickWithRay(rayOrigin, rayDirection);

			if (pickResult.second.HasHit) {
				for (const auto& body : _Solver.GetBodies()) {
					if (body->GetMesh() == pickResult.first) {
						_SelectedBody = body;

						if (_SelectedBody->GetGlobalVolumeConstraints().size() > 0)
							_CurrentBodyPressure = _SelectedBody->GetGlobalVolumeConstraints()[0]->GetPressure();
					}
				}

				glm::vec3 hitPoint = pickResult.second.HitPoint;

				_IsDragging   = true;
				_DragDistance = glm::length(hitPoint - rayOrigin);
				_DraggedFace  = pickResult.second.FaceIndex;
			}
		} else if (_SelectedBody != nullptr) {
			int index0 = _SelectedBody->GetMesh()->GetVertex().Indices[_DraggedFace * 3];
			int index1 = _SelectedBody->GetMesh()->GetVertex().Indices[_DraggedFace * 3 + 1];
			int index2 = _SelectedBody->GetMesh()->GetVertex().Indices[_DraggedFace * 3 + 2];

			std::shared_ptr<Particle> particle0 = _SelectedBody->GetParticles()[index0];
			std::shared_ptr<Particle> particle1 = _SelectedBody->GetParticles()[index1];
			std::shared_ptr<Particle> particle2 = _SelectedBody->GetParticles()[index2];

			glm::vec3 t0 = particle0->Position;
			glm::vec3 t1 = particle1->Position;
			glm::vec3 t2 = particle2->Position;

			glm::vec3 baryCenter  = (t0 + t1 + t2) / 3.0f;
			glm::vec3 hitPoint    = rayOrigin + rayDirection * _DragDistance;
			glm::vec3 translation = hitPoint - baryCenter;

			for (const auto& particle : _SelectedBody->GetParticles()) {
				float weight = 1.0f / (1.0f + glm::length(particle->Position - baryCenter));

				particle->Position += translation * weight;
			}

			_SelectedBody->GetMesh()->GetVertex().ComputeNormals();
			_SelectedBody->GetMesh()->SendVertexDataToGPU();
		}
	}
}
