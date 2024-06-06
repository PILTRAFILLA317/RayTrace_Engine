#include "Sphere.h"
#include <algorithm>
#include <glm/glm.hpp>
#include "Shape.h"
#include "Renderer.h"

Sphere::Sphere() : Shape()
{
    this->Position = glm::vec3(0.0f);
    this->Radius = 0.5f;
    this->MaterialIndex = 0;
    this->SetType(ShapeType::Sphere);
}

Sphere::~Sphere()
{
}

bool Sphere::Intersect(const glm::vec3 &RayOrigin, const glm::vec3 &RayDirection, float &t) const
{
    glm::vec3 L = Position - RayOrigin;
    float tca = glm::dot(L, RayDirection);
    float d2 = glm::dot(L, L) - tca * tca;
    if (d2 > Radius * Radius)
        return false;
    float thc = sqrt(Radius * Radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if (t0 > t1)
        std::swap(t0, t1);

    if (t0 < 0)
    {
        t0 = t1;
        if (t0 < 0)
            return false;
    }

    t = t0;
    return true;
}

glm::vec3 Sphere::GetNormal(const glm::vec3 &Point) const
{
    return glm::normalize(Point);
}

float Sphere::GetClosestHit(const Ray &ray) const
{
    int closestSphere = -1;
    float hitDistance = std::numeric_limits<float>::max();

    glm::vec3 origin = ray.Origin - Position;

    float a = glm::dot(ray.Direction, ray.Direction);
    float b = 2.0f * glm::dot(origin, ray.Direction);
    float c = glm::dot(origin, origin) - Radius * Radius;

    // Quadratic forumula discriminant:
    // b^2 - 4ac

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return (hitDistance);

    // Quadratic formula:
    // (-b +- sqrt(discriminant)) / 2a

    // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
    float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    if (closestT > 0.0f && closestT < hitDistance)
    {
        hitDistance = closestT;
        return (hitDistance);
    }

    // if (closestSphere < 0)
    //     return Miss(ray);

    // return ClosestHit(ray, hitDistance, closestSphere);
}