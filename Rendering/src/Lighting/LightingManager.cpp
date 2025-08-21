#include "LightingManager.hpp"

LightingManager* LightingManager::instance = nullptr;
glm::vec3 LightingManager::globalLightDir = glm::vec3(-1.f, 1.f, 0.5f);
glm::vec3 LightingManager::globalLightColor = glm::vec3(0.9f, 0.9f, 0.6f);
glm::vec3 LightingManager::globalAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
std::vector<Light> LightingManager::lights;
bool LightingManager::showLightingWindow = false;
glm::vec4 LightingManager::clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);

LightingManager::LightingManager()
{
}

LightingManager::~LightingManager()
{
}

LightingManager* LightingManager::getInstance()
{
	if (instance == nullptr) {
		instance = new LightingManager();
	}
	return instance;
}

void LightingManager::RenderLighting(GLuint shader)
{
	ShaderProgram::loadUniformVec3(shader, "uGlobalLightDir", globalLightDir);
	ShaderProgram::loadUniformVec3(shader, "uLightDiffuse", globalLightColor);
	ShaderProgram::loadUniformVec3(shader, "uSceneAmbient", globalAmbient);

	for (int i = 0; i < lights.size() && i < 50; i++) {
		lights[i].renderLight(shader, i);
	}

	if (lights.size() > 50) {
		std::cout << "Warning: Only 50 lights can be rendered" << std::endl;
		ShaderProgram::loadUniformInt(shader, "uNumLights", 50);
	}
	else {
		ShaderProgram::loadUniformInt(shader, "uNumLights", lights.size());
	}

}

void LightingManager::setGlobalLightDir(glm::vec3 dir)
{
	globalLightDir = dir;
}

void LightingManager::setGlobalLightColor(glm::vec3 color)
{
	globalLightColor = color;
}

void LightingManager::setGlobalAmbient(glm::vec3 ambient)
{
	globalAmbient = ambient;
}

glm::vec3 LightingManager::getGlobalLightDir()
{
	return globalLightDir;
}

glm::vec3 LightingManager::getGlobalLightColor()
{
	return globalLightColor;
}

glm::vec3 LightingManager::getGlobalAmbient()
{
	return globalAmbient;
}

void LightingManager::addLight(Light light)
{
	lights.push_back(light);
}

std::vector<Light> LightingManager::getLights()
{
	return lights;
}

void LightingManager::ImGuiWindow()
{
	ImGui::Begin("Lighting");
	ImGui::Text("Global Light Settings");
	ImGui::Separator();
	ImGui::SliderFloat3("Light Direction", glm::value_ptr(globalLightDir), -1, 1);
	ImGui::ColorEdit3("Light Color", glm::value_ptr(globalLightColor));
	ImGui::ColorEdit3("Ambient Color", glm::value_ptr(globalAmbient));
	ImGui::ColorEdit4("Clear Color", glm::value_ptr(clearColor));
	ImGui::Separator();
	ImGui::Text("Lights");
	ImGui::Separator();

	for (int i = 0; i < lights.size(); i++) {
		lights[i].ImGuiWindowInfo(i);
	}

	ImGui::End();
}
