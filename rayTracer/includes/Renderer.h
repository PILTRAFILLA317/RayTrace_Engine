#pragma once

#define GL_SILENCE_DEPRECATION
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
        bool Accumulate = false;
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

    glm::uint32 frameIndex = 1;

    std::vector<glm::uint32> hIter, vIter;

public:
    Renderer(Window &window);
    ~Renderer();

    void Update(float ts);
    void OnResize(glm::uint32 width, glm::uint32 height);
    void Render(Scene &scene, Camera &camera);

    glm::vec4 RayGun(glm::uint32 x, glm::uint32 y); // RayGen

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