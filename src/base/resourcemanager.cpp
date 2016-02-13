#include "resourcemanager.h"
#include "logger.h"

namespace ps {

void Logging::LogArg1(const char *message, const char *arg1) {
    LogErrorArg1(message, arg1);
}

}
