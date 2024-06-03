#pragma once

#define GL_SILENCE_DEPRECATION
#include "Shape.h"
#include "Ray.h"
#include <glm/glm.hpp>
#include <vector>

class Sphere : public Shape
{
public:
    Sphere();
    ~Sphere();

    bool Intersect(const glm::vec3& RayOrigin, const glm::vec3& RayDirection, float& t) const override;
    glm::vec3 GetNormal(const glm::vec3& Point) const override;
    float GetClosestHit(const Ray& ray) const override;

    float GetRadius() const { return Radius; }
    void SetRadius(float radius) { Radius = radius; }

private:
    float Radius = 0.5f;
};
