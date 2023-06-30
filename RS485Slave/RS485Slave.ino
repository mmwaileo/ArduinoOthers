#include <SoftwareSerial.h>
#include "RS485_protocol.h"

SoftwareSerial rs485 (19, 18);  // receive pin, transmit pin
const byte ENABLE_PIN = 17;

union cvt{
  byte boxID;
  byte humidity;
  float temp;
  float pressure;
  unsigned char b[12];
} RS485Data;

void fWrite (const byte what)
  {
  rs485.write (what);  
  }
  
int fAvailable ()
  {
  return rs485.available ();  
  }

int fRead ()
  {
  return rs485.read ();  
  }
  
void setup()
{
  rs485.begin (28800);
  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
}

void loop()
{
  byte buf [12];
  byte data =33;
  
  byte received = recvMsg (fAvailable, fRead, buf, sizeof (buf));
  
  if (received)
    {
    if (buf [0] != 1)
      return;  // not my device
    
    RS485Data.boxID =  1;
    RS485Data.humidity= 95;
    RS485Data.temp= -23.5;
    RS485Data.pressure = 14.7;

    if(buf[1]){
      //on relay
    }else{
      //off relay
    }
      
    delay (1);  // give the master a moment to prepare to receive
    digitalWrite (ENABLE_PIN, HIGH);  // enable sending
    sendMsg (fWrite, RS485Data.b, sizeof RS485Data);
    digitalWrite (ENABLE_PIN, LOW);  // disable sending
    
    }
   
}  // end of loop