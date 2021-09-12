#include "libat09utilities.h"

#ifdef DEBUG
    #include <stdarg.h>
#endif

using namespace at09;

bool at09::serialReadLine(Stream& serial,
						  char * buffer,
						  unsigned int bufferLen,
						  unsigned int& index,
						  unsigned int timeout)
{
	int readByte;
	unsigned long start = millis();
	unsigned int maxWriteIndex = bufferLen - 2;
	bool isComplete = false;

	if (index > maxWriteIndex)
	{
		logMessage("ERROR: Index out of bounds.\n");
		buffer[bufferLen - 1] = 0;
		index = 0;
		return false;
	}

	do
	{
		if (serial.available())
		{
			readByte = serial.read();

			// Ignore zeroes
			if (readByte == 0)
				continue;
			
			buffer[index++] = readByte;

			if (readByte == '\n')
			{
				isComplete = true;
				break;
			}
		}
	} while (timeout != 0 &&
			 (millis() - start) < timeout &&
			 index <= maxWriteIndex);

	if (isComplete)
	{
		// Terminate the string and reset the index.
		buffer[index] = 0;
		index = 0;
	}

	return isComplete;
}

#ifdef DEBUG
#define LOG_BUFFER_SIZE 128
void at09::logMessage(const char * format, ...)
{
    va_list argptr;
    char buffer[LOG_BUFFER_SIZE];

    va_start(argptr, format);

    vsnprintf(buffer, LOG_BUFFER_SIZE, format, argptr);

    Serial.print(buffer);

    va_end(argptr);
}
#endif
