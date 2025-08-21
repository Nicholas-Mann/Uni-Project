#pragma once

#include <vector>
#include <unordered_map>

#include "../../../Rendering/src/Objects/Object.hpp"
#include "Chunk.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

class TerrainManager;

class Terrain: public Object
{
public:
	Terrain(std::string name, int chunkSize, int chunkResolution, int renderDistance, int seed);
	Terrain(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int chunkSize, int chunkResolution, int renderDistance, int seed);
	~Terrain();
	void draw(glm::mat4 modelPos = glm::mat4(1.0f)) override;
	void updateTerrain();
	void processTerrain();
	virtual void registerChunk(Chunk* chunk);
	virtual void generateHighMap(Chunk* chunk, Terrain* terrain);
	void regenerateTerrain();
	void updateTexture();
	Chunk* getChunk(glm::ivec2 chunkPos) { return chunks[chunkPos]; }


	void ImGuiWindow() override;
	void ImGuiTerrainDefault();
	
	static Terrain* ImGuiWindowAdd();
	static void showNewTerrainWindow() { 
		showNewTerrain = true; 
		posUI = glm::vec3(0.0f);
		rotUI = glm::vec3(0.0f);
		sclUI = glm::vec3(1.0f);
	}

	static std::string checkName(std::string desiredName);

	static bool showTextureWindow;
	static void shaderSettings();

	// Debug Settings
	void debugStep(Chunk* chunk, float* currentHightmap);
	void debugInit(Chunk* chunk);
	void debugUI();

	void incromentStep();
	void jumpToStep(int step);
	void jumpToEnd();

protected:
	std::unordered_map<glm::ivec2 , Chunk*> chunks;
	std::string terrainType;
	bool precessing = false;

	int chunkSize;
	int chunkResolution;
	int renderDistance;
	int seed;

	

	static bool showNewTerrain;
	static glm::vec3 posUI;
	static glm::vec3 rotUI;
	static glm::vec3 sclUI;

	static glm::vec3 grassColour;
	static glm::vec3 mountainColour;
	static float blendAmount;
	static float mountainAngle;

	// Debug Settings
	int targetStep = 1;
};