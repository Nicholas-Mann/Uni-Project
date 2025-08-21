#include "RenderingHelp.hpp"


std::thread::id RenderingHelp::mainThreadID = std::this_thread::get_id();

GLFWwindow* RenderingHelp::createOpenGLContext(int width, int height, const char* title, GLFWwindow* share) {
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	if (share) {
		// Ensure the shared context is valid
		if (!glfwGetWindowAttrib(share, GLFW_CONTEXT_VERSION_MAJOR)) {
			std::cerr << "Invalid shared context provided." << std::endl;
			return nullptr;
		}
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, share);
	if (!window) {
		const char* description;
		int code = glfwGetError(&description);
		std::cerr << "Failed to create GLFW window. Error code: " << code << ", Description: " << description << std::endl;
		return nullptr;
	}

	glfwHideWindow(window);

	return window;
}