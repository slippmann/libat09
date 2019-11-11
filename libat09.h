#ifndef LIBAT09
#define LIBAT09

#include "libat09config.h"
#include <Arduino.h>

#ifdef DEBUG
#include <SoftwareSerial.h>
#endif

#include <HardwareSerial.h>

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
    String lastResponse;
    Stream * serial;

#ifdef DEBUG
    char rxPin;
    char txPin;
#endif

    void findBaudRate();
    bool sendAndWait(String message);
    bool isResponseValid();
    
  public:
#ifdef DEBUG
    void Initialize(char rx, char tx);
#else
    void Initialize();
#endif
    void HandleSerialEvent();
    bool IsConnected();
    bool SetBaud(long baud);
    bool SendHello();
    bool SetName(String btName);
    bool SetRole(BTRole role);
    bool SetServiceUUID(short uuid);
    bool SetCharacteristicUUID(short uuid);
  };

#ifdef DEBUG
    void logMessage(String message);
#endif
}

#endif /* LIBAT09 */
