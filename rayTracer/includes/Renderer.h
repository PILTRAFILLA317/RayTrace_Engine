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

    Scene *activeScene;
    Camera *activeCamera;

public:
    Renderer(Window &window);
    ~Renderer();

    void Update(float ts);

    void Render(Scene& scene, Camera& camera);

    struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);

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