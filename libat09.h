#ifndef LIBAT09_H
#define LIBAT09_H

#include "libat09config.h"

#include <Arduino.h>

#ifdef DEBUG
	#define BUFFER_SIZE 512
#else
	#define BUFFER_SIZE 16
#endif

namespace at09
{
	extern const int AT09_RESPONSE_TIMEOUT_MS;
	
	extern const long baudRates[];

	extern const char * commandPrefix;
	extern const char * commandBaud;
	extern const char * commandName;
	extern const char * commandRole;
	extern const char * commandUuid;
	extern const char * commandChar;

	typedef enum 
	{
		PERIPHERAL = 0,
		CENTRAL = 1
	} BTRole;

	class AT09
	{
	private:
		bool initialized;
		char baudRate;
		char statePin;
		char at09Response[BUFFER_SIZE];
		unsigned int responseIndex;
		Stream * serial;

#ifdef DEBUG
		char rxPin;
		char txPin;
#endif

		void findBaudRate();
		bool isResponseValid();
		bool sendAndWait(const char * message);
		
	public:

		void Initialize(
#ifdef DEBUG
			char rx, 
			char tx
#endif
		);

#ifdef DEBUG
		void HandleSerialRelay();
#endif

		bool Disconnect();
		bool IsConnected();
		bool FactoryReset();
		bool Reboot();
		bool SetAdvertismentEnable(bool isEnabled);
		bool SetAdvertisingInterval(char interval);
		bool SetConnectionInterval(char interval);
		long GetBluetoothAddress();
		bool SetBaud(long baud);
		bool SendHello();
		bool SetName(char * btName);
		bool SetRole(BTRole role);
		bool SetServiceUUID(int uuid);
		bool SetCharacteristicUUID(int uuid);
		bool SetPowerLevel(char level);
		bool Sleep();
	}; /* class AT09 */

	bool serialReadLine(Stream * serial,
						char * buffer,
						unsigned int * index,
						unsigned int bufferLen,
						unsigned int timeout);

#ifdef DEBUG
	void logMessage(const char * format, ...);
#endif

} /* namespace at09 */

#endif /* LIBAT09_H */
