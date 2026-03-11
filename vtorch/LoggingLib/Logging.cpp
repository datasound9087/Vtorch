#include "Logging.h"

namespace logging {
void Init() { spdlog::info(""); }

void Shutdown() { spdlog::shutdown(); }
} // namespace logging
