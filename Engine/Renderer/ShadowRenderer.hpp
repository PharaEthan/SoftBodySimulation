#pragma once

#include "Light/Light.hpp"
#include "Materials/DepthMaterial.hpp"
#include "Texture/Texture.hpp"
#include "Mesh/Mesh.hpp"

#include <memory>

namespace Exodia {

    class ShadowRenderer {

        public:
    
            explicit ShadowRenderer(std::shared_ptr<DirectionalLight> directionalLight, const unsigned int shadowMapWidth = 2048, const unsigned int shadowMapHeight = 2048);

        public:

            void Bind();
            void Unbind();

            void Render();

            void ComputeProjectionViewMatrix();

            void AddShadowCaster(std::shared_ptr<Mesh> mesh);
            void RemoveShadowCaster(std::shared_ptr<Mesh> mesh);

        public:

            glm::mat4 GetProjectionViewMatrix()
            {
                return _ProjectionViewMatrix;
            }

            Texture *GetDepthTexture()
            {
                return &_DepthTexture;
            }

            std::shared_ptr<DirectionalLight> GetDirectionalLight()
            {
                return _DirectionalLight;
            }

        private:

            std::shared_ptr<DirectionalLight> _DirectionalLight;

            glm::mat4 _ProjectionViewMatrix;

            std::vector<std::shared_ptr<Mesh>> _ShadowCasters;

            std::shared_ptr<DepthMaterial> _DepthMaterial;

            unsigned int _DepthMapFBO {};

            Texture _DepthTexture;

            unsigned int _Width  {};
            unsigned int _Height {};

            int _InitialWidth  {};
            int _InitialHeight {};
    };
};
