#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

class Material
{
public:
	Material();
	Material(float m_ambient, float m_diffuse, float m_specular, float m_shininess, glm::vec3 m_emissive);
	~Material();

	void useMaterial(GLuint prog);

	float getAmbient() { return m_ambient; }
	void setAmbient(float ambient) { m_ambient = ambient; }
	float getDiffuse() { return m_diffuse; }
	void setDiffuse(float diffuse) { m_diffuse = diffuse; }
	float getSpecular() { return m_specular; }
	void setSpecular(float specular) { m_specular = specular; }
	glm::vec3 getEmissive() { return m_emissive; }
	void setEmissive(glm::vec3 emissive) { m_emissive = emissive; }

private:
	float m_ambient = 0;
	float m_diffuse = 0;
	float m_specular = 0;
	glm::vec3 m_emissive = glm::vec3(0.0f);
	float m_shininess = 0;
};

