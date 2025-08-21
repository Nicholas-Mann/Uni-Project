#include "Perlin.hpp"
#include <iostream>
#include "../../../Rendering/src/Objects/SimpleShapes/Cube.hpp"
#include "../../../Core/src/Core/Scene.hpp"
#include <chrono>

int Perlin::addNoiseSize = 100;
float Perlin::addWeight = 60.0f;
int Perlin::addOctaves = 9;

Perlin::Perlin(std::string name, int chunkSize, int chunkResolution, int renderDistance, int seed) : Terrain(name, chunkSize, chunkResolution, renderDistance, seed)
{
	terrainType = "Perlin";
}

Perlin::Perlin(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int chunkSize, int chunkResolution, int renderDistance, int seed) : Terrain(name, pos, rot, scale, chunkSize, chunkResolution, renderDistance, seed)
{
	terrainType = "Perlin";
}

void Perlin::generateHighMap(Chunk* chunk, Terrain* terrain)
{
	debugInit(chunk);

	// Start Time
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	// Lock the chunk
	chunk->setProcessing(true);

	float* heightMap = chunk->getHeightMap();

	for (int x = 0; x < chunk->getHeightMapSize(); x++) {
		for (int z = 0; z < chunk->getHeightMapSize(); z++) {
			heightMap[x * chunk->getHeightMapSize() + z] = 0.0f;
		}
	}

	for (int i = 0; i < octaves; i++) {
		float factor = pow(2, i);

		float* localMap = createPerlin(chunk, noiseSize * factor, weight / factor, seed);

		for (int x = 0; x < chunk->getHeightMapSize(); x++) {
			for (int z = 0; z < chunk->getHeightMapSize(); z++) {
				heightMap[x * chunk->getHeightMapSize() + z] += localMap[x * chunk->getHeightMapSize() + z];
			}
		}

		debugStep(chunk, heightMap);

		delete[] localMap;
	}


	//chunk->setHeightMap(heightMap);

	// Set the next state of the chunk
	chunk->setChunkState(-2);

	// Unlock the chunk
	chunk->setProcessing(false);

	// Print the time taken
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = std::chrono::duration_cast<std::chrono::duration<float>>(end - start);
	std::cout << "Time taken to generate height map: " << duration.count() << " seconds" << std::endl;
}

float* Perlin::createPerlin(Chunk* chunk, float noiseSize, float weight, float seed)
{
	int resolution = chunk->getHeightMapSize();
	int chunkSize = chunk->getSize();

	float triangleSize = (float)chunkSize / ((float)chunk->getResolution() - 1);

	glm::vec2 chunkPos = chunk->getChunkPos();

	float* heightMap = new float[(resolution * resolution)];

	for (int x = 0; x < resolution; x++) {
		for (int y = 0; y < resolution; y++) {


			// Convert the x and y to world space
			float worldX = ((x-1) * triangleSize + chunkPos.x * chunkSize);
			float worldY = ((y-1) * triangleSize + chunkPos.y * chunkSize);

			// Find the noise grid position
			float noiseGridPosX = std::floor(worldX / noiseSize);
			float noiseGridPosY = std::floor(worldY / noiseSize);

			float noiseInWorldX = noiseGridPosX * noiseSize;
			float noiseInWorldY = noiseGridPosY * noiseSize;

			// Calculate the positoin within the cell
			float cellOffsetX = worldX - (float)noiseGridPosX * noiseSize;
			if (cellOffsetX < 0)
				cellOffsetX += noiseSize;
			float cellOffsetY = worldY - (float)noiseGridPosY * noiseSize;
			if (cellOffsetY < 0)
				cellOffsetY += noiseSize;

			// Normalise the valuse so that they can be used to interpolate
			float normX = cellOffsetX / noiseSize;
			float normY = cellOffsetY / noiseSize;

			// Calculate dot products
			float n0 = dot(glm::vec2(noiseGridPosX, noiseGridPosY), glm::vec2(noiseGridPosX + normX, noiseGridPosY + normY), noiseSize, seed);
			float n1 = dot(glm::vec2(noiseGridPosX + 1, noiseGridPosY), glm::vec2(noiseGridPosX + normX, noiseGridPosY + normY), noiseSize, seed);
			float ix0 = interpolate(n0, n1, normX);

			n0 = dot(glm::vec2(noiseGridPosX, noiseGridPosY + 1), glm::vec2(noiseGridPosX + normX, noiseGridPosY + normY), noiseSize, seed);
			n1 = dot(glm::vec2(noiseGridPosX + 1, noiseGridPosY + 1), glm::vec2(noiseGridPosX + normX, noiseGridPosY + normY), noiseSize, seed);
			float ix1 = interpolate(n0, n1, normX);

			float height = interpolate(ix0, ix1, normY);

			heightMap[x * resolution + y] = height * weight;
		}
	}

	return heightMap;
}

void Perlin::ImGuiWindow()
{
	ImGui::Begin("Object");

	ImGuiTerrainDefault();

	ImGui::Separator();

	// Terrain Settings
	ImGui::Text("Terrain Settings");

	// Noise Size
	ImGui::InputInt("Noise Size", &this->noiseSize);

	// Weight
	ImGui::InputFloat("Weight", &this->weight);

	// Octaves
	ImGui::InputInt("Octaves", &this->octaves);
	if (this->octaves < 1) {
		this->octaves = 1;
	}
	float factor = pow(2, this->octaves);
	ImGui::Text("Larges Map Size: %f", factor);

	ImGui::End();
}

float Perlin::dot(glm::vec2 noisePos, glm::vec2 gridPos, float noiseSize, float seed) {
	// Calculate the noise
	float angle = (float)(MathsFunction::randomPos(noisePos.x * noiseSize, noisePos.y * noiseSize, seed) * 2.0f * 3.14159f);
	float x = cos(angle);
	float y = sin(angle);

	// Calcualte the distance between the noise and the grid position
	float dx = gridPos.x - noisePos.x;
	float dy = gridPos.y - noisePos.y;

	// Compute the dot-product
	return (dx * x + dy * y);
}

void Perlin::ImGuiWindowAdd() {
	ImGui::Separator();

	// Inital Perlin Settings
	ImGui::Text("Perlin Settings");

	ImGui::InputInt("Noise Size", &addNoiseSize);
	ImGui::InputFloat("Weight", &addWeight);
	ImGui::InputInt("Octaves", &addOctaves);
	float factor = pow(2, addOctaves);
	ImGui::Text("Larges Map Size: %f", factor);

	ImGui::Separator();
}
