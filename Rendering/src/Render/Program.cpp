#include "Program.hpp"

#include <cassert>
#include <filesystem>

#include "../imgui/imgui.h"

#include <cstdint>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

std::map<std::string, GLuint> ShaderProgram::programsList;

void ShaderProgram::registerShader(std::string const& nickName, std::string const& aFragPath, std::string const& aVertPath)
{
	// Open the files
	FILE* fragFile = fopen(aFragPath.c_str(), "rb");
	FILE* vertFile = fopen(aVertPath.c_str(), "rb");

	// Check if the files are open
	if (fragFile == nullptr) {
		std::cerr << "Failed to open fragment shader file: " << aFragPath << std::endl;
		std::cerr << "Path Checked is: " << std::filesystem::current_path() << aFragPath << std::endl;
		return;
	}

	if (vertFile == nullptr) {
		std::cerr << "Failed to open vertex shader file: " << aVertPath << std::endl;
		std::cerr << "Fill Path checked is: " << std::filesystem::current_path() << aVertPath << std::endl;
		return;
	}

	// Read the contents of the files to strings
	std::string fragSource;
	std::string vertSource;

	// Read the fragment shader
	{
		fseek(fragFile, 0, SEEK_END);
		long size = ftell(fragFile);
		fseek(fragFile, 0, SEEK_SET);
		fragSource.resize(size);
		fread(fragSource.data(), 1, size, fragFile);
	}

	// Read the vertex shader
	{
		fseek(vertFile, 0, SEEK_END);
		long size = ftell(vertFile);
		fseek(vertFile, 0, SEEK_SET);
		vertSource.resize(size);
		fread(vertSource.data(), 1, size, vertFile);
	}

	// Close the files
	fclose(fragFile);
	fclose(vertFile);

	// Create the shaders
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);

	// Compile the shaders
	{
		const char* fragSourceCStr = fragSource.c_str();
		const char* vertSourceCStr = vertSource.c_str();
		glShaderSource(fragShader, 1, &fragSourceCStr, nullptr);
		glShaderSource(vertShader, 1, &vertSourceCStr, nullptr);
		glCompileShader(fragShader);
		glCompileShader(vertShader);
		// Check if the shaders compiled
		GLint fragStatus;
		GLint vertStatus;
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &fragStatus);
		glGetShaderiv(vertShader, GL_COMPILE_STATUS, &vertStatus);
		if (fragStatus == GL_FALSE) {
			GLint logLength;
			glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
			std::string log(logLength, ' ');
			glGetShaderInfoLog(fragShader, logLength, &logLength, log.data());
			std::cerr << "Failed to compile fragment shader: " << log << std::endl;
			return;
		}
		if (vertStatus == GL_FALSE) {
			GLint logLength;
			glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
			std::string log(logLength, ' ');
			glGetShaderInfoLog(vertShader, logLength, &logLength, log.data());
			std::cerr << "Failed to compile vertex shader: " << log << std::endl;
			return;
		}
	}

	// Create the program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	// Check if the program linked
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		std::string log(logLength, ' ');
		glGetProgramInfoLog(program, logLength, &logLength, log.data());
		std::cerr << "Failed to link shader program: " << log << std::endl;
		return;
	}

	// Validate the program
	glValidateProgram(program);
	GLint validateStatus;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus);
	if (validateStatus == GL_FALSE) {
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		std::string log(logLength, ' ');
		glGetProgramInfoLog(program, logLength, &logLength, log.data());
		std::cerr << "Failed to validate shader program: " << log << std::endl;
		return;
	}

	// Add shader to programs
	programsList[nickName] = program;

	listUniforms(nickName);

	std::cout << "Shader program created: " << program << std::endl;
}

GLuint ShaderProgram::getShader(std::string const& nickName)
{
	return programsList[nickName];
}

void ShaderProgram::deleteShader(std::string const& nickName)
{
	glDeleteProgram(programsList[nickName]);
	programsList.erase(nickName);
}

void ShaderProgram::deleteAllShaders()
{
	for (auto& program : programsList) {
		glDeleteProgram(program.second);
	}
	programsList.clear();
}

void ShaderProgram::useProgram(std::string const& nickName)
{
	glUseProgram(programsList[nickName]);
}

void ShaderProgram::ImGuiWindowInfo() {
	if (ImGui::TreeNode("Shader Programs")) {
		for (auto& program : programsList) {
			if (ImGui::TreeNode(program.first.c_str())) {
				GLint numUniforms = 0;
				glGetProgramiv(program.second, GL_ACTIVE_UNIFORMS, &numUniforms);

				if (numUniforms <= 0) {
					ImGui::Text("No active uniforms in program: %s", program.first.c_str());
				}

				for (GLint i = 0; i < numUniforms; ++i)
				{
					char name[256];
					GLsizei length = 0;
					GLint size = 0;
					GLenum type = 0;
					glGetActiveUniform(program.second, i, sizeof(name), &length, &size, &type, name);

					std::string baseName = "Uniform #" + std::to_string(i) + " " + name;
					if (ImGui::TreeNode(baseName.c_str())) {
						ImGui::Text("  Type: %d", type);
						ImGui::Text("  Size: %d", size);
						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}

void ShaderProgram::loadUniformMat4(GLuint program, std::string const& name, glm::mat4 const& value)
{
	GLint id = glGetUniformLocation(program, name.c_str());

	if (id == -1) {
		std::cerr << "Failed to find uniform: \"" << name << "\" in shader " << program << std::endl;
		return;
	}
	else {
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void ShaderProgram::loadUniformMat3(GLuint program, std::string const& name, glm::mat3 const& value)
{
	GLint id = glGetUniformLocation(program, name.c_str());
	if (id == -1) {
		std::cerr << "Failed to find uniform: \"" << name << "\" in shader " << program << std::endl;
		return;
	}
	else {
		glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void ShaderProgram::loadUniformVec3(GLuint program, std::string const& name, glm::vec3 const& value)
{
	GLint id = glGetUniformLocation(program, name.c_str());
	if (id == -1) {
		std::cerr << "Failed to find uniform: \"" << name << "\" in shader " << program << std::endl;
		return;
	}
	else {
		glUniform3fv(id, 1, glm::value_ptr(value));
	}
}

void ShaderProgram::loadUniformFloat(GLuint program, std::string const& name, float value)
{
	GLint id = glGetUniformLocation(program, name.c_str());
	if (id == -1) {
		std::cerr << "Failed to find uniform: \"" << name << "\" in shader " << program << std::endl;
		return;
	}
	else {
		glUniform1f(id, value);
	}
}

void ShaderProgram::loadUniformInt(GLuint program, std::string const& name, int value)
{
	GLint id = glGetUniformLocation(program, name.c_str());
	if (id == -1) {
		std::cerr << "Failed to find uniform: \"" << name << "\" in shader " << program << std::endl;
		return;
	}
	else {
		glUniform1i(id, value);
	}
}
