#include "Window.h"
#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>

float Window::GetTime()
{
    return (float)glfwGetTime();
}

Window::Window(int width, int height, const std::string &title)
    : width(width), height(height), title(title), renderer(nullptr), window(nullptr)
{
    Material pinkSphere;
    pinkSphere.Albedo = {1.0f, 0.0f, 1.0f};
    pinkSphere.Roughness = 0.0f;
    scene.Materials.push_back(pinkSphere);

    Material blueSphere;
    blueSphere.Albedo = {0.2f, 0.3f, 1.0f};
    blueSphere.Roughness = 0.1f;
    scene.Materials.push_back(blueSphere);

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
#define GL_SILENCE_DEPRECATION
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

        // Procesa los eventos de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        ImGui::Begin("Adjustments");
        ImGui::Text("Shapes");
        for (size_t i = 0; i < scene.Shapes.size(); i++)
        {
            if (scene.Shapes[i]->GetType() == ShapeType::Sphere)
                ImGui::Text("Sphere");
            else if (scene.Shapes[i]->GetType() == ShapeType::Plane)
                ImGui::Text("Plane");
            ImGui::PushID(i);

            Shape &shape = *scene.Shapes[i];
            ImGui::DragFloat3("Position", glm::value_ptr(shape.Position), 0.1f);
            if (shape.GetType() == ShapeType::Sphere)
                ImGui::DragFloat("Radius", &((Sphere &)shape).Radius, 0.1f);
            else if (shape.GetType() == ShapeType::Plane)
                ImGui::DragFloat3("Normal", glm::value_ptr(((Plane &)shape).Normal), 0.1f);
            ImGui::DragInt("Material", &shape.MaterialIndex, 1.0f, 0, (int)scene.Materials.size() - 1);

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::PushID("Sphere");
        ImGui::Button("Add Sphere");
        if (ImGui::IsItemClicked())
        {
            // Sphere sphere = Sphere();
            std::shared_ptr<Shape> sphere = std::make_shared<Sphere>();
            scene.Shapes.push_back(sphere);
        }
        ImGui::PopID();
        ImGui::PushID("Plane");
        ImGui::Button("Add Plane");
        if (ImGui::IsItemClicked())
        {
            // Plane plane = Plane();
            std::shared_ptr<Shape> plane = std::make_shared<Plane>();
            scene.Shapes.push_back(plane);
        }
        ImGui::PopID();
        ImGui::Separator();

        ImGui::Text("Lights");
        ImGui::PushID("Lights");
        ImGui::Text("Ambient Light");
        ImGui::ColorEdit3("Ambient Light", glm::value_ptr(scene.AmbientLight));
        ImGui::DragFloat("Ambient Intensity", &scene.AmbientIntensity, 0.1f);
        ImGui::BeginGroup();
        for (size_t i = 0; i < scene.Lights.size(); i++)
        {
            ImGui::Text("Light %d", i);
            ImGui::PushID(i);

            Light &light = scene.Lights[i];
            ImGui::DragFloat3("Position", glm::value_ptr(light.Position), 0.1f);
            ImGui::ColorEdit3("Color", glm::value_ptr(light.Color));
            ImGui::DragFloat("Intensity", &light.Intensity, 0.1f);

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::EndGroup();
        ImGui::Button("Add Light");
        if (ImGui::IsItemClicked())
        {
            Light light;
            light.Position = {0.0f, 0.0f, 0.0f};
            light.Color = {1.0f, 1.0f, 1.0f};
            scene.Lights.push_back(light);
        }
        ImGui::PopID();
        ImGui::Text("Materials");
        for (size_t i = 0; i < scene.Materials.size(); i++)
        {
            ImGui::PushID(i);

            Material &material = scene.Materials[i];
            ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
            ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Specular", &material.Specular, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Shininess", &material.Shininess, 0.05f, 0.0f, 100.0f);

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::End();

        // Renderiza la ventana de ImGui
        ImGui::Begin("Settings");
        {
            ImGui::Checkbox("Accumulate", &renderer->GetSettings().Accumulate);

            if (ImGui::Button("Reset"))
                renderer->ResetFrameIndex();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Text("Mouse Position: (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
        }
        ImGui::End();

        ImGui::Begin("Scene");
        {
            renderer->OnResize(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
            renderer->Update(io.Framerate);
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
