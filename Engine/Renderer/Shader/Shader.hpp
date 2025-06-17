#pragma once

#include "Utils/Observable.hpp"
#include "Texture/Texture.hpp"

#include <string>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>

namespace Exodia {

    class Shader {

        public:

            explicit Shader(const char* shaderFolder);
            
        public:

            void Compile();

            virtual void Bind();
            virtual void Unbind();

            void BindTexture(const char* uniformName, Texture* texture, int id) const;

        private:

            unsigned int CompileShader(std::string& shaderCode, unsigned int shaderType) const;

        public:

            void SetMat4(const char* uniformName, const glm::mat4* matrix) const;
            void SetVec2(const char* uniformName, const float x, const float y) const;
            void SetVec2(const char* uniformName, const glm::vec2 vector) const;
            void SetVec3(const char* uniformName, const glm::vec3 vector) const;
            void SetInt(const char* uniformName, int integer) const;
            void SetFloat(const char* uniformName, float value) const;
            void SetBool(const char* uniformName, bool value) const;

            void SetDefine(const char *defineName)
            {
                _FragmentShaderCode = "#define " + std::string(defineName) + "\n" + _FragmentShaderCode;
                _VertexShaderCode   = "#define " + std::string(defineName) + "\n" + _VertexShaderCode;
            }

            unsigned int GetProgram() const
            {
                return _Program;
            }

        public:

            Observable<> OnBeforeBind {};
            Observable<> OnAfterBind  {};

        private:

            unsigned int _Program {};

            std::string _VertexShaderCode   {};
            std::string _FragmentShaderCode {};
    };
};
