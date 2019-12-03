#include "libat09config.h"
#include "libat09.h"

namespace at09
{
#ifdef DEBUG
  char debugResponse[RX_BUFFER_SIZE];
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
  void AT09::Initialize(char rx, char tx)
  {
    rxPin = rx;
    txPin = tx;    
    serial = new SoftwareSerial(rx, tx);

    Serial.begin(115200);

    findBaudRate();
    
  }
#else
  void AT09::Initialize() 
  {
    serial = &Serial;
    findBaudRate();
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
  
    for(char i = 0; i < 7; i++)
    {
#ifdef DEBUG
      logMessage("Attempting %l...", baudRates[i]);
#endif
      port->begin(baudRates[i]);
      isBaudFound = sendAndWait("AT");

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
  }
  
  bool AT09::sendAndWait(char * message) 
  {
    bool isResponseReceived = false;
    unsigned long start;
    unsigned long diff = 0;

    start = millis();

#ifdef DEBUG
    logMessage("> %s", message);
#endif

    serial->println(message);

    for (int i = 0; i < RX_BUFFER_SIZE && serial->available(); i++)
    {
      at09Response[i] = serial->read();
      
      if ((millis() - start) > AT09_RESPONSE_TIMEOUT_MS)
        return false;
    }

    isResponseReceived = isResponseValid();

#ifdef DEBUG
    logMessage("< %s", at09Response);
#endif

    return isResponseReceived;
  }

  bool AT09::isResponseValid()
  {
    return (at09Response[0] == '+' ||
        strcmp(at09Response, "OK\r") == 0);
  }
  
  void AT09::HandleSerialEvent()
  {
#ifdef DEBUG
    // Relay serial to software serial
    for (int i = 0; Serial.available() && i < RX_BUFFER_SIZE; i++)
    {
      debugResponse[i] = Serial.read();
    }

    logMessage("< %s", debugResponse);
    serial->println(debugResponse);
#endif
    
    for (int i = 0; serial->available() && i < RX_BUFFER_SIZE; i++)
    {
      at09Response[i] = serial->read();
    }

#ifdef DEBUG
    logMessage("> %s", at09Response);
#endif
  }
  
  bool AT09::IsConnected()
  {
    return false;
  }

  bool SetBaud(long baud)
  {

  }
  
  bool AT09::SendHello()
  {
    sendAndWait(commandPrefix);

    return true;
  }
  
  bool AT09::SetName(String btName)
  {
    return false;
  }
  
  bool AT09::SetRole(BTRole role)
  {
    return false;
  }
  
  bool AT09::SetServiceUUID(short uuid)
  {
    return false;
  }
  
  bool AT09::SetCharacteristicUUID(short uuid)
  {
    return false;
  }

#ifdef DEBUG
#define LOG_BUFFER_SIZE 32
  void logMessage(const char * format, ...)
  {
    va_list argptr;
    char buffer[LOG_BUFFER_SIZE] = {'\0'};

    va_start(argptr, format);

    vsnprintf(buffer, LOG_BUFFER_SIZE, format, argptr);

    Serial.println(buffer);

    va_end(argptr);
  }
#endif
}
