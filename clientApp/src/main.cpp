#include "../glfw/include/GLFW/glfw3.h"
// #include "../glad/include/glad/glad.h"
#include "imgui.h"
#include <iostream>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
// #ifdef __APPLE__
//     std::cout << "I'm apple machine" << std::endl;
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

void renderTriangle()
{
    // Define las coordenadas de los vértices del triángulo
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Vértice 1 (abajo izquierda)
        0.5f, -0.5f, 0.0f,  // Vértice 2 (abajo derecha)
        0.0f, 0.5f, 0.0f    // Vértice 3 (arriba centro)
    };

    // Genera un identificador para el búfer de vértices (VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Enlaza el búfer de vértices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Habilita el atributo de posición de los vértices (0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    // Dibuja el triángulo
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Deshabilita el atributo de posición de los vértices (0)
    glDisableVertexAttribArray(0);
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
    std::cout << "I'm apple machine" << std::endl;
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(1280, 720, "OpenGL + ImGui", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Haz la ventana GLFW el contexto actual
    glfwMakeContextCurrent(window);

    // Inicializa Glad para cargar las funciones de OpenGL
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    // {
    //     fprintf(stderr, "Error al inicializar Glad\n");
    //     return -1;
    // }

    // Inicializa ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Renderiza el triángulo
        renderTriangle();

        // Procesa los eventos de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        // ImGui::DockSpaceOverViewport();
        ImGui::NewFrame();

        // Renderiza la ventana de ImGui
        {
            ImGui::Begin("Hello, world!");
            ImGui::Text("JULAPAS PUTA VACA");
            ImGui::End();
        }

        // Renderiza la ventana de ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Intercambia los buffers y maneja los eventos de GLFW
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