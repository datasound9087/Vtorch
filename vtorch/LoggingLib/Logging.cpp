#include "Logging.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace logging
{
    void Init()
    {
        static auto logger = spdlog::stdout_color_mt("logger");
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::info);
        spdlog::set_pattern("[%Y-%m-%dT%H:%M:%S] [%^%l%$] %v");
    }

    void Shutdown() { spdlog::shutdown(); }

} // namespace logging
