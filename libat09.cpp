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
	char debugResponse[RECEIVE_BUFFER_SIZE];
#endif

	const unsigned int AT09_RESPONSE_TIMEOUT_MS = 1000;

	const unsigned int MAC_ADDRESS_LEN = 11;

	const unsigned char NUM_BAUD_RATES = 7;
	const char BAUD_CODE_OFFSET = 2; // Codes 0 and 1 both correspond to 9600 baud.
	const unsigned long baudRates[NUM_BAUD_RATES] = {
			//9600	// AT09 Code 0
			//9600	// AT09 Code 1
			2400, 	// AT09 Code 2
			4800,	// AT09 Code 3
			9600,	// AT09 Code 4
			19200,	// AT09 Code 5
			38400,	// AT09 Code 6
			57600,	// AT09 Code 7
			115200	// AT09 Code 8
		};

#ifdef DEBUG
	void AT09::Initialize(unsigned char rx, unsigned char tx)
	{
		rxPin = rx;
		txPin = tx;
		serial = new SoftwareSerial(rx, tx);
		serial->setTimeout(AT09_RESPONSE_TIMEOUT_MS);

		Serial.begin(115200);
		Serial.setTimeout(AT09_RESPONSE_TIMEOUT_MS);

		findBaudRate();
	}
#else
	void AT09::Initialize() 
	{
		serial = &Serial;
		serial.setTimeout(AT09_RESPONSE_TIMEOUT_MS);
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
			logMessage("Attempting %ld...", baudRates[i]);
#endif
			port->begin(baudRates[i]);
			isBaudFound = SendHello();

			if (isBaudFound)
			{
#ifdef DEBUG
				logMessage("Found.");
#endif
				return;
			}
		}

#ifdef DEBUG
		logMessage("Not found.");
#endif
		initialized = false;
	}

	bool AT09::sendAndWait(const char * message) 
	{
		unsigned long start;
		int numBytesRead = 0;
		int i = 0;

		if (!initialized)
			return false;

		at09Response[0] = 0; // Reset response buffer

		start = millis();

#ifdef DEBUG
		logMessage("< %s", message);
#endif

		serial->println(message);

		while ((millis() - start) < AT09_RESPONSE_TIMEOUT_MS)
		{
			if (serial->available())
			{
				numBytesRead = serial->readBytesUntil('\n', at09Response, sizeof(at09Response));
				at09Response[numBytesRead] = 0; // Terminate string
				break;
			}
		}

#ifdef DEBUG
		if (at09Response[0])
			logMessage("> %s", at09Response);
#endif
		return isResponseValid();
	}

	bool AT09::isResponseValid()
	{
		return (at09Response[0] == '+' ||
				(at09Response[0] == 'O' && 
				 at09Response[1] == 'K'));
	}

	void AT09::HandleSerialEvent()
	{
		int numBytesRead = 0;

		if (!initialized)
			return;

#ifdef DEBUG
		// Relay serial to software serial
		if (Serial.available())
		{
			numBytesRead = Serial.readBytesUntil('\n', debugResponse, sizeof(debugResponse));
			debugResponse[numBytesRead] = 0; // Terminate string
		}

		if (debugResponse[0])
		{
			logMessage("< %s", debugResponse);
			serial->println(debugResponse);
			debugResponse[0] = 0;
		}

#endif

		if (serial->available())
		{
			numBytesRead = serial->readBytesUntil('\n', at09Response, sizeof(at09Response));
			at09Response[numBytesRead] = 0; // Terminate string
		}

#ifdef DEBUG
		if (at09Response[0])
			logMessage("> %s", at09Response);
			at09Response[0] = 0;
#endif
	}

	bool AT09::IsConnected()
	{
		return false;
	}
	
	bool AT09::SendHello()
	{
		const char * commandHello = "AT";
		return sendAndWait(commandHello);
	}

	const char * commandBaud = "BAUD";

	bool AT09::setParameter(const char * param, char * value)
	{
		char command[SEND_BUFFER_SIZE];
		snprintf(command, sizeof(command), "AT+%s%s", param, value);
		return sendAndWait(command);
	}

	bool AT09::setParameter(const char * param, unsigned int value)
	{
		char command[SEND_BUFFER_SIZE];
		snprintf(command, sizeof(command), "AT+%s%u", param, value);
		return sendAndWait(command);
	}

	bool AT09::getParameter(const char * param, char * result, unsigned int resultSize)
	{
		bool isResponseValid = false;
		unsigned int dataOffset = 0;
		char command[SEND_BUFFER_SIZE];
		snprintf(command, sizeof(command), "AT+%s", param);
		isResponseValid = sendAndWait(command);
		
		if (isResponseValid)
		{
			for (dataOffset = 0; dataOffset < strlen(at09Response); dataOffset++)
			{
				if (at09Response[dataOffset] == '=' ||
				    at09Response[dataOffset] == ':')
					break;
			}

			if (dataOffset >= strlen(at09Response))
				return false;

			snprintf(result, resultSize, "%s", &at09Response[dataOffset + 1]);
		}

		return isResponseValid;
	}

	bool AT09::SetBaud(unsigned long baud)
	{
		const char * commandBaud = "BAUD";
		unsigned char baudCode = baudToCode(baud);
		return setParameter(commandBaud, (unsigned int)baudCode);
	}

	bool AT09::SetName(char * btName)
	{
		const char * commandName = "NAME";
		return setParameter(commandName, btName);
	}

	bool AT09::SetRole(BTRole role)
	{
		const char * commandRole = "ROLE";
		return setParameter(commandRole, (unsigned int)role);
	}

	bool AT09::SetServiceUUID(unsigned int uuid)
	{
		const char * commandUuid = "UUID";
		return setParameter(commandUuid, (unsigned int)uuid);
	}

	bool AT09::SetCharacteristicUUID(unsigned int uuid)
	{
		const char * commandChar = "CHAR";
		return setParameter(commandChar, (unsigned int)uuid);
	}

	bool AT09::SetAdvertismentEnable(bool isEnabled)
	{
		const char * commandAdvEn = "ADVEN";
		return setParameter(commandAdvEn, (unsigned int)isEnabled);
	}

	bool AT09::SetAdvertisingInterval(unsigned char interval)
	{
		const char * commandAdvInt = "ADVI";
		return setParameter(commandAdvInt, (unsigned int)interval);
	}

	bool AT09::SetConnectionInterval(unsigned char interval)
	{
		const char * commandConnInt = "NINTERVAL";
		return setParameter(commandConnInt, (unsigned int)interval);
	}

	bool AT09::GetBluetoothAddress(char * btAddress, unsigned int bufferLen)
	{
		const char * commandBtAddr = "LADDR";
		return getParameter(commandBtAddr, btAddress, bufferLen);
	}

	unsigned char baudToCode(unsigned long baud)
	{
		for (int i = 0; i < NUM_BAUD_RATES; i++)
		{
			if (baudRates[i] == baud)
				return i + BAUD_CODE_OFFSET;
		}

#ifdef DEBUG
		logMessage("Invalid baud rate: %ul.");
#endif
		return 255;
	}

#ifdef DEBUG
#define LOG_BUFFER_SIZE 128
	void logMessage(const char * format, ...)
	{
		va_list argptr;
		char buffer[LOG_BUFFER_SIZE];

		va_start(argptr, format);

		vsnprintf(buffer, sizeof(buffer), format, argptr);

		Serial.println(buffer);

		va_end(argptr);
	}
#endif
}
