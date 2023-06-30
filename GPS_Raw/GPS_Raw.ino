#include <SoftwareSerial.h>

//SoftwareSerial soft(10, 11); // Rx, Tx (Rx to Tx of Microcontroller, Tx to Rx of Microcontroller)
SoftwareSerial soft(4, 3);// soft(rx, tx)

// Globals
String utc = "";  // hhmmss.00
String lat = "";  // DDMM.MMMM N/S
String lon = "";  // DDMM.MMMM E/W
String alt = "";  // x meters

void setup() {
  Serial.begin(9600);
  soft.begin(9600);
}

void loop() {

  String msg = soft.readStringUntil('\r');
  Serial.print(msg);

/*
   // Poll GPS for data
  getGPS();

  // Print out time and the 'tudes
  Serial.print("Time: ");
  Serial.println(utc);
  Serial.print("Latitude: ");
  Serial.println(lat);
  Serial.print("Longitude: ");
  Serial.println(lon);
  Serial.print("Altitude: ");
  Serial.println(alt);
  Serial.println();
*/
}

void getGPS() {

  int index;
  
  // Look for $GPGGA
  
  if ( soft.find("$GNGGA,") ) {

    // Parse message for time and position
    utc = soft.readStringUntil(',');
    lat = soft.readStringUntil(',');
    lat.concat(soft.readStringUntil(','));
    lon = soft.readStringUntil(',');
    lon.concat(soft.readStringUntil(','));

    // Flush data up until altitude
    for ( int i = 0; i < 3; i++ ) {
      soft.readStringUntil(',');
    }

    // Parse altitude data
    alt = soft.readStringUntil(',');
    alt.concat(soft.readStringUntil(','));

    // Flush rest of message
    soft.readStringUntil('\r');
  }
}

