#pragma once

#define GL_SILENCE_DEPRECATION
#include <glm/glm.hpp>
#include "Sphere.h"
#include <vector>

struct Light
{
    glm::vec3 Position;
    glm::vec3 Color;
};

struct Material
{
    glm::vec3 Albedo{ 1.0f };
    float Roughness = 1.0f;
    float Metallic = 0.0f;
    float Specular = 0.5f; // Intensidad especular
    float Shininess = 32.0f; // Exponente especular
};

// struct Sphere
// {
// 	glm::vec3 Position{0.0f};
// 	float Radius = 0.5f;

// 	int MaterialIndex = 0;
// };

struct Scene
{
	std::vector<std::shared_ptr<Shape>> Shapes;
	std::vector<Material> Materials;
	std::vector<Light> Lights;
};