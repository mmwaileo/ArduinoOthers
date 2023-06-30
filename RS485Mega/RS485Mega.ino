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
const int XP = 8, XM = A2, YP = A3, YM = 9; //ID=0x9341
const int TS_LEFT = 132, TS_RT = 913, TS_TOP = 102, TS_BOT = 906;
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
int pixel_x, pixel_y;     //Touch_getXY() updates global vars



#define Enable_Pin 17

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

void setup() {
  
  Serial1.begin(9600);//Using Serial1 Port
  Serial.begin(9600);
  pinMode(Enable_Pin, OUTPUT);//DE/RE Controling pin of RS-485

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
}

void loop() {
  char getdata='m';

  digitalWrite(Enable_Pin,HIGH);//DE/RE=HIGH Transmit Enabled M1
  Serial1.print('9');//Write '9' and Fetch Data From UNO
   
  digitalWrite(Enable_Pin,LOW);//DE/RE=LOW Receive Enabled M1
  delay(1000);
  
  if(Serial1.available()){ //If Serial Data is available
    
    while(Serial1.available() && getdata!='d')
    { 
    getdata=Serial1.read();
    Serial.print(getdata);
    }
    
    Serial.println("");
    }

  tft.setCursor(10, 30);
  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.println(getdata);
}