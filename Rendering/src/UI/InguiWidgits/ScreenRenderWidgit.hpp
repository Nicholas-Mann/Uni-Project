#pragma once

class ScreenRenderWidgit
{
public:
	static bool showScreenRenderWidgit;

	ScreenRenderWidgit(unsigned int width, unsigned int height);
	~ScreenRenderWidgit();
	void cleanUp();
	void create_framebuffer();
	void bind_framebuffer();
	void unbind_framebuffer();
	void rescale_framebuffer(int width, int height);

	void updateWindow();
	void render();
private:
	unsigned int FBO = -1;
	unsigned int texture_id = -1;
	unsigned int RBO = -1;
	int width = 800;
	int height = 600;
};

