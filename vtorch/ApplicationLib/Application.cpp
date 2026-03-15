#include "Application.h"

#include "LoggingLib/Logging.h"
#include "VulkanLib/VulkanRenderer.h"
#include <GLFW/glfw3.h>
#include <gsl/gsl>
#include <stdexcept>

Application::Application(const ApplicationArgs &args) : m_appArgs(args) {}

void Application::Run()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialise GLFW");
    }
    const auto shutdown = gsl::finally([]() { glfwTerminate(); });

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,
                   m_appArgs.resizable ? GLFW_TRUE : GLFW_FALSE);

    m_window = glfwCreateWindow(m_appArgs.width, m_appArgs.height,
                                m_appArgs.name.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        logging::ThrowSystemError("Failed to create window",
                                  glfwGetError(nullptr));
    }

    LOG_ALL("Initialising renderer...");
    m_renderer = std::make_unique<VulkanRenderer>(m_window);
    LOG_ALL("Renderer initialised");

    RunGameLoop();
}

void Application::RunGameLoop()
{
    while (!m_exit)
    {
        m_exit = glfwWindowShouldClose(m_window) == GLFW_TRUE;
        glfwPollEvents();
    }
}