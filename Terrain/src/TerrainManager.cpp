#include "TerrainManager.hpp"
#include <iostream>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Terrain.hpp"
#include "../../../Core/src/Core/Camera.hpp"
#include "../../../Rendering/src/imgui/imgui.h"

bool TerrainManager::showTerrainWindow = true;
bool TerrainManager::usePlayerPosForRendering = false;
int TerrainManager::chunkSize = 40;
int TerrainManager::chunkResolution = 200;
int TerrainManager::renderDistance = 3;
int TerrainManager::seed = 0;
bool TerrainManager::running = false;
bool TerrainManager::debug = false;
std::thread TerrainManager::updateThread;

std::vector<Terrain*> TerrainManager::terrains;

TerrainManager::~TerrainManager()
{
	terrains.clear();
}

void TerrainManager::init()
{
	running = true;
	updateThread = std::thread(&processTerrain);
	updateThread.detach();
}

void TerrainManager::destroy()
{
	running = false;
	updateThread.join();
}

void TerrainManager::processTerrain()
{
	while (running)
	{
		for (int i = 0; i < terrains.size(); i++) {
			terrains[i]->processTerrain();
		}
	}
}

void TerrainManager::updateTerrain()
{
	for (Terrain* terrain : terrains) {
		terrain->updateTerrain();
	}
}

void TerrainManager::regenerateTerrain()
{
	for (Terrain* terrain : terrains) {
		terrain->regenerateTerrain();
	}
}

void TerrainManager::ImGuiWindow()
{
	ImGui::Begin("Terrain Manager", &showTerrainWindow);

	// Toggle Debug
	ImGui::Checkbox("Debug", &debug);

	ImGui::InputInt("Seed", &seed);
	ImGui::InputInt("Chunk Size", &chunkSize);
	ImGui::InputInt("Chunk Resolution", &chunkResolution);
	ImGui::InputInt("Render Distance", &renderDistance);

	ImGui::Checkbox("Use Player Pos For Rendering", &usePlayerPosForRendering);

	ImGui::Separator();

	// Player position in chunks
	glm::vec3 playerPos = Camera::activeCamera->get_position();
	playerPos /= chunkSize;
	glm::ivec3 playerChunkPos = glm::ivec3(playerPos.x, playerPos.y, playerPos.z);
	ImGui::Text("Player Position: %d, %d", playerChunkPos.x, playerChunkPos.z);


	// Add terrain
	if (ImGui::Button("Add Terrain")) {
		Terrain::showNewTerrainWindow();
	}
	Terrain::ImGuiWindowAdd();

	// Regenerate Terrain
	if (ImGui::Button("Regenerate Terrain")) {
		regenerateTerrain();
	}

	// List all terrains
	ImGui::Text("Terrains:");
	ImGui::Separator();
	for (Terrain* terrain : terrains) {
		if (ImGui::Button(terrain->get_name().c_str())) {
			Object::set_current_object(terrain);
		}
	}

	ImGui::End();
}

