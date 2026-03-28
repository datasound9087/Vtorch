#include "Application.h"

#include "EventLib/EventBus.h"
#include "LoggingLib/Logging.h"
#include "VulkanLib/VulkanRenderer.h"
#include <GLFW/glfw3.h>
#include <gsl/gsl>
#include <stdexcept>

namespace
{
    void ResizeFrameBufferCallback(GLFWwindow *window, int width,
                                   int height) noexcept
    {
        auto app =
            reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));

        app->OnResize(width, height);
    }

} // namespace

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

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, ResizeFrameBufferCallback);

    LOG_ALL("Initialising renderer...");
    m_renderer = std::make_unique<VulkanRenderer>(m_window, m_eventBus);
    LOG_ALL("Renderer initialised");

    RunGameLoop();
}

void Application::OnResize(const int width, const int height)
{
    LOG_DEBUG("OnResize");
    events::WindowResize resizeEvent{.width = width, .height = height};
    std::ignore = m_eventBus.publish(resizeEvent);
}

void Application::RunGameLoop()
{
    while (!m_exit)
    {
        m_exit = glfwWindowShouldClose(m_window) == GLFW_TRUE;
        glfwPollEvents();
        m_renderer->RenderFrame();
    }
}