#include <glad/glad.h>
#include <stdexcept>

#include "Shader.hpp"

namespace Exodia {

    Shader::Shader(const char *shaderFolder)
    {
        std::string vertexPath;
        std::string fragmentPath;

        vertexPath.append(shaderFolder);
        fragmentPath.append(shaderFolder);

        vertexPath   += "/vertex.glsl";
        fragmentPath += "/fragment.glsl";

        LoadFileToBuffer(vertexPath.c_str()  , _VertexShaderCode);
        LoadFileToBuffer(fragmentPath.c_str(), _FragmentShaderCode);

        OnBeforeBind.AddOnce([this]() {
            Compile();
        });
    }

    void Shader::Compile()
    {
        _Program = glCreateProgram();

        std::string versionedVertexShaderCode   = "#version 450 core\n" + _VertexShaderCode;
        std::string versionedFragmentShaderCode = "#version 450 core\n" + _FragmentShaderCode;

        CompileShader(versionedVertexShaderCode  , GL_VERTEX_SHADER);
        CompileShader(versionedFragmentShaderCode, GL_FRAGMENT_SHADER);

        int  result       = 0;
        char errLog[1024] = { 0 };

        glLinkProgram(_Program);
        glGetProgramiv(_Program, GL_LINK_STATUS, &result);

        if (!result) {
            glGetProgramInfoLog(_Program, sizeof(errLog), NULL, errLog);

            std::cerr << "Error linking program: '" << errLog << "'\n";

            return;
        }

        glValidateProgram(_Program);
        glGetProgramiv(_Program, GL_VALIDATE_STATUS, &result);

        if (!result) {
            glGetProgramInfoLog(_Program, sizeof(errLog), NULL, errLog);

            std::cerr << "Error validating program: '" << errLog << "'\n";

            return;
        }
    }

    void Shader::Bind()
    {
        OnBeforeBind.NotifyObservers();

        glUseProgram(_Program);

        OnAfterBind.NotifyObservers();
    }

    void Shader::Unbind() {};

    void Shader::BindTexture(const char* uniformName, Texture* texture, int id) const
    {
        glUniform1i(glGetUniformLocation(_Program, uniformName), id);

        texture->Bind(id);
    }

    unsigned int Shader::CompileShader(std::string &shaderCode, unsigned int shaderType) const
    {
        unsigned int fs = glCreateShader(shaderType);

        const char *fragmentShaderCString = shaderCode.c_str();

        glShaderSource(fs, 1, &fragmentShaderCString, nullptr);
        glCompileShader(fs);

        int  success;
        char infoLog[512];

        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(fs, 512, nullptr, infoLog);

            throw std::runtime_error("ERROR in compiling shader\n" + std::string(infoLog) + "\n" + shaderCode);
        }

        glAttachShader(_Program, fs);

        return fs;
    }

    void Shader::SetMat4(const char *uniformName, const glm::mat4 *matrix) const
    {
        glUniformMatrix4fv(glGetUniformLocation(_Program, uniformName), 1, GL_FALSE, glm::value_ptr(*matrix));
    }

    void Shader::SetVec2(const char *uniformName, const float x, const float y) const
    {
        glUniform2f(glGetUniformLocation(_Program, uniformName), x, y);
    }

    void Shader::SetVec2(const char* uniformName, const glm::vec2 vector) const
    {
        glUniform2fv(glGetUniformLocation(_Program, uniformName), 1, glm::value_ptr(vector));
    }

    void Shader::SetVec3(const char* uniformName, const glm::vec3 vector) const
    {
        glUniform3fv(glGetUniformLocation(_Program, uniformName), 1, glm::value_ptr(vector));
    }

    void Shader::SetInt(const char* uniformName, int integer) const
    {
        glUniform1i(glGetUniformLocation(_Program, uniformName), integer);
    }

    void Shader::SetFloat(const char* uniformName, float value) const
    {
        glUniform1f(glGetUniformLocation(_Program, uniformName), value);
    }

    void Shader::SetBool(const char* uniformName, bool value) const
    {
        glUniform1i(glGetUniformLocation(_Program, uniformName), value);
    }
};
