// MCUFRIEND UNO shields have microSD on pins 10, 11, 12, 13
// The official <SD.h> library only works on the hardware SPI pins
// e.g. 11, 12, 13 on a Uno
// e.g. 50, 51, 52 on a Mega2560
// e.g. 74, 75, 76 on a Due
//
// if you are not using a UNO,  you must use Software SPI:
//
// install v1.0.1 of the <SdFat.h> library with the Arduino Library Manager.
// edit the src/SdFatConfig.h file to #define ENABLE_SOFTWARE_SPI_CLASS 1
//
// copy all your BMP files to the root directory on the microSD with your PC
// (or another directory)

#include <SPI.h>             // f.k. for Arduino-1.5.2
#define USE_SDFAT
#include <SdFat.h>           // Use the SdFat library
SdFatSoftSpi<12, 11, 13> SD; //Bit-Bang on the Shield pins (MISO, MOSI, SCK)

#include <Adafruit_GFX.h>    // Hardware-specific library
#include <MCUFRIEND_kbv.h>
#include "usergraphics.h"
#include <TouchScreen.h>
/*
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
*/
#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define MIN_TEMPERATURE -25
#define MAX_TEMPERATURE -20

#define SD_CS     10  //CS Pin for SD card
#define NAMEMATCH ""         // "" matches any name
//#define NAMEMATCH "tiger"    // *tiger*.bmp
#define PALETTEDEPTH   8     // support 256-colour Palette

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define screen_mode 2  //0= portrait, 1= landscape, 2 = inverted portrait, 3= inverted landscape

/*
enum 
{     
  
  CURRENT_TEMP,  // measured room temp from external sensor
  SET_TEMP,   // set-point temperature by user, 
  TOTAL_REGS_SIZE
}; 

float holdingRegs[TOTAL_REGS_SIZE]; // function 3 and 16 register array 
*/
/* Change these values to set the current initial time */
String seconds = "0";
String minutes = "30";
String hours = "10";

/* Change these values to set the current initial date */
String date = "9";
String month = "11";
String year = "22";

String dateMothYear;
String hoursMinutes;

int X,Y;
int iSet_Temperature = -21;
float iCurrent_Temperature = -22.3;

const int XP = 8, XM = A2, YP = A3, YM = 9; //ID=0x9341
const int TS_LEFT = 132, TS_RT = 913, TS_TOP = 102, TS_BOT = 906;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
bool Touch_pressed = false;

Adafruit_GFX_Button on_btn, off_btn;
int pixel_x, pixel_y;     //Touch_getXY() updates global vars

MCUFRIEND_kbv tft;

char namebuf[32] = "/";   //BMP files in root directory
//char namebuf[32] = "/bitmaps/";  //BMP directory e.g. files in /bitmaps/*.bmp

File root;
int pathlen;
bool auto_selection = true;
bool on_selection = false;
 uint8_t iCurrent_temperature = 21;
 uint8_t iSet_temperature = 20;

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

void setup()
{
/*  // Init MODBUS registers
  holdingRegs[CURRENT_TEMP] = iCurrent_temperature;
  holdingRegs[SET_TEMP] = iSet_temperature;
*/
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
    tft.setTextColor(0xFFFF, 0x0000);
    tft.setTextSize(2);
    //tft.setFont(&FreeSans9pt7b);

    on_btn.initButton(&tft,  60, 300, 100, 40, WHITE, GREEN, BLACK, "Auto", 2);
    off_btn.initButton(&tft, 180, 300, 100, 40, WHITE, RED, BLACK, "Off", 2);
    on_btn.drawButton(false);
    off_btn.drawButton(false);
    
    tft.setCursor(10, 10);
    tft.setTextSize(2);
    tft.setTextColor(BLACK);
    dateMothYear = date + "/" + month + "/" + year;
    tft.print(dateMothYear);

    tft.setCursor(170, 10);
    tft.setTextSize(2);
    tft.setTextColor(BLACK);
    hoursMinutes = hours + ":" + minutes;
    tft.print(hoursMinutes);

    draw_Big_circle();
    draw_Small_circle();
    draw_up_button();
    draw_down_button();
   
    bool good = SD.begin(SD_CS);
    if (!good) {
        Serial.print(F("cannot start SD"));
        while (1);
    }
    root = SD.open(namebuf);
    pathlen = strlen(namebuf);
    
}
TSPoint p;
void loop()
{
  
bool down = Touch_getXY();
    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
    off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
    
    if (on_btn.justPressed()){
      if(auto_selection){
      tft.setTextSize(2);
     
     //   tft.setFont(&FreeSans9pt7b);
        on_btn.initButton(&tft, 60, 300, 100, 40, WHITE, WHITE, BLUE, "Manual", 2);
        on_btn.drawButton();
        auto_selection = false;
      }else{
        
   //     tft.setFont(&FreeSans9pt7b);
         tft.setTextSize(2);
        on_btn.initButton(&tft, 60, 300, 100, 40, WHITE, BLACK, GREEN, "Auto", 2);
        on_btn.drawButton();
        auto_selection = true;
      }
      
    }
    if (off_btn.justPressed()){
      
      if(on_selection){
        tft.setTextSize(2);
  //      tft.setFont(&FreeSans9pt7b);
        off_btn.initButton(&tft, 180, 300, 100, 40, WHITE, WHITE, RED, "Off", 2);
        off_btn.drawButton();
        on_selection = false;
      }else{
        tft.setTextSize(2);
  //      tft.setFont(&FreeSans9pt7b);
        off_btn.initButton(&tft, 180, 300, 100, 40, WHITE, BLACK, YELLOW, "On", 2);
        off_btn.drawButton();
        on_selection = true;
      }
      
    }

    if (on_btn.justReleased()) {
        on_btn.drawButton(true);
    }
    if (off_btn.justReleased()) {
        off_btn.drawButton(true);
    }
//for up arrow

if(down && pixel_x > 190 && pixel_y <60){
  if (iSet_temperature < MAX_TEMPERATURE) iSet_temperature++;
  draw_Small_circle();
  draw_up_button();
  

}
//for down arrow

if(down && pixel_x >190 && pixel_y >130 && pixel_y <150){
  if (iSet_temperature > MIN_TEMPERATURE) iSet_temperature--;
  draw_Small_circle();
  draw_down_button();
}


}

void draw_Big_circle()
{

  //draw big circle 
  unsigned char i;
  if (iCurrent_temperature < iSet_temperature) {
    // heating - red
    for(i=0; i < 10; i++) tft.drawCircle(120, 180, 80 + i, BLUE);
  } else if (iCurrent_temperature > iSet_temperature) {
    // cooling - blue
    for(i=0; i < 10; i++) tft.drawCircle(120, 180, 80 + i, RED);    
  } else {
    // Temperature ok
    for(i=0; i < 10; i++) tft.drawCircle(120, 180, 80 + i, GREEN);       
  }

  tft.setTextColor(BLACK);
  //tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(50, 165);
  tft.setTextSize(4);
  tft.print(iCurrent_Temperature);
}

void draw_Small_circle()
{
  //draw small circle
  tft.fillCircle(180, 90, 40, BLACK);
  tft.setTextColor(WHITE, BLACK);
 // tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(160, 82);
  tft.setTextSize(3);
  tft.print(iSet_Temperature);
  //update_Room_temp();

}

void draw_up_button()
{
  //up button 
  tft.fillTriangle(215,30,230,50,200,50, BLACK);
  
}

void draw_down_button()
{
  //down button
  tft.fillTriangle(215,150,230,130,200,130, BLACK);
}





/********************************************************************//**
 * @brief     Drawing of the main program screen
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_main_screen()
{
  
 
}

/********************************************************************//**
 * @brief     Drawing of the screen for Options menu
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_option_screen()
{
  
}


void dataLogging(){
    // make a string for assembling the data to log:
  String dataString = "";

  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 2) {
      dataString += ",";
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}


