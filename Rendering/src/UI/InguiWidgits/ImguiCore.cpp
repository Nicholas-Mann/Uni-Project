#include "ImguiCore.hpp"
#include "../../../Core/src/Core/Scene.hpp"
#include "../../Objects/Object.hpp"
#include "../../../Terrain/src/TerrainManager.hpp"
#include "../../imgui/imgui_impl_opengl3.h"
#include "../../imgui/imgui_internal.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../../imgui/imgui.h"
#include <iostream>
//#include <GLFW/glfw3.h>
#include "../../Lighting/LightingManager.hpp"

// Define static member variables
bool ImguiCore::isVisable = true;
bool ImguiCore::seeThroughDockSpace = false;
bool ImguiCore::showDemoWindow = false;
bool ImguiCore::showImguiSettings = false;
bool ImguiCore::showOpenGLSettings = false;
bool ImguiCore::showProformance = false;
int ImguiCore::triangleCount = 0;
ScreenRenderWidgit* ImguiCore::screenRenderWidgit = nullptr;


ImguiCore::~ImguiCore() {
	destroy();
}

void ImguiCore::init(GLFWwindow* window) {
	// imgui
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		  // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 430";
	ImGui_ImplOpenGL3_Init(glsl_version);

	screenRenderWidgit = new ScreenRenderWidgit(800, 600);
}

void ImguiCore::render() {
	ImGuiIO& io = ImGui::GetIO();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Toggle visability
	if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
		isVisable = !isVisable;
	}

	// Back out if not visable
	if (!isVisable) {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		return;
	}

	// Dockspace
	if (seeThroughDockSpace) {
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	}
	else {
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
	}

	// Menu bar
	menuBar();
	
	// Show demo window
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	// Show imgui settings
	if (showImguiSettings)
		imguiSettingsWiondow();

	// Show openGL settings
	if (showOpenGLSettings)
		openGLSettingsWindow();

	// Show proformance
	if (showProformance)
		proformanceWindow();

	// Draw the scene
	if (Scene::showSceneWindow)
		Scene::ImGuiWindow();

	// Terrain Manager
	if (TerrainManager::showTerrainWindow)
		TerrainManager::ImGuiWindow();

	// Draw the object
	if (Object::showObjectWindow) {
		if (Object::get_current_object() != nullptr) {
			Object::get_current_object()->ImGuiWindow();
		}
		else {
			ImGui::Begin("Object");
			ImGui::Text("No Object Selected");
			ImGui::End();
		}
	}

	// Draw the lighting
	if (LightingManager::showLightingWindow) {
		LightingManager::ImGuiWindow();
	}

	// Draw the screen render widgit
	if (ScreenRenderWidgit::showScreenRenderWidgit) {
		screenRenderWidgit->updateWindow();
	}

	// Terrain Texture Window
	if (Terrain::showTextureWindow) {
		Terrain::shaderSettings();
	}


	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ImguiCore::menuBar() {
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::MenuItem("Debug Window", NULL, &showDemoWindow);
			ImGui::MenuItem("Imgui Settings", NULL, &showImguiSettings);
			ImGui::MenuItem("OpenGL Settings", NULL, &showOpenGLSettings);
			ImGui::MenuItem("Proformance", NULL, &showProformance);
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Scene Viewer", NULL, &Scene::showSceneWindow);
			ImGui::MenuItem("Object Inspector", NULL, &Object::showObjectWindow);
			ImGui::MenuItem("Terrain Shader", NULL, &Terrain::showTextureWindow);
			ImGui::MenuItem("Lighting Manager", NULL, &LightingManager::showLightingWindow);
			ImGui::MenuItem("Screen Render Widgit", NULL, &ScreenRenderWidgit::showScreenRenderWidgit);
			ImGui::MenuItem("Terrain Manager", NULL, &TerrainManager::showTerrainWindow);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ImguiCore::imguiSettingsWiondow() {
	ImGui::Begin("Imgui Settings", &showImguiSettings);
	ImGui::Checkbox("See Through Dock Space", &seeThroughDockSpace);
	ImGui::End();
}

void ImguiCore::openGLSettingsWindow() {
	ImGui::Begin("OpenGL Settings", &showOpenGLSettings);

	ImGui::Text("RENDERER %s", glGetString(GL_RENDERER));
	ImGui::Text("VENDOR %s", glGetString(GL_VENDOR));
	ImGui::Text("VERSION %s", glGetString(GL_VERSION));
	ImGui::Text("SHADING_LANGUAGE_VERSION %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	ImGui::Separator();
	
	// Setting OpenGL veribles
	// Cull face
	bool cullFace = glIsEnabled(GL_CULL_FACE);
	ImGui::Checkbox("GL_CULL_FACE", &cullFace);
	if (cullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	// Polygon mode
	int polygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
	ImGui::Text("Polygon Mode");
	ImGui::RadioButton("GL_POINT", &polygonMode, GL_POINT);
	ImGui::RadioButton("GL_LINE", &polygonMode, GL_LINE);
	ImGui::RadioButton("GL_FILL", &polygonMode, GL_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	// Shader Info
	ShaderProgram::ImGuiWindowInfo();

	ImGui::End();
}

void ImguiCore::proformanceWindow() {
	ImGui::Begin("Proformance", &showProformance);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	// Frame rate graph
	static float values[90] = { 0 };
	static int values_offset = 0;
	static double refresh_time = 0.0;
	if (refresh_time == 0.0)
		refresh_time = ImGui::GetTime();

	while (refresh_time < ImGui::GetTime()) {
		static float phase = 0.0f;
		values[values_offset] = ImGui::GetIO().Framerate;
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		phase += 0.10f * values_offset;
		refresh_time += 1.0 / 30.0;
	}

	ImGui::PlotLines("Frame Times", values, IM_ARRAYSIZE(values), values_offset, "Frame Times", 0.0f, 100.0f, ImVec2(0, 80));

	ImGui::Separator();

	// Triangle count
	ImGui::Text("Triangle Count: %d", triangleCount);

	ImGui::End();
}

void ImguiCore::destroy() {
	// Cleanup.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}