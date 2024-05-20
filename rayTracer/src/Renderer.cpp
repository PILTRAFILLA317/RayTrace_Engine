#include "Renderer.h"

Renderer::Renderer()
{
    renderImage = 0;
    sceneWindowWidth = 1280;
    sceneWindowHeight = 720;
}

void Renderer::Render()
{
    std::cout << "Rendering image with size: " << sceneWindowWidth << "x" << sceneWindowHeight << std::endl;
    if (renderImage != 0)
    {
        glDeleteTextures(1, &renderImage);
    }
    glGenTextures(1, &renderImage);
    glBindTexture(GL_TEXTURE_2D, renderImage);

    // Generar los datos de la imagen
    unsigned char *data = new unsigned char[(uint32_t)sceneWindowWidth * (uint32_t)sceneWindowHeight * 3];
    for (int i = 0; i < sceneWindowWidth * sceneWindowHeight * 3; i++)
    {
        data[i] = rand() % 256; // Generar valores aleatorios entre 0 y 255
    }
    // Subir los datos a la textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneWindowWidth, sceneWindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Configurar parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{

}

float Renderer::GetSceneWindowWidth()
{
    return sceneWindowWidth;
}

float Renderer::GetSceneWindowHeight()
{
    return sceneWindowHeight;
}

GLuint Renderer::GetRenderImage()
{
    return renderImage;
}

void Renderer::setSceneWindowWidth(float width)
{
    sceneWindowWidth = width;
}

void Renderer::setSceneWindowHeight(float height)
{
    sceneWindowHeight = height;
}
