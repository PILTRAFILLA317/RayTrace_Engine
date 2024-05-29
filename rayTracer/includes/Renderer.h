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
public:
    struct Settings
    {
        bool Accumulate = true;
    };
    struct HitPayload
    {
        float HitDistance;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;

        int ObjectIndex;
    };
    struct ImageData
    {
        int width;
        int height;
        glm::uint32 *data;
    };

private:
    GLuint renderImage = 0;
    // int sceneWindowWidth = 1280;
    // int sceneWindowHeight = 720;

    ImageData image;

    Window &window;

    Scene *activeScene;
    Camera *activeCamera;

    Settings settings;

    glm::vec4 *accumulationData = nullptr;

    uint32_t frameIndex = 1;

public:
    Renderer(Window &window);
    ~Renderer();

    void Update(float ts);
    void OnResize(uint32_t width, uint32_t height);
    void Render(Scene &scene, Camera &camera);

    glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen

    HitPayload TraceRay(const Ray &ray);
    HitPayload ClosestHit(const Ray &ray, float hitDistance, int objectIndex);
    HitPayload Miss(const Ray &ray);

    inline float GetSceneWindowWidth()
    {
        return image.width;
    }
    inline float GetSceneWindowHeight()
    {
        return image.height;
    }
    inline GLuint GetRenderImage()
    {
        return renderImage;
    }

    void setSceneWindowWidth(float width)
    {
        image.width = width;
    }
    void setSceneWindowHeight(float height)
    {
        image.height = height;
    }

    void ResetFrameIndex() { frameIndex = 1; }
    Settings &GetSettings() { return settings; }
};