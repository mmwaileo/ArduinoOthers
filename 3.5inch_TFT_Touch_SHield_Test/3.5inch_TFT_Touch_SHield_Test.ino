//#include "DEV_Config.h"
#include "LCD_Driver.h"
//#include "LCD_GUI.h"
//#include "LCD_Bmp.h"

#include <SD.h>
#include <SPI.h>

//#include "Debug.h"

#define SD_CS 5

String dateTimeString;  //to store date/month/year from RTC
String date;
String month;
String year;
String hours;
String minutes;
String seconds;
float fCurrent_Temperature = -22.3;
float fCurrent_Pressure = 14.7;
float fCurrent_Humidity = 95;

void SD_Init(void) 
{
  Sd2Card card;
  card.init(SPI_FULL_SPEED, SD_CS);
  if (!SD.begin( SD_CS ))  {
    Serial.print("SD init failed!");
    while (1);                              // init fail, die here
  }
  Serial.print("SD init OK!");
}

uint8_t System_Init(void)
{
  //set Serial
  Serial.begin(115200);
  
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  return 0;
}

void logData() {
  // make a string for assembling the data to log:
  String dataString = "";

  String date = dateTimeString.substring(0, 2);
  String month = dateTimeString.substring(3, 5);
  String year = dateTimeString.substring(6, 10);
  String hours = dateTimeString.substring(11, 13);
  String minutes = dateTimeString.substring(14, 16);
  String seconds = dateTimeString.substring(17, 19);

  String fileName = year + month + date;

  // add hours, minutes, Temperature, Humidity, Pressure
  for (int i = 0; i < 5; i++) {
    dataString = hours + minutes + seconds + "," + fCurrent_Temperature + "," + fCurrent_Humidity + "," + fCurrent_Pressure;
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //   Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening " + fileName);
  }
}

void setup() {
  // put your setup code here, to run once:
  System_Init();

  Serial.println("3.5inch TFT Touch Shield ShowBMP Demo");
  Serial.println("SD_Init ");
  SD_Init();

//  Serial.println("LCD Init ");
//  LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT;
//  LCD_Init(Lcd_ScanDir, 200);

//  Serial.println("LCD_Clear ");
 // LCD_Clear(LCD_BACKGROUND);

 // LCD_ShowBMP();
}

void loop() {
  // put your main code here, to run repeatedly:

}
