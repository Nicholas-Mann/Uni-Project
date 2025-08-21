#include "Object.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../imgui/imgui.h"
#include "../../../Core/src/Core/Camera.hpp"
#include <iostream>

class Scene {
public :
	static Scene* get_current_scene();
	void remove_object(Object* object);
};

bool Object::showObjectWindow = true;
Object* Object::currentObject = nullptr;

Object::Object() {
	hasMesh = false;
}

Object::Object(std::string name) {
	this->hasMesh = false;
	this->name = name;
}

Object::Object(std::string name, Mesh mesh) {
	this->mesh = mesh;
	this->name = name;
}

Object::Object(std::string name, Mesh mesh, glm::vec3 pos, glm::vec3 rot) {
	this->mesh = mesh;
	this->position = pos;
	this->rotation = rot;
	this->name = name;
}

Object::Object(std::string name, Mesh mesh, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
	this->mesh = mesh;
	this->position = pos;
	this->rotation = rot;
	this->scale = scale;
	this->name = name;
}

Object::~Object() {
	remove();
}

void Object::remove() {
	for (std::shared_ptr<Object> object : objects) {
		object->remove();
	}

	this->objects.clear();
	this->mesh.cleanUp();
}


void Object::add_Object(std::shared_ptr<Object> object) {
	this->objects.push_back(object);
}

void Object::draw(glm::mat4 modelPos) {

	if (!isVisible) {
		return;
	}

	// Calcualte the model matrix
	modelPos = glm::translate(modelPos, position);
	modelPos = glm::rotate(modelPos, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelPos = glm::rotate(modelPos, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelPos = glm::rotate(modelPos, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelPos = glm::scale(modelPos, scale);

	if (hasMesh) {
		// Draw the mesh
		mesh.drawMesh(modelPos, material);
	}

	// Draw all the objects
	for (std::shared_ptr<Object> object : objects) {
		if (object != NULL)
			object->draw(modelPos);
		else
			std::cout << "Rendering: Object is null" << std::endl;
	}
}

void Object::ImGuiWindow() {
	ImGui::Begin("Object");

	if (currentObject == nullptr) {
		ImGui::Text("No Object Selected");
		ImGui::End();
		return;
	}

	//if (ImGui::Button("Delete Object")) {
	//	Scene::get_current_scene()->remove_object(currentObject);
	//	currentObject = nullptr;
	//}

	char buffer[256];
	strcpy_s(buffer, currentObject->name.c_str());	
	ImGui::InputText("Name", buffer, 256);
	currentObject->name = buffer;


	ImGui::Text("Object Name: %s", currentObject->name.c_str());
	ImGui::Separator();

	// Visibility
	ImGui::Text("Visibility");
	ImGui::Checkbox("Visible", &this->isVisible);
	// Position
	ImGui::Text("Position");
	ImGui::DragFloat3("Position", &currentObject->position[0], 0.1f);
	// Rotation
	ImGui::Text("Rotation");
	ImGui::DragFloat3("Rotation", &currentObject->rotation[0], 0.1f);
	// Scale
	ImGui::Text("Scale");
	ImGui::DragFloat3("Scale", &currentObject->scale[0], 0.1f);


	// Remove the object
	if (ImGui::Button("Remove Object")) {
		Scene::get_current_scene()->remove_object(currentObject);
		currentObject = nullptr;
	}

	// Go To
	if (ImGui::Button("Go To")) {
		Camera::activeCamera->set_position(currentObject->position);
	}

	ImGui::End();
}