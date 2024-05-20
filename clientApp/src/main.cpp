#include "FrameBuffer.h"
#include "../glfw/include/GLFW/glfw3.h"
// #include <glad/glad.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include <iostream>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <OpenGL/gl3.h>

GLuint renderImage = 0;
float sceneWindowWidth = 1280;
float sceneWindowHeight = 720;

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "uniform float triangleSize;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "uniform vec4 triangleColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = triangleColor;\n"
                                   "}\n\0";

// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }

float GetTime()
{
    return (float)glfwGetTime();
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

glm::uint32 PerPixel(glm::vec2 coord)
{
    glm::uint8 r = (glm::uint8)(coord.x * 255.0f);
    glm::uint8 g = (glm::uint8)(coord.y * 255.0f);

    return 0xff000000 | (g << 8) | r;
}

void Render(int width, int height)
{
    // std::cout << "Rendering image with size: " << width << "x" << height << std::endl;
    if (renderImage != 0)
    {
        glDeleteTextures(1, &renderImage);
    }
    glGenTextures(1, &renderImage);
    glBindTexture(GL_TEXTURE_2D, renderImage);

    // Generar los datos de la imagen
    glm::uint32 *data = new glm::uint32[width * height];
    for (int y = 0; y < height; y++)
    {
        std::cerr << "Height: " << height << " Y: " << y << "\n";
        for (int x = 0; x < width; x++)
        {
            data[y * width + x] = PerPixel(glm::vec2(x, y));
        }
    }
    // Subir los datos a la textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Configurar parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;
}

int main()
{
    // Inicializa GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Error al inicializar GLFW\n");
        return -1;
    }

    // Crea una ventana GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Briar Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSwapInterval(1); // Enable vsync
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    IMGUI_CHECKVERSION();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER); // the first fragment shader that outputs the color orange
    unsigned int shaderProgram = glCreateProgram();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(fragmentShaderOrange, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderOrange);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShaderOrange);
    glLinkProgram(shaderProgram);

    // Inicializa ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

    // FrameBuffer sceneBuffer(1280, 720);

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        Render(sceneWindowWidth, sceneWindowHeight);
        // Procesa los eventos de GLFW
        glfwPollEvents();
        processInput(window);

        // Procesa los eventos de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        // Renderiza la ventana de ImGui
        {
            ImGui::Begin("Settings");
            // if (ImGui::Button("Render"))
            // {
            //     Render(sceneWindowWidth, sceneWindowHeight);
            // }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        ImGui::Begin("Scene");
        {
            sceneWindowWidth = ImGui::GetContentRegionAvail().x;
            sceneWindowHeight = ImGui::GetContentRegionAvail().y;
            ImGui::Image((void *)(intptr_t)renderImage, ImVec2(sceneWindowWidth, sceneWindowHeight));
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Limpieza
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}