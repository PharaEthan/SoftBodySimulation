#include <glad/glad.h>

#include "Material.hpp"

namespace Exodia {

    Material::Material(const char *shaderFolder) : _Shader(shaderFolder) {};

    void Material::Bind()
    {
        _Shader.Bind();

        if (_IsBackFaceCullingEnabled)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, _IsWireframe ? GL_LINE : GL_FILL);
    }

    void Material::Unbind()
    {
        _Shader.Unbind();
    }
};
