#include "MeshPrimitives.hpp"
#include "Vertex.hpp"

namespace Exodia {

    std::shared_ptr<Mesh> MeshPrimitives::UVSphere(const char *name, Scene &scene, int resolution)
    {
        Vertex vertex = {};

        float x;
        float y;
        float z;
        float xy;

        float nx        = 1.0f;
        float ny        = 1.0f;
        float nz        = 1.0f;
        float lengthInv = 1.0f;

        float s;
        float t;

        float sectorStep = 2 * glm::pi<float>() / (float)resolution;
        float stackStep  =     glm::pi<float>() / (float)resolution;

        float sectorAngle;
        float stackAngle;

        for (int i = 0; i <= resolution; i++) {
            stackAngle = glm::pi<float>() / 2 - i * stackStep;

            xy = cosf(stackAngle);
            z  = sinf(stackAngle);

            for (int j = 0; j <= resolution; j++) {
                sectorAngle = j * sectorStep;

                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);

                vertex.Positions.push_back(x);
                vertex.Positions.push_back(z);
                vertex.Positions.push_back(y);

                nx = x * lengthInv;
                ny = z * lengthInv;
                nz = y * lengthInv;

                vertex.Normals.push_back(nx);
                vertex.Normals.push_back(ny);
                vertex.Normals.push_back(nz);

                s = (float)j / (float)resolution;
                t = (float)i / (float)resolution;

                vertex.UVs.push_back(1.0f - s);
                vertex.UVs.push_back(t);
            }
        }

        unsigned int k1;
        unsigned int k2;

        for (int i = 0; i < resolution; i++) {
            k1 =  i * (resolution + 1);
            k2 = k1 +  resolution + 1;

            for (int j = 0; j < resolution; j++) {
                if (i != 0) {
                    vertex.Indices.push_back(k1    );
                    vertex.Indices.push_back(k1 + 1);
                    vertex.Indices.push_back(k2    );
                }

                if (i != (resolution - 1)) {
                    vertex.Indices.push_back(k1 + 1);
                    vertex.Indices.push_back(k2 + 1);
                    vertex.Indices.push_back(k2    );
                }

                k1++;
                k2++;
            }
        }

        for (int i = 0; i < vertex.Positions.size(); i++)
            vertex.Colors.push_back(1.0f);
        std::shared_ptr<Mesh> mesh = Mesh::FromVertex(name, vertex);

        scene.AddMesh(mesh);

        return mesh;
    }

    std::shared_ptr<Mesh> MeshPrimitives::ICOSphere(const char* name, Scene& scene, unsigned int subdivisions)
    {
        Vertex vertex = {};

        const float X = 0.525731112119133606f;
        const float Z = 0.850650808352039932f;
        const float N = 0.f;

        vertex.Positions = {
            -X,  N,  Z,
             X,  N,  Z,
            -X,  N, -Z,
             X,  N, -Z,
             N,  Z,  X,
             N,  Z, -X,
             N, -Z,  X,
             N, -Z, -X,
             Z,  X,  N,
            -Z,  X,  N,
             Z, -X,  N,
            -Z, -X,  N
        };
        vertex.Indices = {
             4,  0,  1,
             9,  0,  4,
             5,  9,  4,
             5,  4,  8,
             8,  4,  1,
            10,  8,  1,
             3,  8, 10,
             3,  5,  8,
             2,  5,  3,
             7,  2,  3,
            10,  7,  3,
             6,  7, 10,
            11,  7,  6,
             0, 11,  6,
             1,  0,  6,
             1,  6, 10,
             0,  9, 11,
            11,  9,  2,
             2,  9,  5,
             2,  7, 11
        };

        for (unsigned int i = 0; i < subdivisions; i++) {
            Utils::Subdivide(vertex.Positions, vertex.Indices);
            Utils::MergeVertices(vertex.Positions, vertex.Indices);
        }

        for (unsigned int i = 0; i < vertex.Positions.size(); i += 3) {
            glm::vec3 position = { vertex.Positions[i], vertex.Positions[i + 1], vertex.Positions[i + 2] };

            position = glm::normalize(position);

            vertex.Positions[i    ] = position.x;
            vertex.Positions[i + 1] = position.y;
            vertex.Positions[i + 2] = position.z;

            float u = 0.5f + std::atan2(position.z, position.x) / (2 * glm::pi<float>());
            float v = 0.5f - std::asin(position.y) / glm::pi<float>();

            vertex.UVs.push_back(1.0f - u);
            vertex.UVs.push_back(v);
        }
        vertex.ComputeNormals();

        std::shared_ptr<Mesh> mesh = Mesh::FromVertex(name, vertex);

        scene.AddMesh(mesh);

        return mesh;
    }
};
