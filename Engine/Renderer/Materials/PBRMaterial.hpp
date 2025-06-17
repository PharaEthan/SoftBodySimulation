#pragma once

#include "Material.hpp"
#include "Texture/Texture.hpp"
#include "ShadowRenderer.hpp"
#include "Scene/Scene.hpp"

#include <memory>

namespace Exodia {

    class PBRMaterial : public Material {

        public:

            explicit PBRMaterial(std::shared_ptr<Scene> scene);

        public:

            void Bind() override;
            void Unbind() override;

            void ReceiveShadows(std::shared_ptr<ShadowRenderer> shadowRenderer);

        public:

            void SetLightingEnabled(bool enabled)
            {
                _LightingEnabled = enabled;
            }

            void SetRoughnessTexture(Texture *texture)
            {
                if (_RoughnessTexture == nullptr)
                    GetShader()->SetDefine("ROUGHNESS_TEXTURE");
                _RoughnessTexture = texture;
            }

            void SetAlbedoTexture(Texture *texture)
            {
                if (_AlbedoTexture == nullptr)
                    GetShader()->SetDefine("ALBEDO_TEXTURE");
                _AlbedoTexture = texture;
            }

            void SetNormalTexture(Texture *texture)
            {
                if (_NormalTexture == nullptr)
                    GetShader()->SetDefine("NORMAL_TEXTURE");
                _NormalTexture = texture;
            }

        public:

            bool HasAlphaColor = false;

			glm::vec3 AlbedoColor  = glm::vec3(1.0f);
			glm::vec3 AmbientColor = glm::vec3(0.0f);
			glm::vec3 AlphaColor   = glm::vec3(0.0f);

			float Metallic         = 1.0f;
			float Roughness        = 0.5f;
            float AmbientOcclusion = 1.0f;

        private:

            std::shared_ptr<Scene> _Scene;

            Texture* _AlbedoTexture    = nullptr;
            Texture* _NormalTexture    = nullptr;
            Texture* _MetallicTexture  = nullptr;
            Texture* _RoughnessTexture = nullptr;
            Texture* _AOTexture        = nullptr;

            bool _LightingEnabled = true;

            std::shared_ptr<ShadowRenderer> _ShadowRenderer = nullptr;
    };
};
