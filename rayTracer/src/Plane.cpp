#include "Plane.h"
#include <iostream>

Plane::Plane()
{
    Position = glm::vec3(0.0f);
    Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    MaterialIndex = 0;

    SetType(ShapeType::Plane);
}

Plane::~Plane()
{
}

bool Plane::Intersect(const glm::vec3& RayOrigin, const glm::vec3& RayDirection, float& t) const
{
    float denom = glm::dot(Normal, RayDirection);
    if (denom > 1e-6)
    {
        glm::vec3 p0l0 = Position - RayOrigin;
        t = glm::dot(p0l0, Normal) / denom;
        return (t >= 0);
    }
    return false;
}

glm::vec3 Plane::GetNormal(const glm::vec3& Point) const
{
    return Normal;
}

float Plane::GetClosestHit(const Ray& ray) const
{
    float t = 0.0f;
    if (Intersect(ray.Origin, ray.Direction, t))
    {
        return t;
    }
    return -1.0f;
}