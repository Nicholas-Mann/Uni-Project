#ifndef PROGRAM_HPP_39793FD2_7845_47A7_9E21_6DDAD42C9A09
#define PROGRAM_HPP_39793FD2_7845_47A7_9E21_6DDAD42C9A09

#include <glad/glad.h>

#include <string>
#include <map>
#include <iostream>
#include <glm/glm.hpp>


class ShaderProgram final
{
public:
	static void registerShader(std::string const& nickName, std::string const& aFragPath, std::string const& aVertPath);

	static GLuint getShader(std::string const& nickName);

	static void deleteShader(std::string const& nickName);

	static void deleteAllShaders();

	static void useProgram(std::string const& nickName);

	static void loadUniformMat4(GLuint program, std::string const& name, glm::mat4 const& value);

	static void loadUniformMat3(GLuint program, std::string const& name, glm::mat3 const& value);

	static void loadUniformVec3(GLuint program, std::string const& name, glm::vec3 const& value);

	static void loadUniformFloat(GLuint program, std::string const& name, float value);

	static void loadUniformInt(GLuint program, std::string const& name, int value);

    static void listUniforms(std::string const& nickName)
    {
        auto it = programsList.find(nickName);
        if (it == programsList.end())
        {
            std::cerr << "Shader program not found: " << nickName << std::endl;
            return;
        }

        GLuint program = it->second;
        GLint numUniforms = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

        if (numUniforms <= 0) {
			std::cout << "No active uniforms in program: " << nickName << std::endl;
        }

        for (GLint i = 0; i < numUniforms; ++i)
        {
            char name[256];
            GLsizei length = 0;
            GLint size = 0;
            GLenum type = 0;
            glGetActiveUniform(program, i, sizeof(name), &length, &size, &type, name);

            std::cout << "Uniform #" << i << ": " << name << " (Type: " << type << ", Size: " << size << ")" << std::endl;
        }
    }

    static void ImGuiWindowInfo();

private:
	static std::map<std::string, GLuint> programsList;
};

#endif // PROGRAM_HPP_39793FD2_7845_47A7_9E21_6DDAD42C9A09
