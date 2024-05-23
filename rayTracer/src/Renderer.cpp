#include "Renderer.h"
#include "Window.h"

namespace Utils
{

    static uint32_t ConvertToRGBA(const glm::vec4 &color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);

        uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }

}

Renderer::Renderer(Window &window)
{
    renderImage = 0;
    sceneWindowWidth = 1280;
    sceneWindowHeight = 720;
    camera = new Camera(45.0f, 0.1f, 100.0f, window);
}

void Renderer::Update(float ts)
{
    camera->OnResize(sceneWindowWidth, sceneWindowHeight);
    camera->OnUpdate(ts);
    Render(*camera);
}

Renderer::~Renderer()
{
    if (renderImage != 0)
    {
        glDeleteTextures(1, &renderImage);
    }
    delete camera;
}

glm::vec4 Renderer::TraceRay(const Ray &ray)
{
    float radius = 0.5f;
    // rayDirection = glm::normalize(rayDirection);

    // (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
    // where
    // a = ray origin
    // b = ray direction
    // r = radius
    // t = hit distance

    float a = glm::dot(ray.Direction, ray.Direction);
    float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
    float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

    // Quadratic forumula discriminant:
    // b^2 - 4ac

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return glm::vec4(0, 0, 0, 1);

    // Quadratic formula:
    // (-b +- sqrt(discriminant)) / 2a

    float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)

    glm::vec3 hitPoint = ray.Origin + ray.Direction * closestT;
    glm::vec3 normal = glm::normalize(hitPoint);

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
    float lightIntensity = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

    glm::vec3 sphereColor(1, 0, 1);
    sphereColor *= lightIntensity;
    return glm::vec4(sphereColor, 1.0f);
}

void Renderer::Render(const Camera &camera)
{
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
    for (int y = 0; y < sceneWindowHeight; y++)
    {
        for (int x = 0; x < sceneWindowWidth; x++)
        {
            // glm::vec2 coord = glm::vec2(x / (float)sceneWindowWidth, y / (float)sceneWindowHeight);
            // std::cerr << "Pixel: " << x << ", " << y << std::endl;
            ray.Direction = camera.GetRayDirections()[x + y * (float)sceneWindowWidth];
            glm::vec4 color = TraceRay(ray);
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
