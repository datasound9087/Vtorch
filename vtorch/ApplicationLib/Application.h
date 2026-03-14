#pragma once
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

    void Run();

   private:
    void RunGameLoop();

    ApplicationArgs m_appArgs;
    GLFWwindow *m_window{nullptr};

    bool m_exit{false};

    std::unique_ptr<IRenderer> m_renderer;
};