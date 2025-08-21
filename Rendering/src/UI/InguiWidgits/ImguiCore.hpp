#pragma once


#include "../../imgui/imgui_impl_glfw.h"
#include "ScreenRenderWidgit.hpp"

class ImguiCore
{
public:
	~ImguiCore();

	static void init(GLFWwindow* window);
	static void render();
	static void destroy();
	static bool getVisable() { return isVisable; }

	static ScreenRenderWidgit* getScreenRenderWidgit() { return screenRenderWidgit; }
#
	static int triangleCount;

private:
	static void menuBar();

	static void imguiSettingsWiondow();
	static void openGLSettingsWindow();
	static void proformanceWindow();

	static bool isVisable;

	// Our state
	static bool seeThroughDockSpace;
	static bool showDemoWindow;
	static bool showImguiSettings;
	static bool showOpenGLSettings;
	static bool showProformance;

	static ScreenRenderWidgit* screenRenderWidgit;
};

