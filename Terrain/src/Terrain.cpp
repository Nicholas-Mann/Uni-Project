#include "Terrain.hpp"
#include "TerrainManager.hpp"
#include "Models/Perlin.hpp"
#include "Models/HydraulicErosion.hpp"
#include "../../../Rendering/src/Objects/SimpleShapes/Pane.hpp"
#include "../../../Core/src/Core/Scene.hpp"
#include "../../../Core/src/Core/Camera.hpp"
#include "../../../Rendering/src/Render/Program.hpp"
#include <glm/glm.hpp>
#include <thread>
#include <future>
#include <mutex>

std::mutex terrainMutex;

bool Terrain::showNewTerrain = false;
glm::vec3 Terrain::posUI = glm::vec3(0.0f);
glm::vec3 Terrain::rotUI = glm::vec3(0.0f);
glm::vec3 Terrain::sclUI = glm::vec3(1.0f);

bool Terrain::showTextureWindow = true;
glm::vec3 Terrain::grassColour = glm::vec3{ 0.0f, 0.6f, 0.0f };
glm::vec3 Terrain::mountainColour = glm::vec3{ 0.5f, 0.5f, 0.5f };
float Terrain::blendAmount = 0.5f;
float Terrain::mountainAngle = 0.5f;

Terrain::Terrain(std::string name, int chunkSize, int chunkResolution, int renderDistance, int seed)
{
	TerrainManager::addTerrain(this);
	Scene::get_current_scene()->add_object(this);

	name = checkName(name);

	this->set_name(name);
	terrainType = "Flat";

	this->chunkSize = chunkSize;
	this->chunkResolution = chunkResolution + 1;
	this->renderDistance = renderDistance;
	this->seed = seed;
}

Terrain::Terrain(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int chunkSize, int chunkResolution, int renderDistance, int seed)
{
	TerrainManager::addTerrain(this);
	Scene::get_current_scene()->add_object(this);
	this->set_position(pos);
	this->set_rotation(rot);
	this->set_scale(scale);

	name = checkName(name);

	this->set_name(name);
	terrainType = "Flat";

	this->chunkSize = chunkSize;
	this->chunkResolution = chunkResolution + 1;
	this->renderDistance = renderDistance;
	this->seed = seed;
}

Terrain::~Terrain()
{

}

void Terrain::draw(glm::mat4 modelPos)
{
	if (!isVisible)
		return;

	modelPos = glm::translate(modelPos, this->position);
	modelPos = glm::rotate(modelPos, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelPos = glm::rotate(modelPos, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelPos = glm::rotate(modelPos, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelPos = glm::scale(modelPos, this->scale);

	std::lock_guard<std::mutex> lock(terrainMutex);

	GLuint shader = ShaderProgram::getShader("terrain");

	glUseProgram(shader);

	ShaderProgram::loadUniformVec3(shader, "uGrassColour", grassColour);
	ShaderProgram::loadUniformVec3(shader, "uMountainColour", mountainColour);
	ShaderProgram::loadUniformFloat(shader, "uBlendFactor", blendAmount);
	ShaderProgram::loadUniformFloat(shader, "uMountainAngle", mountainAngle);

	for (auto const& [key, val] : chunks) {
		if (val != nullptr) {
			if (val->getChunkState() == -3 && val->getProssesing() == false)
				val->draw(modelPos);
		}
	}
}

void Terrain::processTerrain()
{
	int resolution = chunkResolution;

	float triangleSize = (float)chunkSize / ((float)chunkResolution - 1);

	// check to see if any processing is needed on the chunks
	precessing = false;

	// Check that all chunks on render distance are generated
	glm::ivec2 center = glm::vec2(0, 0);
	if (TerrainManager::getUsePlayerPosForRendering()) {
		// Get the player position
		glm::vec3 playerPos = Camera::activeCamera->get_position();
		playerPos /= scale;

		// correct for terraion position
		playerPos -= this->position;

		playerPos /= chunkSize;
		center = glm::ivec2(playerPos.x, playerPos.z);
	}

	// Add new chunks to the generate queue
	for (int x = -renderDistance + 1; x < renderDistance; x++) {
		for (int z = -renderDistance + 1; z < renderDistance; z++) {
			glm::ivec2 chunkPos = glm::ivec2(x, z) + center;
			if (chunks.find(chunkPos) == chunks.end()) {
				// New chunk is needed to be generated
				std::lock_guard<std::mutex> lock(terrainMutex);
				Chunk* chunk = new Chunk(chunkPos, this, chunkResolution, chunkSize);
				chunk->set_position(glm::vec3(chunkPos.x * chunkSize, 0, chunkPos.y * chunkSize));
				chunks.insert(std::pair<glm::ivec2, Chunk*>(chunkPos, chunk));
			}
		}
	}

	// Remove chunks that are out of render distance
	std::vector<glm::ivec2> removeQueue;
	for (auto const& [key, val] : chunks) {
		if (val != nullptr) {
			if (val->getProssesing() == false) {
				if (abs(glm::ivec2(key) - glm::ivec2(center)).x > (renderDistance + 2)
					|| abs(glm::ivec2(key) - glm::ivec2(center)).y > (renderDistance + 2)) {
					// Out of render distance
					removeQueue.push_back(key);
				}
			}
		}
	}

	for (glm::ivec2 key : removeQueue) {
		std::lock_guard<std::mutex> lock(terrainMutex);
		delete(chunks[key]);
		chunks.erase(key);
	}
	removeQueue.clear();

	for (auto const& [key, val] : chunks) {
		if (val != nullptr) {
			if (val->getProssesing() == false) {
				int chunkState = val->getChunkState();
				switch (chunkState) {
				case -3:
					// Ready to render
					break;
				case -2:
					// Updating mesh
					val->updateMesh(triangleSize, resolution);
					break;
				case -1:
					// Generating Mesh
				{
					val->setProcessing(true);
					std::thread t(&Terrain::generateHighMap, this, val, this);
					t.detach();
				}
				break;
				case 0:
					// Mesh needs to be registered
					registerChunk(val);
					break;
				default:
					break;
				}
			}
			else {
				precessing = true;
			}
		}
	}
}

void Terrain::updateTerrain()
{



}

void Terrain::registerChunk(Chunk* chunk) {
	// Lock the chunk
	chunk->setProcessing(true);

	chunk->createHeightMap();

	chunk->createMesh();

	chunk->setChunkState(-1);

	// Unlock the chunk
	chunk->setProcessing(false);
}

void Terrain::generateHighMap(Chunk* chunk, Terrain* terrain) {
	// Lock the chunk
	chunk->setProcessing(true);

	float* heightMap = chunk->getHeightMap();

	int resolution = chunk->getHeightMapSize();

	for (int x = 0; x < resolution; x++) {
		for (int z = 0; z < resolution; z++) {
			heightMap[x * resolution + z] = 0.0f;
		}
	}

	//chunk->setHeightMap(heightMap);

	// Set the next state of the chunk
	chunk->setChunkState(-2);

	// Unlock the chunk
	chunk->setProcessing(false);
}

void Terrain::regenerateTerrain()
{
	// Update settings
	chunkSize = TerrainManager::getChunkSize();
	chunkResolution = TerrainManager::getChunkResolution() + 1;
	renderDistance = TerrainManager::getRenderDistance();
	seed = TerrainManager::getSeed();

	for (auto const& [key, val] : chunks) {
		val->setChunkState(-1);
	}
}

void Terrain::updateTexture()
{
	float triangleSize = (float)chunkSize / ((float)chunkResolution - 1);

	int resolution = chunkResolution;

	for (auto const& [key, val] : chunks) {
		val->updateMesh(triangleSize, resolution);
	}
}

void Terrain::incromentStep()
{
	targetStep++;
	for (auto const& [key, val] : chunks) {
		val->setStep(false);
	}
}

void Terrain::jumpToStep(int step)
{
	targetStep = step;
	for (auto const& [key, val] : chunks) {
		val->setStep(false);
		val->setShouldEnd(false);
	}
}

void Terrain::jumpToEnd()
{
	for (auto const& [key, val] : chunks) {
		val->setStep(false);
		val->setShouldEnd(true);
	}
}

void Terrain::ImGuiWindow()
{
	ImGui::Begin("Object");

	ImGuiTerrainDefault();

	ImGui::End();
}

void Terrain::ImGuiTerrainDefault() {
	ImGui::Text("Terrain");
	ImGui::Separator();

	char buffer[256];
	strcpy_s(buffer, this->name.c_str());
	ImGui::InputText("Name", buffer, 256);
	this->name = buffer;

	// Prosseing
	ImGui::Separator();
	ImGui::Text("Processing: %s", precessing ? "True" : "False");
	ImGui::Separator();

	// Terrain Type
	ImGui::Text("Terrain Type: %s", terrainType.c_str());
	ImGui::Separator();

	ImGui::Text("Object Name: %s", this->name.c_str());
	ImGui::Separator();

	// Visibility
	ImGui::Text("Visibility");
	ImGui::Checkbox("Visible", &this->isVisible);
	// Position
	ImGui::Text("Position");
	ImGui::DragFloat3("Position", &this->position[0], 0.1f);
	// Rotation
	ImGui::Text("Rotation");
	ImGui::DragFloat3("Rotation", &this->rotation[0], 0.1f);
	// Scale
	ImGui::Text("Scale");
	ImGui::DragFloat3("Scale", &this->scale[0], 0.1f);

	// Number of active chunks
	ImGui::Text("Number of Chunks: %d", chunks.size());

	// Regenerate Terrain
	if (ImGui::Button("Regenerate Terrain")) {
		regenerateTerrain();
	}

	if (ImGui::Button("Update Texture")) {
		updateTexture();
	}

	ImGui::Separator();

	// Remove the object
	if (ImGui::Button("Remove Object")) {
		Scene::get_current_scene()->remove_object(currentObject);
		TerrainManager::removeTerrain(this);
		currentObject = nullptr;
	}

	debugUI();
}

Terrain* Terrain::ImGuiWindowAdd()
{
	if (!showNewTerrain) {
		return nullptr;
	}

	static int generationType = 0;
	static char name[256] = "Terrain";

	ImGui::Begin("Add Terrain");
	Terrain* terrain = nullptr;

	// Terrain Name
	ImGui::InputText("Name", name, 256);

	// Starting Position
	ImGui::DragFloat3("Position", &posUI[0], 0.1f);

	// Rotation
	ImGui::DragFloat3("Rotation", &rotUI[0], 0.1f);

	// Scale
	ImGui::DragFloat3("Scale", &sclUI[0], 0.1f);

	// Generation Type
	ImGui::RadioButton("Flat", &generationType, 0);
	ImGui::RadioButton("Perlin", &generationType, 1);
	ImGui::RadioButton("Hydraulic Erosion", &generationType, 2);

	switch (generationType)
	{
	case 0:
		break;
	case 1:
		Perlin::ImGuiWindowAdd();
		break;
	case 2:
		HydraulicErosion::ImGuiWindowAdd();
		break;
	default:
		break;
	}

	if (ImGui::Button("Add Terrain")) {
		// Add the terrain
		switch (generationType)
		{
		case 0:
			terrain = new Terrain(name, posUI, rotUI, sclUI, TerrainManager::getChunkSize(), TerrainManager::getChunkResolution(), TerrainManager::getRenderDistance(), TerrainManager::getSeed());
			break;
		case 1:
			terrain = (Terrain*) new Perlin(name, posUI, rotUI, sclUI, TerrainManager::getChunkSize(), TerrainManager::getChunkResolution(), TerrainManager::getRenderDistance(), TerrainManager::getSeed());
			break;
		case 2:
			terrain = (Terrain*) new HydraulicErosion(name, posUI, rotUI, sclUI, TerrainManager::getChunkSize(), TerrainManager::getChunkResolution(), TerrainManager::getRenderDistance(), TerrainManager::getSeed());
			break;
		default:
			terrain = new Terrain(name, posUI, rotUI, sclUI, TerrainManager::getChunkSize(), TerrainManager::getChunkResolution(), TerrainManager::getRenderDistance(), TerrainManager::getSeed());
			break;
		}

		showNewTerrain = false;
	}
	ImGui::End();

	return terrain;
}

void Terrain::debugUI()
{
	// Debug Settings
	if (TerrainManager::getDebug() == true) {
		ImGui::Separator();
		ImGui::Text("Debug Settings");
		ImGui::Separator();
		if (ImGui::Button("Incroment Step")) {
			incromentStep();
		}

		// Current Step
		ImGui::Text("Current Step: %d", targetStep);

		// Jump to step
		ImGui::InputInt("Jump to Step Value", &targetStep);
		if (ImGui::Button("Jump to Step")) {
			jumpToStep(targetStep);
		}

		if (ImGui::Button("Jump to End")) {
			jumpToEnd();
		}

	}
}

void Terrain::debugInit(Chunk* chunk)
{
	chunk->setStep(false);
	chunk->setShouldEnd(false);
}

void Terrain::debugStep(Chunk* chunk, float* currentHightmap)
{
	if (TerrainManager::getDebug() && !chunk->getShouldEnd() && targetStep - 1 <= chunk->getStepNumber()) {
		chunk->setStep(true);
		chunk->setCurrentStage(chunk->getChunkState());

		// Runder the current hight map
		//chunk->setHeightMap(currentHightmap);

		// Give time for the hightmap to copy
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Unlock the chunk
		chunk->setProcessing(false);

		// Set the next state of the chunk
		chunk->setChunkState(-2);

		// What for the user to press the button
		while (chunk->getStep() == true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		// Lock the chunk
		chunk->setProcessing(true);

		// Return to the current stage
		chunk->setChunkState(chunk->getCurrentStage());

		// Reset the step
		chunk->setStep(false);
	}
	chunk->incrementStepNumber();
}

std::string Terrain::checkName(std::string desiredName)
{
	Scene* scene = Scene::get_current_scene();
	std::string name = desiredName;

	int instanceCount = 0;

	while (scene->get_object(name) != nullptr) {
		instanceCount++;
		name = desiredName + "[" + std::to_string(instanceCount) + "]";
	}

	return name;
}

void Terrain::shaderSettings()
{
	ImGui::Begin("Shader Settings", &showTextureWindow);
	// Grass Colour
	ImGui::Text("Grass Colour");
	ImGui::ColorEdit3("Grass Colour", &grassColour[0]);
	// Mountain Colour
	ImGui::Text("Mountain Colour");
	ImGui::ColorEdit3("Mountain Colour", &mountainColour[0]);

	// Blend Amount
	ImGui::Text("Blend Amount");
	ImGui::SliderFloat("Blend Amount", &blendAmount, 0.0f, 0.5f);

	// Mountain Angle
	ImGui::Text("Mountain Angle");
	ImGui::SliderFloat("Mountain Angle", &mountainAngle, 0.0f, 1.0f);


	if (ImGui::Button("Close")) {
		showTextureWindow = false;
	}

	ImGui::End();
}