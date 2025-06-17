#include <glad/glad.h>

#include "ShadowRenderer.hpp"

namespace Exodia {

    ShadowRenderer::ShadowRenderer(std::shared_ptr<DirectionalLight> directionalLight, const unsigned int shadowMapWidth, const unsigned int shadowMapHeight) : _Width(shadowMapWidth), _Height(shadowMapHeight), _DirectionalLight(directionalLight), _DepthTexture(shadowMapWidth, shadowMapHeight, DEPTH, false), _DepthMaterial(std::make_shared<DepthMaterial>())
    {
        glGenFramebuffers(1, &_DepthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, _DepthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _DepthTexture.Handle(), 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowRenderer::Bind()
    {
        GLint dims[4] = { 0 };

        glGetIntegerv(GL_VIEWPORT, dims);

        _InitialWidth = dims[2];
        _InitialHeight = dims[3];

        glViewport(0, 0, (int)_Width, (int)_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, _DepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        ComputeProjectionViewMatrix();
    }

    void ShadowRenderer::Unbind()
    {
        glCullFace(GL_BACK);
        glViewport(0, 0, _InitialWidth, _InitialHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowRenderer::Render()
    {
        Bind();

        for (const std::shared_ptr<Mesh> &shadowCaster : _ShadowCasters)
            shadowCaster->Render(_ProjectionViewMatrix, _DepthMaterial->GetShader());
        Unbind();
    }

    void ShadowRenderer::ComputeProjectionViewMatrix()
    {
        float nearPlane = 1.0f;
        float farPlane  = 75.0f;

        glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, nearPlane, farPlane);
        glm::mat4 lightView       = glm::lookAt(_DirectionalLight->GetDirection() * 20.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        _ProjectionViewMatrix = lightProjection * lightView;
    }

    void ShadowRenderer::AddShadowCaster(std::shared_ptr<Mesh> mesh)
    {
        _ShadowCasters.push_back(mesh);
    }

    void ShadowRenderer::RemoveShadowCaster(std::shared_ptr<Mesh> mesh)
    {
        _ShadowCasters.erase(std::remove(_ShadowCasters.begin(), _ShadowCasters.end(), mesh), _ShadowCasters.end());
    }
};
