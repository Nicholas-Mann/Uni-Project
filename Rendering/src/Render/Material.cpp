#include "Material.hpp"

//#include <GLFW/glfw3.h>
#include <iostream>
#include "Program.hpp"


Material::Material() {
	this->m_ambient = 0.1f;
	this->m_diffuse = 0.9f;
	this->m_specular = 0.9f;
	this->m_emissive = glm::vec3{ 0.f, 0.f, 0.f };
	this->m_shininess = 0.9f;
}

Material::Material(float m_ambient, float m_diffuse, float m_specular, float m_shininess, glm::vec3 m_emissive) {
	this->m_ambient = m_ambient;
	this->m_diffuse = m_diffuse;
	this->m_specular = m_specular;
	this->m_emissive = m_emissive;
	this->m_shininess = m_shininess;
}

Material::~Material() {
}

void Material::useMaterial(GLuint prog) {
	ShaderProgram::loadUniformFloat(prog, "uMaterialAmbient", this->m_ambient);
	ShaderProgram::loadUniformFloat(prog, "uMaterialDiffuse", this->m_diffuse);
	ShaderProgram::loadUniformFloat(prog, "uMaterialSpec", this->m_specular);
	ShaderProgram::loadUniformVec3(prog, "uMaterialEmissive", this->m_emissive);
	ShaderProgram::loadUniformFloat(prog, "uMaterialShininess", this->m_shininess);
}