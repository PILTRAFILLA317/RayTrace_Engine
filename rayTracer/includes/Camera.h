#pragma once

#include <glm/glm.hpp>
#include "Window.h"
#include <vector>

class Window;

class Camera
{
public:
	Camera(float verticalFOV, float nearClip, float farClip, Window &window);

	bool OnUpdate(float ts);
	void OnResize(glm::uint32 width, glm::uint32 height);

	const glm::mat4& GetProjection() const { return Projection; }
	const glm::mat4& GetInverseProjection() const { return InverseProjection; }
	const glm::mat4& GetView() const { return View; }
	const glm::mat4& GetInverseView() const { return InverseView; }
	
	const glm::vec3& GetPosition() const { return Position; }
	const glm::vec3& GetDirection() const { return ForwardDirection; }

	const std::vector<glm::vec3>& GetRayDirections() const { return RayDirections; }

	float GetRotationSpeed();
private:
	void RecalculateProjection();
	void RecalculateView();
	void RecalculateRayDirections();
private:
	Window &newWindow;

	glm::mat4 Projection{ 1.0f };
	glm::mat4 View{ 1.0f };
	glm::mat4 InverseProjection{ 1.0f };
	glm::mat4 InverseView{ 1.0f };

	float VerticalFOV = 45.0f;
	float NearClip = 0.1f;
	float FarClip = 100.0f;

	glm::vec3 Position{0.0f, 0.0f, 0.0f};
	glm::vec3 ForwardDirection{0.0f, 0.0f, 0.0f};

	// Cached ray directions
	std::vector<glm::vec3> RayDirections;

	glm::vec2 LastMousePosition{ 0.0f, 0.0f };

	glm::uint32 ViewportWidth = 0, ViewportHeight = 0;
};
