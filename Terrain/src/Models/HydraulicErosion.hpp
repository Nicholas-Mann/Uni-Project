#pragma once

#include "../Terrain.hpp"
#include <glm/gtx/hash.hpp>


class HydraulicErosion : public Terrain
{
public:
	HydraulicErosion(std::string name, int chunkSize, int chunkResolution, int renderDistance, int seed);
	HydraulicErosion(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int chunkSize, int chunkResolution, int renderDistance, int seed);
	
	void generateHighMap(Chunk* chunk, Terrain* terrain) override;

	glm::vec2 getHighestNeighbour(float* heightMap, int x, int z, int resolution, float height);
	glm::vec2 getLowestNeighbour(float* heightMap, int x, int z, int resolution, float height);

	void ImGuiWindow() override;

	static void ImGuiWindowAdd();

private:
	float dot(glm::vec2 gradient, glm::vec2 gridPos);
	float interpolate(float a0, float a1, float w)
	{
		return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
	}


	int noiseSize = addNoiseSize;
	float weight = addWeight;
	int octaves = addOctaves;

	static int addNoiseSize;
	static float addWeight;
	static int addOctaves;

	int rainFactor = addRainFactor;
	float erosionFactor = addErosionFactor;
	int hydraulicIterations = addHydraulicIterations;
	float sedimentCarryFactor = addSedimentCarryFactor;
	float maxSlope = addMaxSlope;
	int rainSmoothing = addRainSmoothing;

	static int addRainFactor;
	static float addErosionFactor;
	static int addHydraulicIterations;
	static float addSedimentCarryFactor;
	static float addMaxSlope;
	static int addRainSmoothing;

	bool edgeSmoothingEnabled = addEdgeSmoothingEnabled;
	bool edgeBlending = addEdgeBlending;
	int edgeSmoothingSize = addEdgeSmoothingSize;
	int edgeSmoothingIterations = addEdgeSmoothingIterations;

	static bool addEdgeSmoothingEnabled;
	static bool addEdgeBlending;
	static int addEdgeSmoothingSize;
	static int addEdgeSmoothingIterations;

	int padding = 1;
	
	// Thermal Erosion
	//int theremalIterations = 100;
	//float thermalErosionFactor = 0.5f;
	//float thermalThresholdAngle = 0.5f;

	// Hydraulic Erosion
	//float waterConstat = 0.01f;
	//float sedimentConstat = 0.01f;
	//float evaporation = 0.05f;

};

