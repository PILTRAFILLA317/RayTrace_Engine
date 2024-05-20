#pragma once

#include <iostream>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_opengl3.h"



class Renderer
{
private:
    GLuint renderImage = 0;
    float sceneWindowWidth = 1280;
    float sceneWindowHeight = 720;

public:
    Renderer();

    void Render();
    uint32_t PerPixel(glm::vec2 coord);

    float GetSceneWindowWidth();
    float GetSceneWindowHeight();
    GLuint GetRenderImage();

    void setSceneWindowWidth(float width);
    void setSceneWindowHeight(float height);
};