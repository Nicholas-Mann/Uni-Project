#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera
{
public:
	static Camera* activeCamera;

	float fov;

	glm::vec3 position;
	glm::vec3 rotation;
	bool movement_contorls = true;
	float renderDistance = 10000.f;
	float renderRatio = 1000.f;

	Camera(float fov, float near, float far, glm::vec3 startPos, bool movement_contorls);

	int get_render_distance() {
		return this->renderDistance;
	}
	void set_render_distance(int renderDistance) {
		this->renderDistance = renderDistance;
	}
	float get_render_ratio() {
		return this->renderRatio;
	}
	void set_render_ratio(float renderRatio) {
		this->renderRatio = renderRatio;
	}

	void set_active() {
		this->activeCamera = this;
	}

	static Camera* get_active() {
		return activeCamera;
	}

	float* get_movement_speed() {
		return &this->moveSpeed;
	}
	float* get_rotation_speed() {
		return &this->rotateSpeed;
	}
	void set_fov(float fov) {
		this->fov = fov;
	}
	void set_window_size(float width, float height) {
		this->width = width;
		this->height = height;
	}
	void set_near(float near) {
		this->near = near;
	}
	void set_far(float far) {
		this->far = far;
	}
	void set_position(glm::vec3 position) {
		this->position = position;
	}
	void set_rotation(glm::vec3 rotation) {
		this->rotation = rotation;
	}
	void set_movement_controls(bool movement_controls) {
		this->movement_contorls = movement_controls;
	}
	float get_fov() {
		return this->fov;
	}
	float get_near() {
		return this->near;
	}
	float get_far() {
		return this->far;
	}
	glm::vec3& get_position() {
		return this->position;
	}
	glm::vec3 get_rotation() {
		return this->rotation;
	}
	bool get_movement_controls() {
		return this->movement_contorls;
	}
	glm::mat4 get_projection() {
		glm::mat4 projection = glm::perspective(glm::radians(fov), width / height, this->near, this->far);
		return projection;
	}

	glm::mat4 get_view() {
		// Camera forward vector
		glm::vec3 forward;
		forward.x = cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
		forward.y = -sin(glm::radians(rotation.x));
		forward.z = -cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
		forward = glm::normalize(forward);

		// Camera up vector
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		// Use glm::lookAt for a correct view matrix
		glm::mat4 view = glm::lookAt(
			this->position,           // Camera position
			this->position + forward, // Target position
			up                          // Up direction
		);

		return view;
	}

	void update(float detltaTime, GLFWwindow* window);

private:
	float width = 10;
	float height = 10;

	float moveSpeed = 60.0f;
	float rotateSpeed = 20 * 10.0f;

	float near;
	float far;
};

#endif // CAMERA_HPP
