#pragma once
#include <glad/glad.h>
#include <string>
#include "../../../Rendering/src/Render/Material.hpp"
#include <memory>


class Mesh
{
public:
	Mesh();
	Mesh(std::string const& shaderName);
	~Mesh();
	void registerMesh();
	void updateMesh(std::shared_ptr<float[]> vertices, unsigned int verticesSize, std::shared_ptr<float[]> colour, unsigned int colourSize, std::shared_ptr<float[]> normals, unsigned int normalsSize, std::shared_ptr<unsigned int[]> indices, unsigned int indicesSize);
	void refreshMesh();
	
	void drawMesh(glm::mat4 modelTranslation, Material *material);
	void cleanUp();

private:
	std::shared_ptr<float[]> vertices;
	std::shared_ptr<float[]> colour;
	std::shared_ptr<float[]> normals;
	std::shared_ptr<unsigned int[]> indices;
	int vertexCount;
	int colourCount;
	int normalCount;
	int indexCount;

	bool needsRenderData = false;
	bool redistered = false;

	GLuint vao, ebo, vboPos, vboColour, vboNormal;
	GLuint shader;
	unsigned int size;
};

