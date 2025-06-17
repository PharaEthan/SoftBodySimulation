#pragma once

#include "Utils/Observable.hpp"
#include "Texture/Texture.hpp"
#include "Shader/Shader.hpp"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Exodia {

    class Material {

        public:

            explicit Material(const char *shaderFolder);

        public:

            virtual void Bind();
            virtual void Unbind();

        public:

            void SetWireframe(bool enabled)
            {
                _IsWireframe = enabled;
            }

            bool IsWireframe() const
            {
                return _IsWireframe;
            }

            bool IsBackFaceCullingEnabled() const
            {
                return _IsBackFaceCullingEnabled;
            }

            void SetBackFaceCullingEnabled(bool enabled)
            {
                _IsBackFaceCullingEnabled = enabled;
            }

            Shader *GetShader()
            {
                return &_Shader;
            }

        private:

            Shader _Shader;

            bool _IsWireframe              = false;
            bool _IsBackFaceCullingEnabled = true;
    };

    class DefaultMaterial : public Material {

        public:

            DefaultMaterial() : Material("../../Assets/Shaders/Default")
            {
				SetBackFaceCullingEnabled(false);
            }
    };
};
