#include "Camera.h"
#include "Window.h"
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera(float verticalFOV, float nearClip, float farClip, Window &window)
    : VerticalFOV(verticalFOV), NearClip(nearClip), FarClip(farClip), newWindow(window)
{
    ForwardDirection = glm::vec3(0, 0, -1);
    Position = glm::vec3(0, 0, 3);
}

void Camera::OnUpdate(float ts)
{
    glm::vec2 mousePos = newWindow.GetMousePosition();
    glm::vec2 delta = (mousePos - LastMousePosition) * 0.002f;
    LastMousePosition = mousePos;

    if (!newWindow.IsMouseButtonDown(MouseButton::Right))
    {
        newWindow.SetCursorMode(CursorMode::Normal);
        return;
    }

    newWindow.SetCursorMode(CursorMode::Locked);

    bool moved = false;

    constexpr float upX = 0.0f;
    constexpr float upY = 1.0f;
    constexpr float upZ = 0.0f;
    const glm::vec3 upDirection(upX, upY, upZ);
    glm::vec3 rightDirection = glm::cross(ForwardDirection, upDirection);

    float speed = 0.0003f;

    // Movement
    if (newWindow.IsKeyDown(KeyCode::W))
    {
        Position += ForwardDirection * speed * ts;
        moved = true;
    }
    else if (newWindow.IsKeyDown(KeyCode::S))
    {
        Position -= ForwardDirection * speed * ts;
        moved = true;
    }
    if (newWindow.IsKeyDown(KeyCode::A))
    {
        Position -= rightDirection * speed * ts;
        moved = true;
    }
    else if (newWindow.IsKeyDown(KeyCode::D))
    {
        Position += rightDirection * speed * ts;
        moved = true;
    }
    if (newWindow.IsKeyDown(KeyCode::E))
    {
        Position -= upDirection * speed * ts;
        moved = true;
    }
    else if (newWindow.IsKeyDown(KeyCode::Q))
    {
        Position += upDirection * speed * ts;
        moved = true;
    }

    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f)
    {
        float pitchDelta = delta.y * GetRotationSpeed();
        float yawDelta = delta.x * GetRotationSpeed();

        glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
                                                glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
        ForwardDirection = glm::rotate(q, ForwardDirection);

        moved = true;
    }

    if (moved)
    {
        RecalculateView();
        RecalculateRayDirections();
    }
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
    if (width == ViewportWidth && height == ViewportHeight)
        return;

    ViewportWidth = width;
    ViewportHeight = height;

    RecalculateProjection();
    RecalculateRayDirections();
}

float Camera::GetRotationSpeed()
{
    return 0.3f;
}

void Camera::RecalculateProjection()
{
    Projection = glm::perspectiveFov(glm::radians(VerticalFOV), (float)ViewportWidth, (float)ViewportHeight, NearClip, FarClip);
    InverseProjection = glm::inverse(Projection);
}

void Camera::RecalculateView()
{
    View = glm::lookAt(Position, Position + ForwardDirection, glm::vec3(0, 1, 0));
    InverseView = glm::inverse(View);
}

void Camera::RecalculateRayDirections()
{
    RayDirections.resize(ViewportWidth * ViewportHeight);

    for (uint32_t y = 0; y < ViewportHeight; y++)
    {
        for (uint32_t x = 0; x < ViewportWidth; x++)
        {
            glm::vec2 coord = {(float)x / (float)ViewportWidth, (float)y / (float)ViewportHeight};
            coord = coord * 2.0f - 1.0f; // -1 -> 1

            glm::vec4 target = InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
            glm::vec3 rayDirection = glm::vec3(InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
            RayDirections[x + y * ViewportWidth] = rayDirection;
        }
    }
}
