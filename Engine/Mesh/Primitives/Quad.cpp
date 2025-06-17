#include <glad/glad.h>

#include "MeshPrimitives.hpp"
#include "ScreenQuad.hpp"
#include "Vertex.hpp"

namespace Exodia {

    std::shared_ptr<Mesh> MeshPrimitives::Plane(const char *name, Scene &scene, unsigned int subdivisions)
    {
        Vertex vertex = {};

        vertex.Positions = std::vector<float>(subdivisions * subdivisions * 3);
        vertex.Indices   = std::vector<int>((subdivisions - 1) * (subdivisions - 1) * 3 * 2);
        vertex.UVs       = std::vector<float>(subdivisions * subdivisions * 2);
        vertex.Normals   = std::vector<float>(subdivisions * subdivisions * 3);

        float size = 1.0f;

        for (int x = 0; x < subdivisions; x++) {
            float xPosition = ((float)x / (float)(subdivisions - 1)) * size - size / 2.0f;
            float xUV       =  (float)x / (float)(subdivisions - 1);

            for (int z = 0; z < subdivisions; z++) {
                float zPosition = ((float)z / (float)(subdivisions - 1)) * size - size / 2.0f;
                float zUV       =  (float)z / (float)(subdivisions - 1);

                int positionIndex = 3 * (x * subdivisions + z);

                vertex.Positions[positionIndex    ] = xPosition;
                vertex.Positions[positionIndex + 1] = 0.0f;
                vertex.Positions[positionIndex + 2] = zPosition;

                int normalIndex = 3 * (x * subdivisions + z);

                vertex.Normals[normalIndex    ] = 0.0f;
                vertex.Normals[normalIndex + 1] = 1.0f;
                vertex.Normals[normalIndex + 2] = 0.0f;

                int uvIndex = 2 * (x * subdivisions + z);

                vertex.UVs[uvIndex    ] = xUV;
                vertex.UVs[uvIndex + 1] = zUV;

                if (x == subdivisions - 1 || z == subdivisions - 1)
                    continue;
                int index = 6 * (x * (subdivisions - 1) + z);

                vertex.Indices[index    ] = (x + 1) * subdivisions + z;
                vertex.Indices[index + 1] =  x      * subdivisions + z;
                vertex.Indices[index + 2] =  x      * subdivisions + z + 1;
                vertex.Indices[index + 3] = (x + 1) * subdivisions + z;
                vertex.Indices[index + 4] =  x      * subdivisions + z + 1;
                vertex.Indices[index + 5] = (x + 1) * subdivisions + z + 1;
            }
        }

        std::shared_ptr<Mesh> mesh = Mesh::FromVertex(name, vertex);

        scene.AddMesh(mesh);

        return mesh;
    }

    std::shared_ptr<Mesh> MeshPrimitives::ScreenQuad(const char *name)
    {
        Vertex vertex = {};

        vertex.Positions = {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f, 0.0f
        };
        vertex.Indices = {
            0, 1, 2,
            1, 3, 2
        };
        vertex.UVs = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };
        vertex.Normals = {
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f
        };

        std::shared_ptr<Mesh> mesh = Mesh::FromVertex(name, vertex);

        // TODO: Remove the picking of this object

        return mesh;
    }

    ScreenQuad::ScreenQuad()
    {
        std::vector<float> positions = {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
             1.0f, 1.0f, 0.0f
        };
        std::vector<float> uvs = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };

        int vertexLayoutIndex = 0;

        glGenBuffers(1, &_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_READ);

        glGenVertexArrays(1, &_VAO);
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glVertexAttribPointer(vertexLayoutIndex, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(vertexLayoutIndex);

        glGenBuffers(1, &_IVO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IVO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(int), Indices.data(), GL_DYNAMIC_READ);

        int uvLayoutIndex = 1;

        glGenBuffers(1, &_UVVBO);
        glBindBuffer(GL_ARRAY_BUFFER, _UVVBO);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(uvLayoutIndex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(uvLayoutIndex);
    }

    void ScreenQuad::Render()
    {
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IVO);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, nullptr);
    }
};
