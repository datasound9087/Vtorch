#include <spdlog/spdlog.h>

namespace logging
{
    void Init();
    void Shutdown();

    [[noreturn]] void ThrowSystemError(const std::string &message,
                                       const int error);
} // namespace logging

// Wrap macros
#define LOG_ALL(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)