#include "Light.hpp"

#include <glm/gtc/type_ptr.hpp>

Light::Light(glm::vec3 m_position, glm::vec3 m_colour, float m_intensity) {
	this->m_position = m_position;
	this->m_colour = m_colour;
	this->m_intensity = m_intensity;
}

Light::~Light() {
}

void Light::renderLight(GLuint prog, int lightIndex) {
    std::string baseName = "uLights[" + std::to_string(lightIndex) + "]";

    // Set position
	ShaderProgram::loadUniformVec3(prog, (baseName + ".position"), this->m_position);

    // Set color
	ShaderProgram::loadUniformVec3(prog, (baseName + ".color"), this->m_colour);

    // Set scalar values
	ShaderProgram::loadUniformFloat(prog, (baseName + ".intensity"), this->m_intensity);
}

void Light::ImGuiWindowInfo(int index) {
	std::string baseName = "Lights[" + std::to_string(index) + "]";

	if (ImGui::TreeNode(baseName.c_str())) {
		ImGui::Text("Position");
		ImGui::DragFloat3("Position", &m_position[0], 0.1f);
		ImGui::Text("Color");
		ImGui::ColorEdit3("Light Color", glm::value_ptr(m_colour));
		ImGui::Text("Intensity");
		ImGui::SliderFloat("Intensity", &m_intensity, 0.0f, 10.0f);
		ImGui::TreePop();
	}
}
