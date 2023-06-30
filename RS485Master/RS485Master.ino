#include "RS485_protocol.h"
#include <SoftwareSerial.h>

#include <Adafruit_GFX.h>    // Hardware-specific library
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define screen_mode 2  //0= portrait, 1= landscape, 2 = inverted portrait, 3= inverted landscape
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
int pixel_x, pixel_y;     //Touch_getXY() updates global vars

const int XP = 8, XM = A2, YP = A3, YM = 9; //ID=0x9341
const int TS_LEFT = 132, TS_RT = 913, TS_TOP = 102, TS_BOT = 906;

const byte ENABLE_PIN = 17;
const byte LED_PIN = 13;

SoftwareSerial rs485 (19, 18);  // receive pin, transmit pin

bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
      if(screen_mode == 2){
        pixel_x = map(p.x, TS_LEFT, TS_RT, tft.width(), 0); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, tft.height(), 0);

      }else{
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
      }
       
    }
    return pressed;
}

// RS485 callback routines
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
  uint16_t ID;
  unsigned char i;
  Serial.begin(9600);
  Serial.print("Show BMP files on TFT with ID:0x");
  ID = tft.readID();
  Serial.println(ID, HEX);
  if (ID == 0x0D3D3) ID = 0x9481;

  tft.begin(ID);
  tft.setRotation(screen_mode);            //0 = Portrait, 1 = landscape
  tft.fillScreen(WHITE);

  tft.setCursor(10, 10);
  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.println("Hello World");


  rs485.begin (28800);
  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
  pinMode (LED_PIN, OUTPUT);  // built-in LED
}  // end of setup
  
void loop()
{
  // assemble message
  byte msg [] = { 
     1,    // device 1
     1,    // turn on relay
  };
  // send to slave  
  digitalWrite (ENABLE_PIN, HIGH);  // enable sending
  sendMsg (fWrite, msg, sizeof msg);
  digitalWrite (ENABLE_PIN, LOW);  // disable sending

  // receive response  
  byte buf [12];
  byte received = recvMsg (fAvailable, fRead, buf, sizeof buf);
  
  digitalWrite (LED_PIN, received == 0);  // turn on LED if error    
  
  // only send once per successful change
 if (received){
  byte boxID = buf[0];
  byte humidity = buf[1];

  byte temperature[2]={buf[4],buf[3]};
  float temp = *((float*)temperature);

  byte incoming[2]={buf[6],buf[5]};
  float pressure = *((float*)incoming);
 }
  //tft.setTextColor(BLUE);
  //tft.setTextSize(3);
  //tft.println("Testing");

   



}  // end of loop