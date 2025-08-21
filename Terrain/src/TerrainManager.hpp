#pragma once

#include "Terrain.hpp"
#include <thread>

class TerrainManager
{
public:
	static bool showTerrainWindow;

	~TerrainManager();

	static void init();
	static void destroy();

	static void processTerrain();
	static void updateTerrain();
	static void regenerateTerrain();

	static void setSeed(int seed) { TerrainManager::seed = seed; }
	static void setChunkSize(int size) { chunkSize = size; }
	static void setChunkResolution(int resolution) { chunkResolution = resolution; }
	static void setRenderDistance(int distance) { renderDistance = distance; }

	static bool getDebug() { return debug; }

	static int getSeed() { return seed; }
	static int getChunkSize() { return chunkSize; }
	static int getChunkResolution() { return chunkResolution; }
	static int getRenderDistance() { return renderDistance; }

	static void addTerrain(Terrain* terrain) { terrains.push_back(terrain); }
	static void removeTerrain(Terrain* terrain) {
		auto it = std::find(terrains.begin(), terrains.end(), terrain);
		if (it != terrains.end()) {
			terrains.erase(it);
		}
	}

	static bool getUsePlayerPosForRendering() { return usePlayerPosForRendering; }

	static void ImGuiWindow();

private:
	static std::vector<Terrain*> terrains;
	static int chunkSize;
	static int chunkResolution;
	static int renderDistance;
	static int seed;
	static bool usePlayerPosForRendering;
	static bool running;
	static std::thread updateThread;

	static bool debug;
};

