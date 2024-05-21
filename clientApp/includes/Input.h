// Input.h
#pragma once

#include "../glfw/include/GLFW/glfw3.h"
#include <glm/glm.hpp>

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

class Input
{
public:
    static void Initialize(GLFWwindow *window)
    {
        s_Window = window;
    }

    static bool IsKeyDown(KeyCode key)
    {
        return glfwGetKey(s_Window, static_cast<int>(key)) == GLFW_PRESS;
    }

    static bool IsMouseButtonDown(MouseButton button)
    {
        return glfwGetMouseButton(s_Window, static_cast<int>(button)) == GLFW_PRESS;
    }

    static glm::vec2 GetMousePosition()
    {
        double x, y;
        glfwGetCursorPos(s_Window, &x, &y);
        return glm::vec2(static_cast<float>(x), static_cast<float>(y));
    }

    static void SetCursorMode(CursorMode mode)
    {
        glfwSetInputMode(s_Window, GLFW_CURSOR, static_cast<int>(mode));
    }

private:
    static GLFWwindow *s_Window;
};

GLFWwindow *Input::s_Window = nullptr;
