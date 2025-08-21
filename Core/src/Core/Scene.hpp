#pragma once

#include <string>
#include <vector>
#include "../../../Rendering/src/Objects/Object.hpp"
#include <mutex>

class Scene
{
public:
	static bool showSceneWindow;
	static std::mutex objectsMutex;

	Scene(std::string name);
	~Scene();
	void render();
	void add_object(Object* object);
	void remove_object(Object* object);
	void set_name(std::string name) { this->name = name; }
	std::vector<Object*> get_ojects() { return objects; }
	Object* get_object(std::string name) {
		// Lock
		std::lock_guard<std::mutex> lock(objectsMutex);
		for (Object* object : objects) {
			if (object->get_name() == name) {
				return object;
			}
		}
		return nullptr;
	}

	static Scene* get_current_scene() { return currentScene; }
	static void set_current_scene(Scene* scene) { currentScene = scene; }
	void set_current_scene() { currentScene = this; }
	static void render_current_scene() { currentScene->render(); }

	static void ImGuiWindow();


private:
	std::string name;
	std::vector<Object*> objects;

	static Scene* currentScene;
};

