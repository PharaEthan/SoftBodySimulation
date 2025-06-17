#include <glad/glad.h>

#include "Mesh.hpp"

#include <iostream>

namespace Exodia {

    std::shared_ptr<Material> Mesh::BaseMaterial = std::make_shared<DefaultMaterial>();

    std::shared_ptr<Mesh> Mesh::FromVertex(const char *name, Vertex &vertex)
    {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name);

        mesh->SetVertex(vertex);

        return mesh;
    }

    Mesh::Mesh(const char* name) : Transformable(), Renderable(), _Name(name), _ID(UUID::GenerateUUIDV4()), _Material(Mesh::BaseMaterial) {};

    void Mesh::Render(glm::mat4 projectionViewMatrix, Shader *shaderOverride)
    {
        if (!Enabled)
            return;
        const glm::mat4 world        = _Transform.ComputeWorldMatrix();
        const glm::mat4 normalMatrix = _Transform.ComputeNormalMatrix();

        _AABB.UpdateWithVertex(_Vertex, world);

        Shader *shader = shaderOverride == nullptr ? _Material->GetShader() : shaderOverride;

        if (shaderOverride == nullptr)
            _Material->Bind();
        else
            shader->Bind();
        shader->SetMat4("projectionView", &projectionViewMatrix);
        shader->SetMat4("world"         , &world);
        shader->SetMat4("normalMatrix"  , &normalMatrix);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);

        auto drawMode = _Vertex.Positions.size() == 6 ? GL_LINES : GL_TRIANGLES;

        glDrawElements(drawMode, _Vertex.Indices.size(), GL_UNSIGNED_INT, nullptr);

        if (shaderOverride == nullptr)
            _Material->Unbind();
        else
            shader->Unbind();
    }

    void Mesh::SendVertexDataToGPU()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, _Vertex.Positions.size() * sizeof(float), _Vertex.Positions.data(), GL_DYNAMIC_READ);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _Vertex.Indices.size() * sizeof(int), _Vertex.Indices.data(), GL_DYNAMIC_READ);

        glBindBuffer(GL_ARRAY_BUFFER, _NormalVBO);
        glBufferData(GL_ARRAY_BUFFER, _Vertex.Normals.size() * sizeof(float), _Vertex.Normals.data(), GL_DYNAMIC_READ);

        glBindBuffer(GL_ARRAY_BUFFER, _UVVBO);
        glBufferData(GL_ARRAY_BUFFER, _Vertex.UVs.size() * sizeof(float), _Vertex.UVs.data(), GL_DYNAMIC_READ);

        glBindBuffer(GL_ARRAY_BUFFER, _ColorVBO);
        glBufferData(GL_ARRAY_BUFFER, _Vertex.Colors.size() * sizeof(float), _Vertex.Colors.data(), GL_DYNAMIC_READ);
    }

    void Mesh::BakeTransformIntoGetVertex()
    {
        const glm::mat4 worldMatrix  = _Transform.ComputeWorldMatrix();
        const glm::mat4 normalMatrix = _Transform.ComputeNormalMatrix();

        for (int i = 0; i < _Vertex.Positions.size(); i += 3) {
            glm::vec4 position = { _Vertex.Positions[i], _Vertex.Positions[i + 1], _Vertex.Positions[i + 2], 1.0f };
            glm::vec4 transformedPosition = worldMatrix * position;

            _Vertex.Positions[i    ] = transformedPosition.x;
            _Vertex.Positions[i + 1] = transformedPosition.y;
            _Vertex.Positions[i + 2] = transformedPosition.z;

            glm::vec4 normal                      = { _Vertex.Normals[i], _Vertex.Normals[i + 1], _Vertex.Normals[i + 2], 1.0f };
            glm::vec4 transformedNormal           = normalMatrix * normal;
            glm::vec3 normalizedTransformedNormal = glm::normalize(glm::vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z));

            _Vertex.Normals[i    ] = normalizedTransformedNormal.x;
            _Vertex.Normals[i + 1] = normalizedTransformedNormal.y;
            _Vertex.Normals[i + 2] = normalizedTransformedNormal.z;
        }

        SendVertexDataToGPU();

        _Transform.Reset();
    }

    void Mesh::BakeRotationIntoGetVertex() {
        const glm::mat4 rotationMatrix = _Transform.ComputeRotationMatrix();
        const glm::mat4 normalMatrix   = _Transform.ComputeNormalMatrix();

        for (int i = 0; i < _Vertex.Positions.size(); i += 3) {
            glm::vec4 position            = { _Vertex.Positions[i], _Vertex.Positions[i + 1], _Vertex.Positions[i + 2], 1.0f };
            glm::vec4 transformedPosition = rotationMatrix * position;

            _Vertex.Positions[i    ] = transformedPosition.x;
            _Vertex.Positions[i + 1] = transformedPosition.y;
            _Vertex.Positions[i + 2] = transformedPosition.z;

            glm::vec4 normal                      = { _Vertex.Normals[i], _Vertex.Normals[i + 1], _Vertex.Normals[i + 2], 1.0f };
            glm::vec4 transformedNormal           = normalMatrix * normal;
            glm::vec3 normalizedTransformedNormal = glm::normalize(glm::vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z));

            _Vertex.Normals[i    ] = normalizedTransformedNormal.x;
            _Vertex.Normals[i + 1] = normalizedTransformedNormal.y;
            _Vertex.Normals[i + 2] = normalizedTransformedNormal.z;
        }

        SendVertexDataToGPU();

        _Transform.Rotation = { 0.0f, 0.0f, 0.0f };
    }

    void Mesh::BakeScalingIntoGetVertex()
    {
        const glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), _Transform.Scale);
        const glm::mat4 normalMatrix  = _Transform.ComputeNormalMatrix();

        for (int i = 0; i < _Vertex.Positions.size(); i += 3) {
            glm::vec4 position            = { _Vertex.Positions[i], _Vertex.Positions[i + 1], _Vertex.Positions[i + 2], 1.0f };
            glm::vec4 transformedPosition = scalingMatrix * position;

            _Vertex.Positions[i    ] = transformedPosition.x;
            _Vertex.Positions[i + 1] = transformedPosition.y;
            _Vertex.Positions[i + 2] = transformedPosition.z;

            glm::vec4 normal                      = { _Vertex.Normals[i], _Vertex.Normals[i + 1], _Vertex.Normals[i + 2], 1.0f };
            glm::vec4 transformedNormal           = normalMatrix * normal;
            glm::vec3 normalizedTransformedNormal = glm::normalize(glm::vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z));
            
            _Vertex.Normals[i    ] = normalizedTransformedNormal.x;
            _Vertex.Normals[i + 1] = normalizedTransformedNormal.y;
            _Vertex.Normals[i + 2] = normalizedTransformedNormal.z;
        }

        SendVertexDataToGPU();

        _Transform.SetScale(1.0f);
    }

	PickResult Mesh::Pick(glm::vec3 rayOrigin, glm::vec3 rayDirection)
	{
		if (!_IsPickingEnabled)
            return {};
        glm::mat4 worldMatrix = _Transform.ComputeWorldMatrix();

        return Utils::PickWithRay(_Vertex.Indices, _Vertex.Positions, rayOrigin, rayDirection, worldMatrix);
	}

    void Mesh::SetVertex(Vertex &vertex)
    {
        _Vertex = vertex;

        int vertexLayoutIndex = 0;

        glGenBuffers(1, &_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, _Vertex.Positions.size() * sizeof(float), _Vertex.Positions.data(), GL_DYNAMIC_READ);

        glGenVertexArrays(1, &_VAO);
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glVertexAttribPointer(vertexLayoutIndex, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(vertexLayoutIndex);

        glGenBuffers(1, &_IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _Vertex.Indices.size() * sizeof(int), _Vertex.Indices.data(), GL_DYNAMIC_READ);

        int normalLayoutIndex = 2;

        glGenBuffers(1, &_NormalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, _NormalVBO);
        glBufferData(GL_ARRAY_BUFFER, _Vertex.Normals.size() * sizeof(float), _Vertex.Normals.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(normalLayoutIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(normalLayoutIndex);

        int uvLayoutIndex = 3;

        glGenBuffers(1, &_UVVBO);
        glBindBuffer(GL_ARRAY_BUFFER, _UVVBO);
        glBufferData(GL_ARRAY_BUFFER, _Vertex.UVs.size() * sizeof(float), _Vertex.UVs.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(uvLayoutIndex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(uvLayoutIndex);

        int colorLayoutIndex = 1;

        glGenBuffers(1, &_ColorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, _ColorVBO);
        glBufferData(GL_ARRAY_BUFFER, _Vertex.Colors.size() * sizeof(float), _Vertex.Colors.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(colorLayoutIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(colorLayoutIndex);
    }
};
