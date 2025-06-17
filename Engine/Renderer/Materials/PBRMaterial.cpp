#include <glad/glad.h>
#include <string>
#include <vector>

#include "PBRMaterial.hpp"
#include "Settings.hpp"
#include "Light/Light.hpp"
#include "Scene/Scene.hpp"

namespace Exodia {

    PBRMaterial::PBRMaterial(std::shared_ptr<Scene> scene) : Material("../../Assets/Shaders/PBR"), _Scene(scene)
    {
        std::string maxPointLights       = std::to_string(Settings::MAX_POINT_LIGHTS);
        std::string maxDirectionalLights = std::to_string(Settings::MAX_DIRECTIONAL_LIGHTS);

        GetShader()->SetDefine((std::string("MAX_POINT_LIGHTS ")       + maxPointLights).c_str());
        GetShader()->SetDefine((std::string("MAX_DIRECTIONAL_LIGHTS ") + maxDirectionalLights).c_str());
    }

    void PBRMaterial::Bind()
    {
        Material::Bind();

        if (_AlbedoTexture != nullptr)
            GetShader()->BindTexture("albedoTexture", _AlbedoTexture, 0);
        if (_RoughnessTexture != nullptr)
            GetShader()->BindTexture("roughnessTexture", _RoughnessTexture, 1);
        if (_NormalTexture != nullptr)
            GetShader()->BindTexture("normalTexture", _NormalTexture, 2);
        if (_ShadowRenderer != nullptr) {
            glm::mat4 lightSpaceMatrix = _ShadowRenderer->GetProjectionViewMatrix();

            GetShader()->BindTexture("shadowMap", _ShadowRenderer->GetDepthTexture(), 3);
            GetShader()->SetMat4("lightSpaceMatrix", &lightSpaceMatrix);
            GetShader()->SetVec3("lightDirection", _ShadowRenderer->GetDirectionalLight()->GetDirection());
        }

        GetShader()->SetVec3("cameraPosition" , _Scene->GetActiveCamera()->Position);
        GetShader()->SetBool("lightingEnabled", _LightingEnabled);

        GetShader()->SetVec3("albedoColor" , AlbedoColor);
        GetShader()->SetVec3("ambientColor", AmbientColor);

        if (HasAlphaColor)
            GetShader()->SetVec3("alphaColor", AlphaColor);
        GetShader()->SetFloat("metallic" , Metallic);
        GetShader()->SetFloat("roughness", Roughness);
        GetShader()->SetFloat("ao"       , AmbientOcclusion);

        std::vector<std::shared_ptr<PointLight>> *pointLights = _Scene->GetPointLights();

        GetShader()->SetInt("pointLightCount", (int)pointLights->size());

        for (int i = 0; i < pointLights->size(); i++) {
            std::shared_ptr<PointLight> light = pointLights->at(i);

            GetShader()->SetVec3(("pointLights["  + std::to_string(i) + "].position").c_str() , light->Transform()->Position);
            GetShader()->SetVec3(("pointLights["  + std::to_string(i) + "].color").c_str()    , light->GetColor());
            GetShader()->SetFloat(("pointLights[" + std::to_string(i) + "].intensity").c_str(), light->GetIntensity());
        }

        std::vector<std::shared_ptr<DirectionalLight>> *directionalLights = _Scene->GetDirectionalLights();

        GetShader()->SetInt("directionalLightCount", (int)directionalLights->size());

        for (int i = 0; i < directionalLights->size(); i++) {
            std::shared_ptr<DirectionalLight> light = directionalLights->at(i);

            GetShader()->SetVec3(("directionalLights["  + std::to_string(i) + "].direction").c_str(), light->GetDirection());
            GetShader()->SetVec3(("directionalLights["  + std::to_string(i) + "].color").c_str()    , light->GetColor());
            GetShader()->SetFloat(("directionalLights[" + std::to_string(i) + "].intensity").c_str(), light->GetIntensity());
        }
    }

    void PBRMaterial::Unbind()
    {
        Material::Unbind();

        if (_ShadowRenderer != nullptr)
            _ShadowRenderer->GetDepthTexture()->Unbind();
    }

    void PBRMaterial::ReceiveShadows(std::shared_ptr<ShadowRenderer> shadowRenderer)
    {
        if (_ShadowRenderer == nullptr)
            GetShader()->SetDefine("SHADOW_MAP");
        _ShadowRenderer = shadowRenderer;
    }
};
