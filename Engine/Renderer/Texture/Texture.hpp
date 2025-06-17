#pragma once

#include "Utils/Utils.hpp"

#include <vector>
#include <glm/vec4.hpp>

namespace Exodia {

    enum TextureType {
        DEPTH,
        RGBA
    };

    class Texture {

        public:

            Texture(int id = 0);

            explicit Texture(const char* filepath);

            Texture(unsigned int width, unsigned int height, TextureType type = RGBA, bool generateMipMap = true);

            ~Texture();

        public:

            void Resize(int width, int height);

            void Bind(int shaderID);
            void Unbind();

        public:

            unsigned int Handle() const
            {
                return _Handle;
            }

            unsigned int &Handle()
            {
                return _Handle;
            }

        private:

            unsigned int _Handle {};

            TextureType _Type;
    };
};
