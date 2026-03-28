#pragma once
#include "EventLib/EventBus.h"
#include "RendererLib/IRenderer.h"
#include <memory>
#include <string>

struct ApplicationArgs
{
    std::string name{"App"};
    int width{640};
    int height{480};
    bool resizable{false};
    bool fullscreen{false};
};

struct GLFWwindow;

class Application
{
   public:
    Application(const ApplicationArgs &args);
    virtual ~Application() = default;

    void Run();
    void OnResize(const int width, const int height);

   private:
    void RunGameLoop();

    ApplicationArgs m_appArgs;
    event::SystemBus m_eventBus;

    GLFWwindow *m_window{nullptr};

    bool m_exit{false};

    std::unique_ptr<IRenderer> m_renderer;
};