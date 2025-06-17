#pragma once

#include "Renderer/Shader/Shader.hpp"

#include <glm/glm.hpp>

namespace Exodia {

    class Renderable {

        public:

            virtual ~Renderable() = default;

        public:

            virtual void Render(glm::mat4 projectionViewMatrix, Shader *shaderOverride = nullptr) = 0;
    };
};
