#pragma once

#define GL_SILENCE_DEPRECATION
#include <glm/glm.hpp>

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
};