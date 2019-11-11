#include "libat09.h"

namespace at09
{
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
    this->rxPin = rx;
    this->txPin = tx;    
    this->serial = new SoftwareSerial(rx, tx);

    Serial.begin(115200);

    findBaudRate();
    
  }
#else
  void AT09::Initialize() 
  {
    this->serial = &Serial;
    findBaudRate();
  }
#endif
  
  void AT09::findBaudRate()
  {
    String response;
    bool isBaudFound;
    
#ifdef DEBUG
    SoftwareSerial * port = this->serial;
#else
    HardwareSerial * port = this->serial;
#endif
  
    for(int i = 0; i < 5; i++)
    {
#ifdef DEBUG
      logMessage("Attempting " + baudRates[i] + "...");
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
  
  bool AT09::sendAndWait(String msg) 
  {
    bool isResponseReceived = false;
    unsigned long start;
    unsigned long diff = 0;

    start = millis();
    
    this->serial->println(msg);

    while (diff < AT09_RESPONSE_TIMEOUT_MS)
    {
      if(this->serial->available())
      {
        this->lastResponse = this->serial->readStringUntil('\n');
        isResponseReceived = this->isResponseValid();
        break;
      }

      diff = millis() - start;
    }
  
    return isResponseReceived;
  }

  bool isResponseValid()
  {
    if (this->lastResponse.charAt(0) == '+' ||
        this->lastResponse == "OK")
    {
      return true;
    }

    return false;
  }
  
  void AT09::HandleSerialEvent()
  {
    String received;

#ifdef DEBUG
    // Relay serial to software serial
    if (Serial.available())
    {
      received = Serial.readStringUntil('\n');
      
      logMessage("< " + received);
      
      this->serial->println(received);
    }
#endif
    
    if (this->serial->available())
    {
      this->lastResponse = this->serial->readStringUntil('\n');

#ifdef DEBUG
      logMessage("> " + this->lastResponse);
#endif
    }
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
    this->sendAndWait(commandPrefix);

#ifdef DEBUG
  logMessage("> " + this->lastResponse);
#endif

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
  void logMessage(String message)
  {
    Serial.println(message);
  }
#endif
}
