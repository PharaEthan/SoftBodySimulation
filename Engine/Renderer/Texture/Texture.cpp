#include <glad/glad.h>

#include "Texture.hpp"

namespace Exodia {

    Texture::Texture(int id) : _Handle(id) {};

    Texture::Texture(const char *filepath) : _Handle(LoadTextureFromFileToGPU(filepath)) {};

    Texture::Texture(unsigned int width, unsigned int height, TextureType type, bool generateMipMap) : _Type(type)
    {
        glGenTextures(1, &_Handle);
        glBindTexture(GL_TEXTURE_2D, _Handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        switch (_Type) {
            case RGBA:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (int)width, (int)height, 0, GL_RGBA, GL_FLOAT, nullptr);
                glBindImageTexture(0, _Handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

                break;
            case DEPTH: {
                float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)width, (int)height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

                break;
            }

            default:
                break;
        }

        if (generateMipMap) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &_Handle);
    }

    void Texture::Resize(int width, int height)
    {
        glBindTexture(GL_TEXTURE_2D, _Handle);

        switch (_Type) {
            case RGBA:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (int)width, (int)height, 0, GL_RGBA, GL_FLOAT, nullptr);

                break;
            case DEPTH:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)width, (int)height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

                break;

            default:
				break;
        }
    }

    void Texture::Bind(int shaderID)
    {
        glActiveTexture(GL_TEXTURE0 + shaderID);
        glBindTexture(GL_TEXTURE_2D, _Handle);
    }

    void Texture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
