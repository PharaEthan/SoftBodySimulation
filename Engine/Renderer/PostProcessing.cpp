#include <glad/glad.h>

#include "PostProcessing.hpp"
#include "Mesh/Primitives/MeshPrimitives.hpp"

namespace Exodia {

    PostProcessing::PostProcessing(const char *shaderFolder, std::shared_ptr<Engine> engine)
    {
        engine->GetWindowSize(&_Width, &_Height);

        _ScreenQuad = std::make_shared<ScreenQuad>();

        glGenFramebuffers(1, &_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

        _InputTexture = std::make_shared<Texture>(_Width, _Height, RGBA, false);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _InputTexture->Handle(), 0);

        glGenRenderbuffers(1, &_RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, _RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _Width, _Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _RBO);

        _Shader = std::make_shared<Shader>(shaderFolder);

        _UniformScreenResolution = glGetUniformLocation(_Shader->GetProgram(), "screenResolution");
    }

    void PostProcessing::RenderTo(unsigned int targetFramebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, targetFramebuffer);

        _Shader->Bind();

        OnBeforeRender.NotifyObservers();

        _Shader->SetVec2("screenResolution", (float)_Width, (float)_Height);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_DEPTH_TEST);

        _Shader->BindTexture("screenTexture", _InputTexture.get(), 0);

        _ScreenQuad->Render();

        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }

    void PostProcessing::RenderToScreen()
    {
        RenderTo(0);
    }

    void PostProcessing::Resize(int width, int height)
    {
        _Width  = width;
        _Height = height;

        _InputTexture->Resize(_Width, _Height);

        glBindRenderbuffer(GL_RENDERBUFFER, _RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _Width, _Height);
    }
};
