/*
  WiFi Web Server

 A simple web server that shows the value of the analog input pins.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * Analog inputs attached to pins A0 through A5 (optional)

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe

 */

#include <SPI.h>
#include <WiFiNINA.h>


#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;              // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

WiFiClient client = server.available();

int red_LED = 2;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(red_LED, OUTPUT);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  enable_WiFi();
  connect_WiFi();

  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    printWeb();
  }
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printWeb() {
  Serial.println("client sent HTTP request");
  // an HTTP request ends with a blank line
  String currentLine = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
      // if you've gotten to the end of the line (received a newline
      // character) and the line is blank, the HTTP request has ended,
      // so you can send a reply
      if (c != '\n' && c != '\r') {
        currentLine += c;
      } else if (c == '\n') {
        if (currentLine.length() == 0) {
          //add codes to build a proper HTTP response and send to client
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          client.println("!DOCTYPE HTML");
          client.println("<h1>Arduino LED Remote Control</h1>");
          client.println("<table border=1 style='text-align:center'>");

          //Red LED
          client.print("<tr><td>Red LED is ");
          if (digitalRead(red_LED)){
            client.print("<font style='color:green;'>ON</font>");
          }else{
            client.print("<font style='color:red;'>OFF</font>");
            
          }
          client.println("</td><td><button type=\"/button\" onclick=\"location.href='/redLED/on'\">ON</button> /<button type=\"/button\" onclick=\"location.href='/greenLED/off'\">OFF</button>");
          client.println("</table>");
          client.println();
          break;
        } else{
          // you're starting a new line
          currentLine = "";
        }
     
      }
      //add codes here to control LEDs based on lines in request


    }
    
  }
  // give the web browser time to receive the data
  delay(1);

  // close the connection:
  client.stop();
  Serial.println("client disconnected");
}
