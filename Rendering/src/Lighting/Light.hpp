#pragma once

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "../Render/Program.hpp"

#include "../imgui/imgui.h"

class Light
{
public:
	Light(glm::vec3 m_position, glm::vec3 m_colour, float m_intensity);
	~Light();

	void set_position(glm::vec3 position) {
		this->m_position = position;
	}
	void set_colour(glm::vec3 colour) {
		this->m_colour = colour;
	}
	void set_intensity(float intensity) {
		this->m_intensity = intensity;
	}
	glm::vec3 get_position() {
		return this->m_position;
	}
	glm::vec3 get_colour() {
		return this->m_colour;
	}
	float get_intensity() {
		return this->m_intensity;
	}

	void renderLight(GLuint prog, int lightIndex);

	void ImGuiWindowInfo(int index);

private:
	glm::vec3 m_position;
	glm::vec3 m_colour;
	float m_intensity;
};