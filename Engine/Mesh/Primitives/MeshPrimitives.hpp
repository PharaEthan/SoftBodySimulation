#pragma once

#include "Mesh.hpp"
#include "Scene/Scene.hpp"

#include <iostream>
#include <glm/glm.hpp>
#include <memory>

namespace Exodia {

    class MeshPrimitives {

        public:

            static std::shared_ptr<Mesh> Cube(const char* name, Scene &scene);
            static std::shared_ptr<Mesh> UVCube(const char* name, Scene &scene);

            static std::shared_ptr<Mesh> UVSphere(const char *name, Scene &scene, int resolution);
            static std::shared_ptr<Mesh> ICOSphere(const char*name, Scene &scene, unsigned int subdivisions);

            static std::shared_ptr<Mesh> Plane(const char *name, Scene &scene, unsigned int subdivisions);
            static std::shared_ptr<Mesh> ScreenQuad(const char *name);
    };
};
