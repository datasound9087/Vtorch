#include "Logging.h"
#include <exception>
#include <format>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace logging
{
    void Init()
    {
        static auto logger = spdlog::stdout_color_mt("logger");
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("[%Y-%m-%dT%H:%M:%S] [%^%l%$] %v");

        LOG_ALL("VTorchTestApp");
    }

    void Shutdown() { spdlog::shutdown(); }

    void ThrowSystemError(const std::string &message, const int error)
    {
        throw std::system_error(std::error_code(error, std::generic_category()),
                                message);
    }
} // namespace logging