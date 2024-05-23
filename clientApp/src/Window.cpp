#include "Window.h"
#include "Renderer.h"

float Window::GetTime()
{
    return (float)glfwGetTime();
}

Window::Window(int width, int height, const std::string &title)
    : width(width), height(height), title(title), renderer(nullptr), window(nullptr)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(1);
    }

    glfwPollEvents();

    // Crea una ventana GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    createRenderer();
}

void Window::mainLoop()
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

    while (!glfwWindowShouldClose(window))
    {
        // Procesa los eventos de GLFW
        glfwPollEvents();

        renderer->Update(io.Framerate);

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
            ImGui::Text("Mouse Position: (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
            ImGui::End();
        }

        ImGui::Begin("Scene");
        {
            renderer->setSceneWindowWidth(ImGui::GetContentRegionAvail().x);
            renderer->setSceneWindowHeight(ImGui::GetContentRegionAvail().y);
            ImGui::Image((void *)(intptr_t)renderer->GetRenderImage(), ImVec2(renderer->GetSceneWindowWidth(), renderer->GetSceneWindowHeight()), ImVec2(0, 1), ImVec2(1, 0));
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
}

void Window::createRenderer()
{
    renderer = new Renderer(*this);
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
    delete renderer;
}

void Window::Update()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Window::IsOpen()
{
    return !glfwWindowShouldClose(window);
}

void Window::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Close()
{
    glfwSetWindowShouldClose(window, true);
}

void Window::SetCursorMode(CursorMode mode)
{
    glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(mode));
}

void Window::SetCursorPos(double x, double y)
{
    glfwSetCursorPos(window, x, y);
}
