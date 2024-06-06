#pragma once

#define GL_SILENCE_DEPRECATION
#include "Shape.h"

class Plane : public Shape
{
public:
    Plane();
    ~Plane();

    bool Intersect(const glm::vec3& RayOrigin, const glm::vec3& RayDirection, float& t) const override;
    glm::vec3 GetNormal(const glm::vec3& Point) const override;
    float GetClosestHit(const Ray& ray) const override;

    glm::vec3 Normal{0.0f, -1.0f, 0.0f};
private:
};