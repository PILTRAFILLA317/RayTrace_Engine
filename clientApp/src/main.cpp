#include <GLFW/glfw3.h>
#include "imgui.h"
#include <iostream>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void renderTriangle() {
    // Definir las coordenadas de los vértices del triángulo
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Vértice 1 (abajo izquierda)
         0.5f, -0.5f, 0.0f, // Vértice 2 (abajo derecha)
         0.0f,  0.5f, 0.0f  // Vértice 3 (arriba centro)
    };

    // Generar un identificador para el búfer del triángulo
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Enlazar el búfer de vértices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Habilitar y configurar el atributo de posición de los vértices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Renderizar el triángulo
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Deshabilitar el atributo de posición de los vértices
    glDisableVertexAttribArray(0);
}

int main() {
    // Inicializa GLFW
    if (!glfwInit()) {
        std::cout << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Crea una ventana GLFW
    GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL + ImGui", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Configura la configuración de la ventana GLFW
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Habilita el Vsync

    // Inicializa ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 120");

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        // Procesa los eventos de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Renderiza el triángulo
        renderTriangle();

        // Renderiza la ventana de ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
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
