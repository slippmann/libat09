#ifndef LIBAT09_H
#define LIBAT09_H

#include "libat09config.h"
#include <Arduino.h>

#ifdef DEBUG
  #include <stdarg.h>
  #include <SoftwareSerial.h>
#endif

#include <HardwareSerial.h>

#ifdef DEBUG
  #define RX_BUFFER_SIZE 128
#else
  #define RX_BUFFER_SIZE 16
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
    char baudRate;
    char statePin;
    char at09Response[RX_BUFFER_SIZE];
    Stream * serial;

#ifdef DEBUG
    char rxPin;
    char txPin;
#endif

    void findBaudRate();
    bool sendAndWait(char * message);
    bool isResponseValid();
    
  public:

    void Initialize(
#ifdef DEBUG
      char rx, 
      char tx
#endif
    );

    void HandleSerialEvent();
    bool IsConnected();
    bool SetBaud(long baud);
    bool SendHello();
    bool SetName(String btName);
    bool SetRole(BTRole role);
    bool SetServiceUUID(short uuid);
    bool SetCharacteristicUUID(short uuid);
  }; /* class AT09 */

#ifdef DEBUG
  void logMessage(const char * format, ...);
#endif

} /* namespace at09 */

#endif /* LIBAT09_H */
