#pragma once

#include "Utils/Observable.hpp"
#include "Components/Transformable.hpp"

#include <glm/glm.hpp>
#include <string>

namespace Exodia {

    class Light {

        public:

            explicit Light(const char* name) : _Name(name) {};

        public:

            inline void SetColor(float r, float g, float b)
            {
				_Color = { r, g, b };

                OnColorChanged.NotifyObservers(_Color);
            }

            inline glm::vec3 GetColor()
            {
                return _Color;
            }

            float GetIntensity() const
            {
                return _Intensity;
            };

        public:

            Observable<glm::vec3> OnColorChanged;

        private:
            std::string _Name;

            float _Intensity = 1.0f;

            glm::vec3 _Color = glm::vec3(1.0f);
    };

    class PointLight : public Light, public Transformable {

        public:

            explicit PointLight(const char *name) : Light(name), Transformable() {};
    };

    class DirectionalLight : public Light {

        public:

            DirectionalLight(const char *name, float x, float y, float z) : Light(name), _Direction({ x, y, z }) {};
			DirectionalLight(const char *name, glm::vec3 direction) : Light(name), _Direction(direction) {};

        public:

            inline glm::vec3 GetDirection()
            {
                return _Direction;
            }

            inline void SetDirection(glm::vec3 direction)
            {
                _Direction = direction;
            }

        private:
            glm::vec3 _Direction {};
    };
};
