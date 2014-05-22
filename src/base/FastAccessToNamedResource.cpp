#include "FastAccessToNamedResource.h"
#include "Logger.h"

namespace PS {

void Logging::LogArg1(const char *message, const char *arg1) {
    LogErrorArg1(message, arg1);
}

}
