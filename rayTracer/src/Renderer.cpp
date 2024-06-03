#include "Renderer.h"
#include "Window.h"
#include <random>
#include <execution>
#include <algorithm>
#include <tbb/parallel_for_each.h>

namespace Utils
{
    static thread_local std::mt19937 RandomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> Distribution;

    static glm::uint32 ConvertToRGBA(const glm::vec4 &color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);

        glm::uint32 result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }

    static float Float()
    {
        return (float)Distribution(RandomEngine) / (float)std::numeric_limits<glm::uint32>::max();
    }

    static glm::vec3 RandomVec3(float min, float max)
    {
        return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
    }

}

Renderer::Renderer(Window &window) : window(window)
{
    renderImage = 0;
    image.width = 1280;
    image.height = 720;
    activeCamera = new Camera(45.0f, 0.1f, 100.0f, window);
    activeScene = &window.scene;
}

void Renderer::Update(float ts)
{
    activeCamera->OnResize(image.width, image.height);
    // activeCamera->OnUpdate(ts);
    if (activeCamera->OnUpdate(ts))
        ResetFrameIndex();
    Render(*activeScene, *activeCamera);
}

Renderer::~Renderer()
{
    if (renderImage != 0)
    {
        glDeleteTextures(1, &renderImage);
    }
    delete activeCamera;
}

glm::vec4 Renderer::RayGun(glm::uint32 x, glm::uint32 y)
{
    Ray ray;
    ray.Origin = activeCamera->GetPosition();
    ray.Direction = activeCamera->GetRayDirections()[x + y * image.width];

    glm::vec3 color(0.0f);
    float multiplier = 1.0f;

    int bounces = 5;
    for (int i = 0; i < bounces; i++)
    {
        Renderer::HitPayload payload = TraceRay(ray);
        if (payload.HitDistance < 0.0f)
        {
            glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
            color += skyColor * multiplier;
            break;
        }

        // glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
        // float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle)

        const Shape &shape = *activeScene->Shapes[payload.ObjectIndex];
        const Material &material = activeScene->Materials[shape.GetMaterialIndex()];

        glm::vec3 sphapeColor = material.Albedo;
        // sphapeColor *= lightIntensity;
        color += sphapeColor * multiplier;

        multiplier *= 0.5f;

        ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        ray.Direction = glm::reflect(ray.Direction,
                                     payload.WorldNormal + material.Roughness * Utils::RandomVec3(-0.5f, 0.5f));
    }

    return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray &ray)
{
    // (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
    // where
    // a = ray origin
    // b = ray direction
    // r = radius
    // t = hit distance

    int closestShape = -1;
    float hitDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i < activeScene->Shapes.size(); i++)
    {
        const Shape &shape = *activeScene->Shapes[i];
        float distance = shape.GetClosestHit(ray);
        if (distance < hitDistance)
        {
            hitDistance = distance;
            closestShape = (int)i;
        }
    }

    if (closestShape < 0)
        return Miss(ray);

    return ClosestHit(ray, hitDistance, closestShape);
}

void Renderer::OnResize(glm::uint32 width, glm::uint32 height)
{
    if (width == image.width && height == image.height)
        return;
    image.width = width;
    image.height = height;

    delete[] accumulationData;
    accumulationData = new glm::vec4[width * height];

    hIter.resize(width);
    vIter.resize(height);
    for (glm::uint32 i = 0; i < width; i++)
        hIter[i] = i;
    for (glm::uint32 i = 0; i < height; i++)
        vIter[i] = i;
}

void Renderer::Render(Scene &scene, Camera &camera)
{
    activeScene = &scene;
    activeCamera = &camera;

    if (renderImage != 0)
    {
        glDeleteTextures(1, &renderImage);
    }
    glGenTextures(1, &renderImage);
    glBindTexture(GL_TEXTURE_2D, renderImage);

    if (frameIndex == 1)
    {
        memset(accumulationData, 0, image.width * image.height * sizeof(glm::vec4));
    }

    Ray ray;
    // Generar los datos de la imagen
    ray.Origin = camera.GetPosition();
    glm::uint32 *data = new glm::uint32[image.width * image.height];

#define MT 1
#if MT
    tbb::parallel_for_each(vIter.begin(), vIter.end(),
                           [this, data](uint32_t y)
                           {
                               tbb::parallel_for_each(hIter.begin(), hIter.end(),
                                                      [this, y, &data](uint32_t x)
                                                      {
                                                          glm::vec4 color = RayGun(x, y);
                                                          // color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
                                                          // data[y * image.width + x] = Utils::ConvertToRGBA(color);
                                                          accumulationData[x + y * image.width] += color;

                                                          glm::vec4 accumulatedColor = accumulationData[x + y * image.width];
                                                          accumulatedColor /= (float)frameIndex;

                                                          accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
                                                          data[x + y * image.width] = Utils::ConvertToRGBA(accumulatedColor);
                                                      });
                           });

#else

    for (glm::uint32 y = 0; y < image.height; y++)
    {
        for (glm::uint32 x = 0; x < image.width; x++)
        {
            glm::vec4 color = RayGun(x, y);
            // color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            // data[y * image.width + x] = Utils::ConvertToRGBA(color);
            accumulationData[x + y * image.width] += color;

            glm::vec4 accumulatedColor = accumulationData[x + y * image.width];
            accumulatedColor /= (float)frameIndex;

            accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
            data[x + y * image.width] = Utils::ConvertToRGBA(accumulatedColor);
        }
    }
#endif
    // Subir los datos a la textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Configurar parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;
    if (settings.Accumulate)
        frameIndex++;
    else
        frameIndex = 1;
}

Renderer::HitPayload Renderer::ClosestHit(const Ray &ray, float hitDistance, int objectIndex)
{
    Renderer::HitPayload payload;
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    const Shape &closestShape = *activeScene->Shapes[objectIndex];

    glm::vec3 origin = ray.Origin - closestShape.GetPosition();
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition);

    payload.WorldPosition += closestShape.GetPosition();

    return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray &ray)
{
    Renderer::HitPayload payload;
    payload.HitDistance = -1.0f;
    return payload;
}
