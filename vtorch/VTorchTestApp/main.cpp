#include <cstdlib>
#include <gsl/gsl>

#include "ApplicationLib/Application.h"
#include "LoggingLib/Logging.h"

class VTorchTestApp : public Application
{
   public:
    VTorchTestApp()
        : Application({.name = "Vtorch test app", .resizable = true})
    {
    }
    ~VTorchTestApp() = default;
};

int main()
{
    logging::Init();
    const auto shutdown = gsl::finally([] { logging::Shutdown(); });

    try
    {
        VTorchTestApp app{};
        app.Run();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Exception occurred: {}", e.what());
    }
    catch (...)
    {
        LOG_ERROR("Unknown exception occurred :(");
    }

    LOG_ALL("Shutdown");
    return EXIT_SUCCESS;
}
