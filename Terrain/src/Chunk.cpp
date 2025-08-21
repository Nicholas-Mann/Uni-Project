#include "Chunk.hpp"
#include "../../../Rendering/src/Objects/SimpleShapes/Pane.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>



Chunk::Chunk(glm::ivec2 chunkPos, Terrain* terrainRef, int resolution, int size)
{
	this->chunkPos = chunkPos;
	this->terrainRef = terrainRef;
	this->resolution = resolution;
	this->size = size;
	this->heightMapSize = resolution + 2;
}

Chunk::~Chunk()
{
	closing = true;
	delete[] heightMap;
	heightMap = nullptr;
	mesh.cleanUp();
}

void Chunk::draw(glm::mat4 modelPos)
{
	modelPos = glm::translate(modelPos, this->position);
	modelPos = glm::rotate(modelPos, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelPos = glm::rotate(modelPos, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelPos = glm::rotate(modelPos, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelPos = glm::scale(modelPos, this->scale);

	mesh.drawMesh(modelPos, this->material);
}
void Chunk::updateMesh(float triangleSie, int chuckResilution)
{
	processing = true;

	Pane::UpdateTerrain(&mesh, "terrain", heightMap, triangleSie, chuckResilution);

	this->heightMap = heightMap;

	chunkState = -3;
	processing = false;
}

void Chunk::createMesh()
{
	processing = true;

	Mesh mesh = Mesh("terrain");
	mesh.registerMesh();
	this->mesh = mesh;

	chunkState = -1;
	processing = false;
}