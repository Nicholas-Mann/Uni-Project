#pragma once


#include "../Objects/Mesh.hpp"
#include <vector>
#include "../Render/Material.hpp"
#include <string>
#include <memory>


class Object
{
public:
	static bool showObjectWindow;

	Object();
	Object(std::string name);
	Object(std::string name, Mesh mesh);
	Object(std::string name, Mesh mesh, glm::vec3 pos, glm::vec3 rot);
	Object(std::string name, Mesh mesh, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	~Object();
	void setMesh(Mesh mesh) { this->mesh = mesh; }
	Mesh getMesh() { return this->mesh; }
	void add_Object(std::shared_ptr<Object> object);
	void remove();
	void set_position(glm::vec3 position) {
		this->position = position;
	}
	void set_rotation(glm::vec3 rotation) {
		this->rotation = rotation;
	}
	void set_scale(glm::vec3 scale) {
		this->scale = scale;
	}
	void set_material(Material* material) {
		this->material = material;
	}
	glm::vec3 get_position() {
		return this->position;
	}
	glm::vec3 get_rotation() {
		return this->rotation;
	}
	glm::vec3 get_scale() {
		return this->scale;
	}
	Material* get_material() {
		return this->material;
	}
	std::string get_name() {
		std::string name = this->name;
		if (name == "") {
			name = "Object";
		}
		return name;
	}
	void set_name(std::string name) {
		this->name = name;
	}
	virtual void draw(glm::mat4 modelPos = glm::mat4(1.0f));
	void set_visible(bool visible) {
		this->isVisible = visible;
	}
	bool get_visible() {
		return this->isVisible;
	}

	static Object* get_current_object() { return currentObject; }
	static void set_current_object(Object* object) { currentObject = object; }
	virtual void ImGuiWindow();

protected:
	std::string name = "Object";
	glm::vec3 position = glm::vec3{ 0.f, 0.f, 0.f };
	glm::vec3 rotation = glm::vec3{ 0.f, 0.f, 0.f };
	glm::vec3 scale = glm::vec3{ 1.f, 1.f, 1.f };
	Mesh mesh;
	bool hasMesh = true;
	bool isVisible = true;
	std::vector<std::shared_ptr<Object>> objects;
	Material defaultMaterial = Material(0.1f, 0.9f, 0.9f, 0.9f, glm::vec3{ 0.f, 0.f, 0.f });
	Material* material = &defaultMaterial;

	static Object* currentObject;
};