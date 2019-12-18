#include "libat09config.h"
#include "libat09.h"

namespace at09
{
#ifdef DEBUG
  char debugResponse[BUFFER_SIZE];
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
  
    for(char i = 0; i < 7; i++)
    {
#ifdef DEBUG
      logMessage("Attempting %ld...", baudRates[i]);
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
    initialized = false;
  }
  
  bool AT09::sendAndWait(char * message) 
  {
    unsigned long start;
    int i = 0;

    if (!initialized)
      return false;

    start = millis();

#ifdef DEBUG
    logMessage("< %s", message);
#endif

    serial->println(message);

    while ((millis() - start) < AT09_RESPONSE_TIMEOUT_MS)
    {
      if (serial->available())
      {
        serial->readBytesUntil('\n', at09Response, BUFFER_SIZE);
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
    bool isValid = (at09Response[0] == '+' ||
                    (at09Response[0] == 'O' && 
                     at09Response[1] == 'K'));
    at09Response[0] = 0;
    return isValid;
  }
  
  void AT09::HandleSerialEvent()
  {
    if (!initialized)
      return;

#ifdef DEBUG
    // Relay serial to software serial
    if (Serial.available())
    {
      Serial.readBytesUntil('\n', debugResponse, BUFFER_SIZE);
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
      serial->readBytesUntil('\n', at09Response, BUFFER_SIZE);
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

  bool SetBaud(long baud)
  {

  }
  
  bool AT09::SendHello()
  {
    sendAndWait(commandPrefix);

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

#ifdef DEBUG
#define LOG_BUFFER_SIZE 32
  void logMessage(const char * format, ...)
  {
    va_list argptr;
    char buffer[LOG_BUFFER_SIZE];

    va_start(argptr, format);

    vsnprintf(buffer, LOG_BUFFER_SIZE, format, argptr);

    Serial.println(buffer);

    va_end(argptr);
  }
#endif
}
