#include "Camera.hpp"
#include <numbers>
#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "../../../Rendering/src/imgui/imgui.h"

Camera* Camera::activeCamera = nullptr;

Camera::Camera(float fov, float near, float far, glm::vec3 pos, bool movement_contorls)
    : fov(fov), near(near), far(far), position(pos), rotation(0.f, 45.f, 0.f), movement_contorls(movement_contorls), width(10), height(10) {
    if (activeCamera == nullptr) {
        this->set_active();
    }
}

void Camera::update(float detltaTime, GLFWwindow* window) {
	if (!this->movement_contorls) {
		return;
	}

	float speed = moveSpeed * detltaTime;

	// Calculate falword Vectors
	glm::vec3 forward = { 0, 0, 0 };
	forward.x = std::cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
	forward.y = -std::sin(glm::radians(rotation.x));
	forward.z = -std::cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));

	// remove y from forward vector
	forward.y = 0;

	forward = normalize(forward);

	// Calculate right Vectors
	glm::vec3 right = cross(forward, glm::vec3{ 0.f,1.f,0.f });
	right = normalize(right);
	glm::vec3 up = cross(right, forward);
	up = normalize(up);

	// is shift pressed? Yes speed up
	ImGui::IsKeyDown(ImGuiKey_LeftShift);

	if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
		speed = speed * 2;
	}
	// is control pressed? Yes slow down
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
		speed = speed * 0.5;
	}
	// is W pressed? Yes move forward
	if (ImGui::IsKeyDown(ImGuiKey_W)) {
		position += forward * speed;
	}
	// is S pressed? Yes move backward
	if (ImGui::IsKeyDown(ImGuiKey_S)) {
		position -= forward * speed;
	}
	// is A pressed? Yes move left
	if (ImGui::IsKeyDown(ImGuiKey_A)) {
		position -= right * speed;
	}
	// is D pressed? Yes move right
	if (ImGui::IsKeyDown(ImGuiKey_D)) {
		position += right * speed;
	}
	// is Q pressed? Yes move up
	if (ImGui::IsKeyDown(ImGuiKey_Q)) {
		position += glm::vec3(0.f, 1.f, 0.f) * speed;
	}
	// is E pressed? Yes move down
	if (ImGui::IsKeyDown(ImGuiKey_E)) {
		position -= glm::vec3(0.f, 1.f, 0.f) * speed;
	}

	speed = rotateSpeed * detltaTime;
	if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
		rotation.x += speed;
	}

	if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
		rotation.x -= speed;
	}

	if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
		rotation.y -= speed;
	}

	if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
		rotation.y += speed;
	}

	if (rotation.x > 89.0f) rotation.x = 89.0f;
	if (rotation.x < -89.0f) rotation.x = -89.0f;
}