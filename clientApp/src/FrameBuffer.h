// FrameBuffer.h

#pragma once
#include <iostream>
#include "../glfw/include/GLFW/glfw3.h"
#include <OpenGL/gl3.h>
#include "imgui.h"
#include <iostream>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class FrameBuffer
{
public:
	FrameBuffer(float width, float height);
	~FrameBuffer();
	unsigned int getFrameTexture();
	void RescaleFrameBuffer(float width, float height);
	void Bind() const;
	void Unbind() const;

private:
	unsigned int fbo;
	unsigned int texture;
	unsigned int rbo;
};