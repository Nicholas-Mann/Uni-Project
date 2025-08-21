#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include "Light.hpp"

#include "../../../Rendering/src/Render/Program.hpp"

#include "../imgui/imgui.h"	

class LightingManager
{
public:
	static bool showLightingWindow;
	static glm::vec4 clearColor;

	LightingManager();
	~LightingManager();
	static LightingManager* getInstance();

	static void RenderLighting(GLuint shader);

	static void setGlobalLightDir(glm::vec3 dir);
	static void setGlobalLightColor(glm::vec3 color);
	static void setGlobalAmbient(glm::vec3 ambient);
	static glm::vec3 getGlobalLightDir();
	static glm::vec3 getGlobalLightColor();
	static glm::vec3 getGlobalAmbient();
	static void addLight(Light light);
	static std::vector<Light> getLights();

	static void ImGuiWindow();

private:
	static LightingManager* instance;
	static glm::vec3 globalLightDir;
	static glm::vec3 globalLightColor;
	static glm::vec3 globalAmbient;
	static std::vector<Light> lights;
};

