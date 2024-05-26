#include "Renderer.h"
#include "Window.h"

namespace Utils
{

    static glm::uint32 ConvertToRGBA(const glm::vec4 &color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);

        glm::uint32 result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }

}

Renderer::Renderer(Window &window) : window(window)
{
    renderImage = 0;
    sceneWindowWidth = 1280;
    sceneWindowHeight = 720;
    activeCamera = new Camera(45.0f, 0.1f, 100.0f, window);
    activeScene = &window.scene;
}

void Renderer::Update(float ts)
{
    activeCamera->OnResize(sceneWindowWidth, sceneWindowHeight);
    activeCamera->OnUpdate(ts);
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

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    Ray ray;
    ray.Origin = activeCamera->GetPosition();
    ray.Direction = activeCamera->GetRayDirections()[x + y * sceneWindowWidth];

    glm::vec3 color(0.0f);
    float multiplier = 1.0f;

    int bounces = 2;
    for (int i = 0; i < bounces; i++)
    {
        Renderer::HitPayload payload = TraceRay(ray);
        if (payload.HitDistance < 0.0f)
        {
            glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
            color += skyColor * multiplier;
            break;
        }

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
        float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle)

        const Sphere &sphere = activeScene->Spheres[payload.ObjectIndex];
        glm::vec3 sphereColor = sphere.Albedo;
        sphereColor *= lightIntensity;
        color += sphereColor * multiplier;

        multiplier *= 0.7f;

        ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal);
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


    int closestSphere = -1;
    float hitDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i < activeScene->Spheres.size(); i++)
    {
        const Sphere &sphere = activeScene->Spheres[i];
        glm::vec3 origin = ray.Origin - sphere.Position;

        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(origin, ray.Direction);
        float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

        // Quadratic forumula discriminant:
        // b^2 - 4ac

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f)
            continue;

        // Quadratic formula:
        // (-b +- sqrt(discriminant)) / 2a

        // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
        float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        if (closestT > 0.0f && closestT < hitDistance)
        {
            hitDistance = closestT;
            closestSphere = (int)i;
        }
    }

    if (closestSphere < 0)
        return Miss(ray);

    return ClosestHit(ray, hitDistance, closestSphere);
}

void Renderer::Render(Scene &scene, Camera &camera)
{
    activeScene = &scene;
    activeCamera = &camera;
    // std::cout << "Rendering image with size: " << width << "x" << height << std::endl;
    if (renderImage != 0)
    {
        glDeleteTextures(1, &renderImage);
    }
    glGenTextures(1, &renderImage);
    glBindTexture(GL_TEXTURE_2D, renderImage);

    Ray ray;
    // Generar los datos de la imagen
    ray.Origin = camera.GetPosition();
    glm::uint32 *data = new glm::uint32[(int)sceneWindowWidth * (int)sceneWindowHeight];

    for (glm::uint32 y = 0; y < sceneWindowHeight; y++)
    {
        for (glm::uint32 x = 0; x < sceneWindowWidth; x++)
        {
            glm::vec4 color = PerPixel(x, y);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            data[y * (int)sceneWindowWidth + x] = Utils::ConvertToRGBA(color);
        }
    }
    // Subir los datos a la textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sceneWindowWidth, sceneWindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Configurar parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;
}

Renderer::HitPayload Renderer::ClosestHit(const Ray &ray, float hitDistance, int objectIndex)
{
    Renderer::HitPayload payload;
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    const Sphere &closestSphere = activeScene->Spheres[objectIndex];

    glm::vec3 origin = ray.Origin - closestSphere.Position;
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition);

    payload.WorldPosition += closestSphere.Position;

    return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray &ray)
{
    Renderer::HitPayload payload;
    payload.HitDistance = -1.0f;
    return payload;
}
