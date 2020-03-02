//
// Created by Drew on 2019-08-15.
//

#include "src/Application.h"
#include "Utilities/YzLogger.h"
#include "src/Window.h"
#include "src/Vulkan.h"

#include <exception>

namespace Yarezo {

    Application* Application::s_AppInstance = nullptr;
    bool Application::logFPS = false;

    Application::Application() {
        if (s_AppInstance) {
            throw std::runtime_error("Application already exists, Don't be doing that fam");
        }
        s_AppInstance = this;
    }

    Application::~Application() {
    }

    void Application::run() {

        Yarezo::YzLogger::init();
        YZ_INFO("Logger Initialized");

        //Create a window
        WindowProperties props = {800, 600};
        m_Window = Window::createNewWindow(props);

        // Create the vulkan pipeline
        GraphicsDevice_Vulkan vulkanDevice;

        double previousTime = glfwGetTime();
        int frameCount = 0;

        while (!glfwWindowShouldClose(static_cast<GLFWwindow*>(m_Window->getNativeWindow()))) {
            double currentTime = glfwGetTime();
            frameCount++;
            double deltaTime = currentTime - previousTime;
            // Output some fps info every 5s to determine if we nuke performace
            if (deltaTime >= 1.0) {
                if (logFPS) YZ_INFO("FPS: " + std::to_string(frameCount));
                frameCount = 0;
                previousTime = currentTime;
                m_Window->getCamera()->setCameraSpeed(deltaTime * 0.002);
            }

            vulkanDevice.drawFrame();
            m_Window->onUpdate();
        }

        vulkanDevice.waitIdle();

    }
}