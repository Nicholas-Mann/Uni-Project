#pragma once

#include "../../../Rendering/src/Objects/Object.hpp"
#include "../../../Rendering/src/Objects/Mesh.hpp"
class Terrain;

class Chunk : public Object
{
public:
	bool smoothedNorth = false;
	bool smoothedEast = false;
	bool smoothedSouth = false;
	bool smoothedWest = false;
	bool smoothedNorthEast = false;
	bool smoothedNorthWest = false;
	bool smoothedSouthEast = false;
	bool smoothedSouthWest = false;

	Chunk(glm::ivec2 chunkPos, Terrain* terrainRef, int resolution, int size);
	~Chunk();

	void updateMesh(float triangleSie, int chuckResilution);

	void createMesh();

	void draw(glm::mat4 modelPos = glm::mat4(1.0f)) override;

	void setChunkState(int state) { chunkState = state; }
	int getChunkState() { return chunkState; }
	int incrementChunkState() { return ++chunkState; }

	void setProcessing(bool state) { processing = state; }
	bool getProssesing() { return processing; }
	bool getClosing() { return closing; }

	glm::ivec2 getChunkPos() { return chunkPos; }

	void setHeightMap(float* heightMap) { this->heightMap = heightMap; }
	float* getHeightMap() { return heightMap; }
	void createHeightMap() { heightMap = new float[heightMapSize * heightMapSize]; }
	void setGenerationState(int state) { generationState = state; }

	int getResolution() { return resolution; }
	int getSize() { return size; }
	int getHeightMapSize() { return heightMapSize; }
	int getGenerationState() { return generationState; }

	// Debug Settings
	bool getStep() { return step; }
	void setStep(bool state) { step = state; }
	void setCurrentStage(int stage) { currentStage = stage; }
	int getCurrentStage() { return currentStage; }
	void setShouldEnd(bool state) { shouldEnd = state; }
	bool getShouldEnd() { return shouldEnd; }
	int getStepNumber() { return stepNumber; }
	void incrementStepNumber() { stepNumber++; }
	void setStepNumber(int number) { stepNumber = number; }

private:
	float* heightMap;
	glm::ivec2 chunkPos;
	int resolution;
	int heightMapSize;
	int size;
	Terrain* terrainRef;
	int chunkState = 0;
	int generationState = 0;
	bool processing = false;
	bool closing = false;

	// Debug Settings
	bool step = false;
	int currentStage = 0;
	int stepNumber = 0;
	bool shouldEnd = false;
};

