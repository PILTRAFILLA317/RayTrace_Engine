#pragma once

#define GL_SILENCE_DEPRECATION
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "../glfw/include/GLFW/glfw3.h"
#include "Renderer.h"
#include "Camera.h"
#include "imgui.h"
#include <OpenGL/gl3.h>
#include "Scene.h"

class Renderer;

enum class CursorMode
{
    Normal = GLFW_CURSOR_NORMAL,
    Hidden = GLFW_CURSOR_HIDDEN,
    Locked = GLFW_CURSOR_DISABLED
};

enum class KeyCode
{
    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,
    Q = GLFW_KEY_Q,
    E = GLFW_KEY_E,
    // Añadir otras teclas según sea necesario
};

enum class MouseButton
{
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE,
    // Añadir otros botones según sea necesario
};

class Window
{
private:
    int width, height;
    std::string title;
    GLFWwindow *window;
    Renderer *renderer;

public:
    Scene scene;

    Window(int width, int height, const std::string &title);
    ~Window();

    GLFWwindow *getWindow()
    {
        return window;
    }

    Renderer *getRenderer()
    {
        return renderer;
    }

    void createRenderer();

    void mainLoop();

    bool IsKeyDown(KeyCode key)
    {
        return glfwGetKey(this->window, static_cast<int>(key)) == GLFW_PRESS;
    }

    bool IsMouseButtonDown(MouseButton button)
    {
        return glfwGetMouseButton(this->window, static_cast<int>(button)) == GLFW_PRESS;
    }

    glm::vec2 GetMousePosition()
    {
        double x, y;
        glfwGetCursorPos(this->window, &x, &y);
        return glm::vec2(static_cast<float>(x), static_cast<float>(y));
    }

    float GetTime();
    void Update();
    bool IsOpen();
    void Clear();
    void Close();
    void SetCursorMode(CursorMode mode);
    void SetCursorPos(double x, double y);
};