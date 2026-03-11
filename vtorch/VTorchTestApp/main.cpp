#include <cstdlib>
#include <gsl/gsl>

#include "LoggingLib/Logging.h"

int main()
{
    logging::Init();
    const auto shutdown = gsl::finally([] { logging::Shutdown(); });
    return EXIT_SUCCESS;
}
