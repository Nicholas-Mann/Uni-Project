#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

class RenderingHelp
{
public:
	static std::thread::id mainThreadID;

	static GLFWwindow* createOpenGLContext(int width, int height, const char* title, GLFWwindow* share = nullptr);
};

