#include "libat09config.h"
#include "libat09.h"
#include "libat09utilities.h"

#include <Arduino.h>
#include <HardwareSerial.h>

#ifdef DEBUG
	#include <SoftwareSerial.h>
#endif

using namespace at09;

#ifdef DEBUG
AT09::AT09(char rx, char tx, unsigned long baudRate) :
	rxPin(rx), txPin(tx), responseIndex(0)
{
	serial = new SoftwareSerial(rx, tx);
	Serial.begin(115200);

	initialized = true;

	if (baudRate == 0)
		findBaudRate();
	else
		((SoftwareSerial*)serial)->begin(baudRate);
}
#else
AT09::AT09(unsigned long baudRate) : responseIndex(0)
{
	serial = &Serial;

	initialized = true;

	if (baudRate == 0)
		findBaudRate();
	else
		((HardwareSerial*)serial)->begin(baudRate);
	
}
#endif

void AT09::findBaudRate()
{
	String response;
	bool isBaudFound = false;

#ifdef DEBUG
	SoftwareSerial * port = (SoftwareSerial *)serial;
#else
	HardwareSerial * port = (HardwareSerial *)serial;
#endif

	for (char i = 0; i < sizeof(baudRates) / sizeof(baudRates[0]); i++)
	{
		logMessage("Attempting %ld...\n", baudRates[i]);
		port->begin(baudRates[i]);
		isBaudFound = SendHello();


		if (isBaudFound)
		{
			logMessage("Found.\n");
			return;
		}
	}

	logMessage("Not found.\n");
	initialized = false;
}

bool AT09::sendAndWait(const char * message, bool waitForOk) 
{
	bool isResponseComplete = false;
	char loopCount = 2;

	if (!initialized)
		return false;

	logMessage("< %s\n", message);

	serial->println(message);

	do
	{
		isResponseComplete = serialReadLine(*serial,
											at09Response,
											sizeof(at09Response),
											responseIndex,
											AT09_RESPONSE_TIMEOUT_MS);
		if (waitForOk && isResponseComplete)
		{
			if (at09Response[0] == 'O' &&
			    at09Response[1] == 'K')
			{
				break;
			}
		}
	} while (waitForOk && --loopCount);

	if (at09Response[0])
		logMessage("> %s", at09Response);

	return (isResponseComplete && isResponseValid());
}

bool AT09::setCommand(const char * commandName, const char * value)
{
	char command[BUFFER_SIZE];
	snprintf(command, BUFFER_SIZE, "AT+%s%s", commandName, value);
	return sendAndWait(command, true);
}

bool AT09::setCommand(const char * commandName, long value)
{
	char numberStr[BUFFER_SIZE];
	ltoa(value, numberStr, 10);
	return setCommand(commandName, numberStr);
}

bool AT09::getCommand(const char * commandName, char * resultBuffer, int bufferLen)
{
	char command[BUFFER_SIZE];
	snprintf(command, BUFFER_SIZE, "AT+%s", commandName);

	if (sendAndWait(command))
	{
		if (resultBuffer)
		{
			snprintf(resultBuffer, bufferLen, "%s", at09Response);
		}
		return true;
	}
	else
	{
		logMessage("ERROR: Invalid response");
		return false;
	}
}

bool AT09::isResponseValid()
{
	bool isValid = (at09Response[0] == '+' ||
				    (at09Response[0] == 'O' && 
					 at09Response[1] == 'K'));
	return isValid;
}

bool AT09::IsConnected()
{
	return false;
}

#ifdef DEBUG
void AT09::HandleSerialRelay()
{
	bool isCommandComplete = false;
	bool isResponseComplete = false;

	if (!initialized)
		return;

	isCommandComplete = serialReadLine(Serial,
									   debugCommand,
									   sizeof(debugCommand),
									   debugIndex);
	if (isCommandComplete)
	{
		logMessage("< %s", debugCommand);
		serial->print(debugCommand);
		debugCommand[0] = 0;
		debugIndex = 0;
	}

	isResponseComplete = serialReadLine(*serial,
										at09Response,
										sizeof(at09Response),
										responseIndex);
	if (isResponseComplete)
	{
		logMessage("> %s", at09Response);
		at09Response[0] = 0;
		responseIndex = 0;
	}
}
#endif
