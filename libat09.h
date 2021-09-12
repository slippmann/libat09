#ifndef LIBAT09_H
#define LIBAT09_H

#include "libat09config.h"

#include <Arduino.h>

#ifdef DEBUG
	#define BUFFER_SIZE 70
#else
	#define BUFFER_SIZE 16
#endif

namespace at09
{
	const long baudRates[] = {
		2400,
		4800,
		9600,
		19200,
		38400,
		57600,
		115200
	};

	enum class BTRole
	{
		PERIPHERAL = 0,
		CENTRAL = 1
	};

	class AT09
	{
	private:
		// Constants
		static constexpr unsigned int AT09_RESPONSE_TIMEOUT_MS = 1000;

		bool initialized = false;
		char baudRate = 0;
		char statePin = 0;
		char at09Response[BUFFER_SIZE] = {0};
		unsigned int responseIndex = 0;
		Stream * serial;

#ifdef DEBUG
		char rxPin = 0;
		char txPin = 0;
		char debugCommand[BUFFER_SIZE] = {0};
		unsigned int debugIndex = 0;
#endif

		void findBaudRate();
		bool isResponseValid();
		bool sendAndWait(const char * message, bool waitForOk = false);
		bool setCommand(const char * commandName, const char * value);
		bool setCommand(const char * commandName, long value);
		bool getCommand(const char * commandName, char * resultBuffer, int bufferLen);
		
	public:

#ifdef DEBUG
		AT09(char rx, char tx, unsigned long baudRate = 0);
		void HandleSerialRelay();
#else
		AT09 Initialize(unsigned long baudRate = 0);
#endif

		bool Disconnect();
		bool IsConnected();
		bool Reboot()								{ return sendAndWait("RESET"); }
		bool FactoryReset()							{ return sendAndWait("DEFAULT"); }
		bool SendHello()							{ return sendAndWait("AT"); }

#ifdef GETTERS
		bool GetBluetoothAddress(char * addressBuffer, int addressBufferLen)	{ return getCommand("LADDR", addressBuffer, addressBufferLen); }
		bool GetFWVersion(char * versionBuffer, int versionBufferLen)			{ return getCommand("VERSION", versionBuffer, versionBufferLen); }
#endif

#ifdef SETTERS
		bool SetAdvertismentEnable(bool isEnabled)	{ return setCommand("ADVEN", (int)isEnabled); }
		bool SetAdvertisingInterval(char interval)	{ return setCommand("ADVI", interval); }
		bool SetConnectionInterval(char interval)	{ return setCommand("NINTERVAL", interval); }
		bool SetBaud(long baud)						{ return setCommand("BAUD", baud); }
		bool SetName(const char * btName)			{ return setCommand("NAME", btName); }
		bool SetRole(BTRole role)					{ return setCommand("ROLE", (int)role); }
		bool SetServiceUUID(int uuid)				{ return setCommand("UUID", uuid); }
		bool SetCharacteristicUUID(int uuid)		{ return setCommand("CHAR", uuid); }
		bool SetPowerLevel(char level)				{ return setCommand("POWE", level); }
#endif

		bool Sleep();
	}; /* class AT09 */
} /* namespace at09 */
#endif //LIBAT09_H
