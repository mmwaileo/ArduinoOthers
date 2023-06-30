/**
 * THGadget
 *
 * Temperature and Humidity gadget
 * 
 * Board = ARDUINO MEGA 2560
 * TH Sensor = DHT11
 * RTC = HT1380 
 * TFT Display = SeedStudio TFT_Touch_Shield_V2 
 *
 * Copyright (C) 2017,  Hernan Monserrat hemonserrat<at>gmail<dot>com
 * 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <TimeLib.h>
#include <HT1380.h>
#include <display.h>

#define DHTPIN            22         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11      // DHT 11 
const int PIN_SD_CS = 4;             // pin of sd card

// Uncomment to set RTC time/date for the first time
//#define SET_TIME 1

// Set pins:  CE, IO,CLK
HT1380 rtc = HT1380(24, 25, 26);
DHT_Unified dht(DHTPIN, DHTTYPE);


uint32_t delayMS;
float last_temp;
float last_hum;

/**
 * Peripherals and variable initialization
 */
void setup(void) {
    sensor_t sensor;

    // Debug port
    Serial.begin(9600); 

    // TFT Display
    TFT_BL_ON;      // turn on the background light
    Tft.TFTinit();  // init TFT library

    // Temperature and Humidity Sensor init
    dht.begin();

    // SD Card
    pinMode(PIN_SD_CS,OUTPUT);
    digitalWrite(PIN_SD_CS,HIGH);    
    if(!SD.begin(PIN_SD_CS))              
    { 
        Serial.println("failed!");
        while(1);                               // init fail, die here
    }
    Serial.println("SD OK!");

    // Load background image
    loadBackground();

    // RTC Init
    rtc.init();
    Serial.println("rtc HT1380 now ready!");
#ifdef SET_TIME
    // set current time to RTC instance
    rtc.setHour(21);
    rtc.setMin(54);
    rtc.setSec(0);
    rtc.setYear(47);
    rtc.setMonth(12);
    rtc.setDate(30);
    rtc.setDay(7);
    rtc.setWP(1);
  
    // write the time to HT1380
    rtc.writeBurst();
 #endif

  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  last_temp = 0;
  last_hum = 0;
}

/**
 * Control loop,  read temp, humidity, date and time, check for changes 
 * display the ones that have updates.
 */
void loop(void) {
  char buf[100];
  // put your main code here, to run repeatedly:
  // read current time from HT1380
  rtc.readBurst();  
   
       Tft.fillRectangle(0, 0, 250, 20, BLACK);
       sprintf(buf,"%02u.%02u.%04u\0", rtc.getDate(), rtc.getMonth(), tmYearToCalendar(rtc.getYear()) );
       Tft.drawString(buf,0,0,2,CYAN);

       Tft.fillRectangle(50, 39, 100, 20, BLACK);
       sprintf(buf,"%02u:%02u:%02u\0", rtc.getHour(),rtc.getMin(),rtc.getSec());
       Tft.drawString(buf, 50, 40, 2,CYAN);
 
 // Get temperature event and print its value.
  sensors_event_t temp_event;  
  sensors_event_t hum_event;  
  dht.temperature().getEvent(&temp_event);
  dht.humidity().getEvent(&hum_event);
  if( last_temp!=temp_event.temperature || last_hum!=hum_event.relative_humidity )
  {
    loadBackground();
    last_temp = 0;
    last_hum = 0;   
  }
  
  if( last_temp!=temp_event.temperature)
  { 
    if (isnan(temp_event.temperature)) {
      strcpy(buf, "Error reading temperature!");
    }
    else {
      sprintf(buf, "%d.%01d C", (int)temp_event.temperature, abs((int)(temp_event.temperature*100)%100));
    }
     //Tft.fillRectangle(0, 20, 250, 20, BLACK);
     Tft.drawString(buf, 40, 100, 4,CYAN);  
     last_temp = temp_event.temperature;
  }
  // Get humidity event and print its value.
  if( last_hum!=hum_event.relative_humidity)
  {
    if (isnan(hum_event.relative_humidity)) {
      strcpy(buf,"Error reading humidity!");
    }
    else {
      sprintf(buf, "%d.%01d%%", (int)hum_event.relative_humidity, abs((int)(hum_event.relative_humidity*100)%100));
    }
   //Tft.fillRectangle(0, 40, 250, 20, BLACK);
   Tft.drawString(buf, 40, 200, 4, CYAN);  
   last_hum = hum_event.relative_humidity;
  } 
   delay(delayMS);    
}



