#include <SPI.h>
#include <SD.h>


#define SD_CS 53  //CS Pin for SD card this is different from 2.8 inch TFT LCD
File myFile;
String buffer;
String title;
String fname;

void SD_Init(void) {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  unsigned long previousSDMillis = millis();
  if (!SD.begin(SD_CS)) {
    Serial.print(F("cannot start SD"));
    while (millis() - previousSDMillis <= 7000)
      //    while(1)
      ;
  } else {
    Serial.print(F("SD Init OK"));
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  fname = __DATE__ ".csv";
}

void logData() {

  // make a string for assembling the data to log:
  String dataString = "";
  /*
  String date = dateTimeString.substring(0, 2);
  String month = dateTimeString.substring(3, 5);
  String year = dateTimeString.substring(6, 10);
  String hours = dateTimeString.substring(11, 13);
  String minutes = dateTimeString.substring(14, 16);
  String seconds = dateTimeString.substring(17, 19);

  String fileName = year + month + date;
*/
  /*
  // add hours, minutes, Temperature, Humidity, Pressure
  for (int i = 0; i < 5; i++) {
    dataString = hours + minutes + seconds + "," + Temperature[1] + "," + Temperature[2] + "," + Temperature[3] + "," + Temperature[4];
  }
  */

  String fileName = "DataFile";
  dataString = "123456";
  //  dataString = hours + minutes + seconds + "," + Temperature[1] + "," + Temperature[2] + "," + Temperature[3] + "," + Temperature[4];
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(fileName, FILE_WRITE);
  //  File dataFile = SD.open("fileName", FILE_WRITE);


  // if the file is available, write to it:
  if (myFile) {
    myFile.println(dataString);
    myFile.close();
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
  SD_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  logData();
}
