#include "FrameBuffer.h"
#include "../glfw/include/GLFW/glfw3.h"
// #include <glad/glad.h>
#include "imgui.h"
#include <iostream>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <OpenGL/gl3.h>

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShader1Source = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                    "}\n\0";

// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void renderTriangle()
{
    // Define las coordenadas de los vértices del triángulo
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Vértice 1 (abajo izquierda)
        0.5f, -0.5f, 0.0f,  // Vértice 2 (abajo derecha)
        0.0f, 0.5f, 0.0f    // Vértice 3 (arriba centro)
    };

    // Genera un identificador para el búfer de vértices (VBO)
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Enlaza el búfer de vértices
    // glBindVertexArray(VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Habilita el atributo de posición de los vértices (0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Dibuja el triángulo
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Deshabilita el atributo de posición de los vértices (0)
    glDisableVertexAttribArray(0);
    glDeleteVertexArrays(1, &VAO);
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

    // Haz la ventana GLFW el contexto actual
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER); // the first fragment shader that outputs the color orange
    unsigned int shaderProgramOrange = glCreateProgram();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(fragmentShaderOrange, 1, &fragmentShader1Source, NULL);
    glCompileShader(fragmentShaderOrange);
    glAttachShader(shaderProgramOrange, vertexShader);
    glAttachShader(shaderProgramOrange, fragmentShaderOrange);
    glLinkProgram(shaderProgramOrange);

    // Inicializa ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    FrameBuffer sceneBuffer(1280, 720);

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Renderiza el triángulo

        // Procesa los eventos de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        // ImGui::DockSpaceOverViewport();
        ImGui::NewFrame();

        // Renderiza la ventana de ImGui
        // {
        //     ImGui::Begin("Hello, world!");
        //     ImGui::Text("JULAPAS PUTA VACA");
        //     ImGui::End();
        // }

        ImGui::Begin("Scene");
        {
            const float window_width = ImGui::GetContentRegionAvail().x;
            const float window_height = ImGui::GetContentRegionAvail().y;
            ImGui::BeginChild("GameRender");
            std::cout << "Window width: " << window_width << std::endl;
            std::cout << "Window height: " << window_height << std::endl;
            sceneBuffer.RescaleFrameBuffer(window_width, window_height);
            glViewport(0, 0, window_width, window_height);
            ImGui::Image(
                (ImTextureID)sceneBuffer.getFrameTexture(),
                ImGui::GetContentRegionAvail(),
                ImVec2(0, 1),
                ImVec2(1, 0));
        }
        ImGui::EndChild();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        sceneBuffer.Bind();

        glClearColor(0.3f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Renderiza la ventana de ImGui
        glUseProgram(shaderProgramOrange);
        renderTriangle();

        // Intercambia los buffers y maneja los eventos de GLFW
        sceneBuffer.Unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}