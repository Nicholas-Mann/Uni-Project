#pragma once

#include "../../../Rendering/src/UI/InguiWidgits/ImguiCore.hpp"
#include "../Terrain.hpp"
#include "../../../Support/src/MathsFunction.hpp"
//#include "../TerrainManager.hpp"
#include <math.h>

class Perlin : public Terrain
{
public:
	Perlin(std::string name, int chunkSize, int chunkResolution, int renderDistance, int seed);
	Perlin(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int chunkSize, int chunkResolution, int renderDistance, int seed);
	
	void generateHighMap(Chunk* chunk, Terrain* terrain) override;

	static float* createPerlin(Chunk* chunk, float noiseSize, float weight, float seed);

	void ImGuiWindow() override;

	static void ImGuiWindowAdd();

private:
	static float dot(glm::vec2 gradient, glm::vec2 gridPos, float noiseSize, float seed);
	static float interpolate(float a0, float a1, float w)
	{
		return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
	}


	int noiseSize = addNoiseSize;
	float weight = addWeight;
	int octaves = addOctaves;

	static int addNoiseSize;
	static float addWeight;
	static int addOctaves;
};

