#pragma once

#include <iostream>
#include "../glfw/include/GLFW/glfw3.h"
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Camera.h"
#include "Window.h"
#include "Ray.h"
#include "Scene.h"

class Window;

class Camera;

class Renderer
{
private:
    GLuint renderImage = 0;
    float sceneWindowWidth = 1280;
    float sceneWindowHeight = 720;
    
    Window &window;

    Camera *camera;

public:
    Renderer(Window &window);
    ~Renderer();

    void Update(float ts);

    void Render(const Camera &camera);

    glm::vec4 TraceRay(const Ray& ray);

    glm::vec4 PerPixel(glm::vec2 coord);

    inline float GetSceneWindowWidth()
    {
        return sceneWindowWidth;
    }
    inline float GetSceneWindowHeight()
    {
        return sceneWindowHeight;
    }
    inline GLuint GetRenderImage()
    {
        return renderImage;
    }

    void setSceneWindowWidth(float width)
    {
        sceneWindowWidth = width;
    }
    void setSceneWindowHeight(float height)
    {
        sceneWindowHeight = height;
    }
};