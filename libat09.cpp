#include "libat09config.h"
#include "libat09.h"

#include <Arduino.h>
#include <HardwareSerial.h>

#ifdef DEBUG
	#include <stdarg.h>
	#include <SoftwareSerial.h>
#endif

namespace at09
{
#ifdef DEBUG
	char debugCommand[BUFFER_SIZE];
	unsigned int debugIndex = 0;
#endif

	const int AT09_RESPONSE_TIMEOUT_MS = 1000;

	const long baudRates[] = {
			2400,
			4800,
			9600,
			19200,
			38400,
			57600,
			115200
		};

	const char * commandPrefix = "AT";
	const char * commandBaud = "+BAUD";
	const char * commandName = "+NAME";
	const char * commandRole = "+ROLE";
	const char * commandUuid = "+UUID";
	const char * commandChar = "+CHAR";

#ifdef DEBUG
	void AT09::HandleSerialRelay()
	{
		bool isCommandComplete = false;
		bool isResponseComplete = false;

		if (!initialized)
			return;

		isCommandComplete = serialReadLine(&Serial,
										   debugCommand,
										   &debugIndex,
										   sizeof(debugCommand),
										   0);
		if (isCommandComplete)
		{
			logMessage("< %s", debugCommand);
			serial->print(debugCommand);
			debugCommand[0] = 0;
			debugIndex = 0;
		}

		isResponseComplete = serialReadLine(serial,
											at09Response,
											&responseIndex,
											sizeof(at09Response),
											0);
		if (isResponseComplete)
		{
			logMessage("> %s", at09Response);
			at09Response[0] = 0;
			responseIndex = 0;
		}
	}
#endif

#ifdef DEBUG
	void AT09::Initialize(char rx, char tx)
	{
		rxPin = rx;
		txPin = tx;
		responseIndex = 0;
		serial = new SoftwareSerial(rx, tx);
		Serial.begin(115200);
		findBaudRate();
	}
#else
	void AT09::Initialize() 
	{
		serial = &Serial;
		responseIndex = 0;
		findBaudRate();
	}
#endif
	
	void AT09::findBaudRate()
	{
		String response;
		bool isBaudFound = false;

		initialized = true;

#ifdef DEBUG
		SoftwareSerial * port = (SoftwareSerial *)serial;
#else
		HardwareSerial * port = (HardwareSerial *)serial;
#endif
	
		for (char i = 0; i < 7; i++)
		{
#ifdef DEBUG
			logMessage("Attempting %ld...\n", baudRates[i]);
#endif
			port->begin(baudRates[i]);
			isBaudFound = sendAndWait("AT");


			if (isBaudFound)
			{
#ifdef DEBUG
				logMessage("Found.\n");
#endif
				return;
			}
		}

#ifdef DEBUG
		logMessage("Not found.\n");
#endif
		initialized = false;
	}

	bool AT09::sendAndWait(const char * message) 
	{
		bool isResponseComplete = false;

		if (!initialized)
			return false;

#ifdef DEBUG
		logMessage("< %s\n", message);
#endif
		serial->println(message);

		isResponseComplete = serialReadLine(serial,
											at09Response,
											&responseIndex,
											sizeof(at09Response),
											AT09_RESPONSE_TIMEOUT_MS);

#ifdef DEBUG
		if (at09Response[0])
			logMessage("> %s", at09Response);
#endif
		return (isResponseComplete && isResponseValid());
	}

	bool AT09::isResponseValid()
	{
		bool isValid = (at09Response[0] == '+' ||
										(at09Response[0] == 'O' && 
										 at09Response[1] == 'K'));
		at09Response[0] = 0;
		return isValid;
	}

	bool AT09::IsConnected()
	{
		return false;
	}

	bool SetBaud(long baud)
	{
		return false;
	}
	
	bool AT09::SendHello()
	{
		sendAndWait((const char *)commandPrefix);

		return true;
	}

	bool AT09::SetName(char * btName)
	{
		char command[BUFFER_SIZE];
		snprintf(command, BUFFER_SIZE, "AT+NAME=%s", btName);
		return sendAndWait(command);
	}

	bool AT09::SetRole(BTRole role)
	{
		char command[BUFFER_SIZE];
		snprintf(command, BUFFER_SIZE, "AT+ROLE=%d", (int)role);
		return sendAndWait(command);
	}

	bool AT09::SetServiceUUID(int uuid)
	{
		char command[BUFFER_SIZE];
		snprintf(command, BUFFER_SIZE, "AT+UUID=%d", (int)uuid);
		return sendAndWait(command);
	}

	bool AT09::SetCharacteristicUUID(int uuid)
	{
		char command[BUFFER_SIZE];
		snprintf(command, BUFFER_SIZE, "AT+CHAR=%d", (int)uuid);
		return sendAndWait(command);
	}

	bool serialReadLine(Stream * serial,
						char * buffer,
						unsigned int * index,
						unsigned int bufferLen,
						unsigned int timeout)
	{
		unsigned long start = millis();
		int readByte;
		bool isComplete = false;

		while (timeout == 0 || (millis() - start) < timeout)
		{
			if (serial->available())
			{
				if ((*index) >= (bufferLen - 1)) // Leave room for terminating character
				{
#ifdef DEBUG
					logMessage("ERROR: Index out of bounds.\n");
#endif
					buffer[0] = 0;
					index = 0;
					return false;
				}

				readByte = serial->read();

				// Ignore zeroes
				if (readByte == 0)
					continue;
				
				buffer[(*index)++] = readByte;

				if (readByte == '\n')
				{
					isComplete = true;
					buffer[(*index)] = 0;
					break;
				}
			}
			else if (timeout == 0)
			{
				break;
			}
		}

		return isComplete;
	}

#ifdef DEBUG
#define LOG_BUFFER_SIZE 128
	void logMessage(const char * format, ...)
	{
		va_list argptr;
		char buffer[LOG_BUFFER_SIZE];

		va_start(argptr, format);

		vsnprintf(buffer, LOG_BUFFER_SIZE, format, argptr);

		Serial.print(buffer);

		va_end(argptr);
	}
#endif
}
