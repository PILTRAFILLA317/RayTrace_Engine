#include "main.h"
#include "Renderer.h"

// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

int main()
{
    Window window(1280, 720, "Briar Engine");

    window.mainLoop();
    // processInput(window);
    return 0;
}