#pragma once

#define GL_SILENCE_DEPRECATION
#include <glm/glm.hpp>
#include "Ray.h"
#include <vector>

enum class ShapeType
{
    Sphere,
    Plane
};

class Shape {
public:
    Shape() = default;
    virtual ~Shape() = default;

    virtual bool Intersect(const glm::vec3& RayOrigin, const glm::vec3& RayDirection, float& t) const = 0;
    virtual glm::vec3 GetNormal(const glm::vec3& Point) const = 0;
    virtual float GetClosestHit(const Ray& ray) const = 0;

    int GetMaterialIndex() const { return MaterialIndex; }
    void SetMaterialIndex(int index) { MaterialIndex = index; }

    glm::vec3 GetPosition() const { return Position; }
    void SetPosition(const glm::vec3& position) { Position = position; }
    ShapeType GetType() const { return Type; }
    ShapeType SetType(ShapeType type) { Type = type; }

    glm::vec3 Position{0.0f};
    int MaterialIndex = 0;
private:
    ShapeType Type;
};

