#ifndef LIBAT09_UTILITIES_H
#define LIBAT09_UTILITIES_H

#include "libat09config.h"

#include <Arduino.h>

namespace at09
{
    bool serialReadLine(Stream& serial,
                        char * buffer,
                        unsigned int bufferLen,
                        unsigned int& index,
                        unsigned int timeout = 0);

#ifdef DEBUG
    void logMessage(const char * format, ...);
#else
    void logMessage(const char * format, ...) {}
#endif
}

#endif //LIBAT09_UTILITIES_H
