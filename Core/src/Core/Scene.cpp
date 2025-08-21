#include "Scene.hpp"

#include "../../../Core/src/Core/Camera.hpp"
#include "../../../Rendering/src/imgui/imgui.h"

class ImguiCore {
public:
	static int triangleCount;
};

bool Scene::showSceneWindow = true;
Scene* Scene::currentScene = nullptr;
std::mutex Scene::objectsMutex;

Scene::Scene(std::string name) : name(name)
{
	if (currentScene == nullptr) {
		currentScene = this;
	}
}

Scene::~Scene()
{
	objects.clear();
}

void Scene::render()
{
	ImguiCore::triangleCount = 0;

	Camera* c = Camera::get_active();
	c->set_far(c->get_render_distance());


	//for (int n = c->get_far() / c->get_render_ratio(); n > 0.1; n /= c->get_render_ratio()) {
	//	c->set_near(n);
	//	for (Object* object : objects) {
	//		object->draw();
	//	}
	//	c->set_far(n);
	//}

	c->set_near(0.1f);

	for (int i = 0; i < objects.size(); i++) {
		std::lock_guard<std::mutex> lock(objectsMutex);
		if (i < objects.size())
			objects[i]->draw();
	}
}

void Scene::add_object(Object* object)
{
	std::lock_guard<std::mutex> lock(objectsMutex);
	objects.push_back(object);
}

void Scene::remove_object(Object* object) {
	// Find the object
	std::lock_guard<std::mutex> lock(objectsMutex);
	auto it = std::find(objects.begin(), objects.end(), object);
	if (it != objects.end()) {
		objects.erase(it);
	}
}

void Scene::ImGuiWindow()
{
	ImGui::Begin("Scene", &showSceneWindow);

	if (currentScene == nullptr) {
		ImGui::Text("No scene selected");
		ImGui::End();
		return;
	}

	ImGui::Text("Scene Name: %s", currentScene->name.c_str());
	ImGui::Separator();

	// Camere Settings
	ImGui::Text("Camera Settings:");
	ImGui::Separator();
	Camera* camera = Camera::activeCamera;
	ImGui::SliderFloat("FOV", &camera->fov, 0.1f, 180.0f);
	ImGui::DragFloat("Render Distance", &camera->renderDistance, 0.1f, 1000000.0f);
	ImGui::DragFloat("Render Ratio", &camera->renderRatio, 0.1f, 1000.0f);
	ImGui::Checkbox("Movement Controls", &camera->movement_contorls);
	ImGui::Separator();
	// Position
	ImGui::Text("Position");
	ImGui::DragFloat3("Position", &camera->position[0], 0.1f);
	// Rotation
	ImGui::Text("Rotation");
	ImGui::DragFloat3("Rotation", &camera->rotation[0], 0.1f);
	// MoveSpeed
	ImGui::DragFloat("Move Speed", camera->get_movement_speed(), 0.1f);
	// Rotation Speed
	ImGui::DragFloat("Rotation Speed", camera->get_rotation_speed(), 0.1f);

	ImGui::Separator();

	// List all objects
	ImGui::Text("Objects:");
	ImGui::Separator();
	for (int i = 0; i < currentScene->objects.size(); i++) {
		std::string name = currentScene->objects[i]->get_name() + "##" + std::to_string(i);
		if (ImGui::Button(name.c_str())) {
			Object::set_current_object(currentScene->objects[i]);
		}
	}

	ImGui::Separator();

	ImGui::End();
}