#pragma once

#include "Exodia.hpp"

using namespace Exodia;

class SceneFactory {

    public:

        static std::shared_ptr<Body> CreateCarpet(std::shared_ptr<Scene> scene, std::shared_ptr<ShadowRenderer> renderer, Solver &solver, int resolution, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
        {
            auto material = std::make_shared<PBRMaterial>(scene);

            material->AlbedoColor = { 1.0f, 1.0f, 1.0f };
            material->Metallic    = 0.0f;
            material->SetAlbedoTexture(new Texture("../../Assets/Textures/carpet.jpg"));
            material->SetNormalTexture(new Texture("../../Assets/Textures/carpet_normal.png"));
            material->SetBackFaceCullingEnabled(false);
            material->ReceiveShadows(renderer);

            auto mesh = MeshPrimitives::Plane("Carpet", *scene, resolution);

            mesh->Transform()->Position = position;
            mesh->Transform()->Rotation = rotation;
            mesh->Transform()->Scale = scale;
            mesh->SetMaterial(material);

            renderer->AddShadowCaster(mesh);

            auto body = std::make_shared<SoftBody>(mesh, 1.0f, 0.1f, 0.01f);

			solver.AddBody(body);

            return body;
        }

        static std::shared_ptr<Body> CreateSphere(std::shared_ptr<Scene> scene, std::shared_ptr<ShadowRenderer> renderer, Solver& solver, glm::vec3 position, glm::vec3 scale, glm::vec3 color, float mass)
        {
            auto material = std::make_shared<PBRMaterial>(scene);

            material->AlbedoColor = color;
            material->Metallic    = 0.2f;
            material->Roughness   = 0.6f;
            material->SetAlbedoTexture(new Texture("../../Assets/Textures/earth.jpg"));
            material->SetBackFaceCullingEnabled(true);
            material->ReceiveShadows(renderer);

            auto mesh = MeshPrimitives::ICOSphere("Sphere", *scene, 4);

            mesh->Transform()->Position = position;
            mesh->Transform()->Scale    = scale;
            mesh->SetMaterial(material);
            mesh->SetPickingEnabled(true);

            renderer->AddShadowCaster(mesh);

            auto body = std::make_shared<SoftBody>(mesh, mass, 1.0f, 1.0f);

            solver.AddBody(body);

            return body;
        }

        static std::shared_ptr<Mesh> CreateGround(std::shared_ptr<Scene> scene, std::shared_ptr<ShadowRenderer> renderer, Solver &solver)
        {
            auto material = std::make_shared<PBRMaterial>(scene);

            material->AlbedoColor = { 0.5f, 0.5f, 0.5f };
            material->Metallic    = 0.5f;
            material->Roughness   = 0.4f;
            material->SetBackFaceCullingEnabled(false);
            material->ReceiveShadows(renderer);

            auto ground = MeshPrimitives::Plane("Ground", *scene, 2);

            ground->Transform()->Position = { 0.0f, 0.0f, 0.0f };
            ground->Transform()->SetScale(40.0f);
            ground->SetPickingEnabled(false);
            ground->SetMaterial(material);

            auto body = std::make_shared<RigidBody>(ground, 0.0f);

            solver.AddBody(body);

            return ground;
        }
};
