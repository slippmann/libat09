#ifndef LIBAT09_H
#define LIBAT09_H

#include "libat09config.h"

#include <Arduino.h>

#define SEND_BUFFER_SIZE    16
#define RECEIVE_BUFFER_SIZE 128

namespace at09
{
	typedef enum 
	{
		PERIPHERAL = 0,
		CENTRAL = 1
	} BTRole;

	class AT09
	{
	private:
		bool initialized;
		unsigned char baudRate;
		unsigned char statePin;
		char at09Response[RECEIVE_BUFFER_SIZE];
		Stream * serial;

#ifdef DEBUG
		unsigned char rxPin;
		unsigned char txPin;
#endif

		void findBaudRate();
		bool isResponseValid();
		bool sendAndWait(const char * message);
		bool setParameter(const char * param, char * value);
		bool setParameter(const char * param, unsigned int value);
		bool getParameter(const char * param, char * result, unsigned int resultSize);
		
	public:

		void Initialize(
#ifdef DEBUG
			unsigned char rx, 
			unsigned char tx
#endif
		);

		bool Disconnect();
		void HandleSerialEvent();
		bool IsConnected();
		bool FactoryReset();
		bool Reboot();
		bool SetAdvertismentEnable(bool isEnabled);
		bool SetAdvertisingInterval(unsigned char interval);
		bool SetConnectionInterval(unsigned char interval);
		bool GetBluetoothAddress(char * btAddress, unsigned int bufferLen);
		bool SetBaud(unsigned long baud);
		bool SendHello();
		bool SetName(char * btName);
		bool SetRole(BTRole role);
		bool SetServiceUUID(unsigned int uuid);
		bool SetCharacteristicUUID(unsigned int uuid);
		bool SetPowerLevel(unsigned char level);
		bool Sleep();
	}; /* class AT09 */

	unsigned char baudToCode(unsigned long baud);

#ifdef DEBUG
	void logMessage(const char * format, ...);
#endif

} /* namespace at09 */

#endif /* LIBAT09_H */
