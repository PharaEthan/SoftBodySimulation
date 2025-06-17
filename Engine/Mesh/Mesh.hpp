#pragma once

#include "Components/Transformable.hpp"
#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"
#include "Components/Pickable.hpp"
#include "Materials/Material.hpp"
#include "Utils/UUID.hpp"
#include "Box/AABB.hpp"
#include "Vertex.hpp"

#include <vector>
#include <memory>

namespace Exodia {

    class Mesh : public Transformable, public Renderable, public Pickable {

        public:

            static std::shared_ptr<Material> BaseMaterial;

        public:

            static std::shared_ptr<Mesh> FromVertex(const char *name, Vertex &vertex);

        public:

            explicit Mesh(const char *name);

        public:

            void Render(glm::mat4 projectionViewMatrix, Shader *shaderOverride = nullptr) override;

            void BakeTransformIntoGetVertex();

            void BakeRotationIntoGetVertex();

            void BakeScalingIntoGetVertex();

            void SendVertexDataToGPU();

            PickResult Pick(glm::vec3 rayOrigin, glm::vec3 rayDirection) override;

        public:

            const std::string &Name() const
            {
                return _Name;
            }

            Exodia::Transform *Transform() override
            {
                return &_Transform;
            }

            AABB *GetAABB()
            {
                return &_AABB;
            }

            void SetMaterial(std::shared_ptr<Material> material)
            {
				_Material = material;
            }

            std::shared_ptr<Material> GetMaterial() const
            {
                return _Material;
            }

            void SetVertex(Vertex &vertex); // In cpp, cause using <glad/glad.h>

            Vertex &GetVertex()
            {
                return _Vertex;
            }

            void SetPickingEnabled(bool enabled) override
			{
				_IsPickingEnabled = enabled;
            }

            bool IsPickingEnabled() const override
            {
                return _IsPickingEnabled;
            }

        public:

            bool operator==(const Mesh& other) const
            {
                return _ID == other._ID;
            }

        public:

            bool Enabled = true;

        protected:
    
            std::shared_ptr<Material> _Material;

        private:

            std::string _Name;
            std::string _ID;

            Vertex _Vertex;

            Exodia::Transform _Transform;

            AABB _AABB;

            unsigned int _VAO       {};
            unsigned int _VBO       {};
            unsigned int _IBO       {};
            unsigned int _NormalVBO {};
            unsigned int _UVVBO     {};
            unsigned int _ColorVBO  {};

			bool _IsPickingEnabled = true;
    };
};
