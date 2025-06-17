#pragma once

#include "Shader/Shader.hpp"
#include "Engine.hpp"
#include "Mesh/Primitives/ScreenQuad.hpp"
#include "Utils/Utils.hpp"

#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>

namespace Exodia {

    class PostProcessing {

        public:

            explicit PostProcessing(const char *shaderFolder, std::shared_ptr<Engine> engine);

            ~PostProcessing() = default;

        public:

            void RenderTo(unsigned int targetFrameBuffer);
            void RenderToScreen();

            void Resize(int width, int height);

        public:

            void SetFBO(unsigned int FBO)
            {
                _FBO = FBO;
            }

            unsigned int GetFBO() const
            {
                return _FBO;
            }

            std::shared_ptr<Shader> GetShader()
            {
                return _Shader;
            }

        public:

            Observable<> OnBeforeRender;

        private:

            int _Width{};
            int _Height{};

            std::shared_ptr<ScreenQuad> _ScreenQuad;

            unsigned int _FBO {};
            unsigned int _RBO {};

            std::shared_ptr<Texture> _InputTexture{};

            std::shared_ptr<Shader> _Shader;

            unsigned int _UniformScreenResolution;
    };
};
