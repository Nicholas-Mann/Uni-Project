#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "../../../Rendering/src/Objects/Object.hpp"
#include <numbers>
#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>

#include "Scene.hpp"

#include "../../../Rendering/src/Render/Program.hpp"

#include "../../../Rendering/src/imgui/imgui.h"
#include "../../../Rendering/src/UI/InguiWidgits/ImguiCore.hpp"
#include "../../../Rendering/src/Render/RenderingHelp.hpp"
#include "../../../Terrain/src/Terrain.hpp"
#include "../../../Terrain/src/TerrainManager.hpp"
#include "../../../Rendering/src/Lighting/LightingManager.hpp"
#include "../../../Rendering/src/Lighting/Light.hpp"
#include "../../../Rendering/src/Objects/SimpleShapes/Cube.hpp"
#include "../../../Core/src/Core/Camera.hpp"

namespace
{
	constexpr char const* kWindowTitle = "Indervidual Project";

	void setupDebugCallback();

	void APIENTRY openglCallbackFunction(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);

	void glfw_callback_error_(int, char const*);

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

int main() try
{
	// Initialize GLFW
	if (GLFW_TRUE != glfwInit())
	{
		char const* msg = nullptr;
		int ecode = glfwGetError(&msg);
		// Print error
		std::cout << "GLFW error: " << msg << " (" << ecode << ")" << std::endl;
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback(&glfw_callback_error_);

	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#	endif // ~ !NDEBUG

	GLFWwindow* window = RenderingHelp::createOpenGLContext(800, 600, kWindowTitle);

	if (!window)
	{
		char const* msg = nullptr;
		int ecode = glfwGetError(&msg);
	}

	// Set up drawing stuff
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-Sync is on.

	// Initialize GLAD
// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
		printf("ERROR");

	//Setting OpenGL veribles
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glfwShowWindow(window);

	GLFWWindowDeleter windowDeleter{ window };

	int iwidth, iheight;

	glfwGetFramebufferSize(window, &iwidth, &iheight);

	RenderingHelp::mainThreadID = std::this_thread::get_id();

	glViewport(0, 0, iwidth, iheight);

	// Create a camera
	Camera camera(90.0f, 0.1f, 500.0f, glm::vec3(-40.0f, 40.0f, 40.0f), true);
	camera.set_rotation(glm::vec3(45.0f, 45.0f, 0.0f));

	// Load the shader programs
	ShaderProgram::registerShader(
		"simple3D", 
		"assets/shaders/simple3D.frag",
		"assets/shaders/simple3D.vert"
	);

	// Load Terrain shader
	ShaderProgram::registerShader(
		"terrain",
		"assets/shaders/terrain.frag",
		"assets/shaders/terrain.vert"
	);

	// Start Imgui
	ImguiCore::init(window);

	// Scene
	Scene scene("Main");

	TerrainManager::init();

	// Setting up timer for frame time
	double last = glfwGetTime();

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Let GLFW process events
		glfwPollEvents();

		if (ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize(window, &nwidth, &nheight);

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if (0 == nwidth || 0 == nheight)
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize(window, &nwidth, &nheight);
				} while (0 == nwidth || 0 == nheight);
			}

		}
		// calculate delta time
		double now = glfwGetTime();
		double deltaTime = now - last;
		last = now;

		// update viewport
		glViewport(0, 0, (GLsizei)fbwidth, (GLsizei)fbheight);

		// Update the camera
		camera.update(deltaTime, window);
		camera.set_window_size(fbwidth, fbheight);

		TerrainManager::updateTerrain();

		if (!ImguiCore::getVisable())
			Scene::render_current_scene();

		ImguiCore::render();


		// Display results
		glfwSwapBuffers(window);

		// Clear the buffers
		glClearColor(
			LightingManager::clearColor.x,
			LightingManager::clearColor.y,
			LightingManager::clearColor.z,
			LightingManager::clearColor.w
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	TerrainManager::destroy();

	return 0;
}
catch (std::exception const& eErr)
{
	std::fprintf(stderr, "Top-level Exception (%s):\n", typeid(eErr).name());
	std::fprintf(stderr, "%s\n", eErr.what());
	std::fprintf(stderr, "Bye.\n");
	return 1;
}


namespace
{
	void APIENTRY openglCallbackFunction(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		//std::cerr << "OpenGL Debug Message: " << message << std::endl;
		//if (severity == GL_DEBUG_SEVERITY_HIGH)
		//{
		//	std::cerr << "Aborting due to high severity error." << std::endl;
		//	abort();
		//}
	}

	void setupDebugCallback()
	{
		//glEnable(GL_DEBUG_OUTPUT);
		//glDebugMessageCallback(openglCallbackFunction, nullptr);
	}

	void glfw_callback_error_(int aErrNum, char const* aErrDesc)
	{
		std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
	}
}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if (window)
		{
			ImguiCore::destroy();
			glfwDestroyWindow(window);
		}
	}
}
