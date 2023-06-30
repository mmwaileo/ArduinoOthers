/* tft Demo for Instructables
    May 2019 - Gord Payne
    Uses MCUfriend.com 2.4" TFT display shield for Uno
*/
#define MINPRESSURE 10 // set pressure thresholds for touchscreen
#define MAXPRESSURE 1000
#include <Adafruit_GFX.h> // graphics library
#include <MCUFRIEND_kbv.h> // screen control library
MCUFRIEND_kbv tft; // define tft object

// configure the Analog ports
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin


// MCUFRIEND UNO shield shares pins with the TFT.
#define YP A1   //A3 for ILI9320
#define YM 7    //9
#define XM A2
#define XP 6    //8 

#include <TouchScreen.h>
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // define touch object
TSPoint tp; //Touchscreen_due branch uses Point
// Define some colours for use in screens
#define  BLACK   0x0000
#define  BLUE    0x001F
#define  GREEN   0x07E0
#define  YELLOW  0xFFE0
#define  WHITE   0xFFFF
#define  CHERRY   0xF020
// for help defining colours, go to: http://www.barth-dev.de/online/rgb565-color-picker/
/* click on the colour square and pick your colour. Then lookunder the RGB565 heading
    and you'll get the hex byte representation for that colour
*/

String cString = "qwerty";// characters for keyboard

char  letter[] = {'q', 'w', 'e', 'r', 't', 'y'};// letters for the keyboard app
int letX[] =     { 194, 291, 402, 516, 629, 739}; // the x and y coordinates for the letters on the keyboard
int letY[] =     { 316, 316, 316, 316, 316, 316};// QWERTY only provided

int funcX[] = {527, 527}; // x and y coordinates for two MENU buttons
int funcY[] = {632, 393};
int func[] = {1, 2}; // '1' is Saucy '7' Ball, '2' is the keyboard app

String msg = ""; // message string to build from keyboard app
long lastTouch = millis();// last touch time for minimum delay between touches
long tThresh = 200;// minimum time between touches
int mode = 0;// current display function - starts with 0 - menu screen
int tMode = 0;// the current touch mode - starts with 0 - menu screen

void setup()
{
  Serial.begin(9600);
  digitalWrite(A0, HIGH);
  pinMode(A0, OUTPUT);
  uint16_t ID = tft.readID(); // get the screen's ID
  tft.begin(ID);// assign the ID to the screen object
  tft.setRotation(0);// LANDSCAPE MODE = 1, Portrait mode = 0
  randomSeed(analogRead(A5));// use this for the Saucy '7' Bball app
  splash(); // display the opening screen for your sketch

}

void loop() {
    switch (mode) { // what app is currently being displayed on the screen
      case 0:
        tMode = 0; // set the current touch mode to be monitored
        menuScreen(); // call your display method for the current mode
        mode = 9;// change to unused mode so screen doesn't keep redrawing endlessly
        break;
      case 1:
        tMode = 1;
       sevenScreen();
        mode = 9;
        break;
    }
  
    switch (tMode) { // which app is currently being monitored for touches
      case 0:
        readMenuTouch(); // watch for menu touches
        break;
      case 1:
        read7Touch(); // watch for keyboard touches
        break;
      default:
        backToMenu();
        break;
    }


}

///////////////////////////

void splash() {// Startup splash screen
  // DISPLAY x and y coordinates are measured from the top left of the screen in PORTRAIT MODE
  tft.fillScreen(WHITE);
  tft.setTextSize(5);
  tft.setCursor(80, 40);
  tft.setTextColor(BLUE);
  tft.print("TFT");
  tft.setTextSize(2);
  tft.setCursor(50, 140);
  tft.setTextColor(CHERRY);
  tft.print("Instructable");
  tft.setTextSize(3);
  tft.setCursor(90, 200);
  tft.setTextColor(BLACK);
  tft.print("Demo");

  delay(2500);// duration for the splash screen to remain displayed
}

void showTouch() {// diagnostic method to display touch locations on screen

  // with the reset button at the top of the board, the touches
  //are oriented with the x,y origin in the LOWER LEFT CORNER
  if (millis() - lastTouch > tThresh) { // if it's been long enough since last touch
    TSPoint p = ts.getPoint(); // get the screen touch

    if ((p.z > MINPRESSURE && p.z < MAXPRESSURE)) { // if it's a valid touch
      pinMode(YP, OUTPUT);  //restore the TFT control pins
      pinMode(XM, OUTPUT);// for display after detecting a touch
     // tft.fillScreen(BLUE);
      tft.fillRect(70, 80, 100, 30, WHITE);// erase previously displayed coordinates
      tft.setTextSize(2);
      tft.setTextColor(BLACK);
      tft.setCursor(80, 85);// top left corner of text
      tft.print(p.x);
      tft.print(",");
      tft.print(p.y);
      // ***remember TOUCH coordinates are not the same as DISPLAY COORDINATES
      if (abs(p.x - 236) < 70 && abs(p.y - 117 < 20)) {// menu button
        Serial.println("menu press");
      }
    }

    lastTouch = millis();// reset lastTouch for the next touch event
  }
}

void menuScreen() {// draw the main menu screen
  pinMode(YP, OUTPUT);  //restore the TFT control pins
  pinMode(XM, OUTPUT);// for display after detecting a touch
  tft.fillScreen(BLUE);
  tft.setFont();
  tft.setTextSize(2);
  tft.setCursor(30, 10);
  tft.setTextColor(WHITE);
  tft.setCursor(65,25);
  tft.print("Main Menu");
  int xVal = 80;// starting x and y coordinates for menu buttons
  int yVal = 80;
  // order of parameters is (x,y, width, height, colour)
  // optionally, colour can be expressed as r,g,b with numbers from 0 to 255 for each colour
  tft.fillRect(xVal, yVal, 80, 80, WHITE  );
  tft.fillRect(xVal, yVal + 100, 80, 80, WHITE);

  // draw icons
  drawSevenIcon(0);
  drawTextIcon();
}


void readMenuTouch() {// which menu option was pressed?
  if (millis() - lastTouch > tThresh) {
    TSPoint p = ts.getPoint();

    if ((p.z > MINPRESSURE && p.z < MAXPRESSURE)) {
      int theFunc = getFunc(p.x, p.y);
      mode = theFunc; // set the selected mode
    }
    lastTouch = millis();
  }
}
int getFunc(int x, int y) { // which function matches the selected option?
  int theFunc;
  for (int i = 0; i < 2; i++) {
    if ((abs(x - funcX[i]) <= 100) && (abs(y - funcY[i]) <= 80)) {
      theFunc = func[i];
    }
  }
  return theFunc;
}


void backToMenu() { // return to menu screen when Home button pressed
  if (millis() - lastTouch > tThresh) {
    TSPoint p = ts.getPoint();
    if ((p.z > MINPRESSURE && p.z < MAXPRESSURE)) {
      if (abs(p.x - 236) < 70 && abs(p.y - 117 < 20)) {
        mode = 0;
      }
    }
  }
}

void drawTextIcon() {
  tft.setCursor(85, 210);
  tft.setTextColor(BLUE);
  tft.setTextSize(2);
  tft.print("QWERTY");

  tft.setCursor(90, 245);
  tft.setTextColor(BLACK);
  tft.setTextSize(0);
  tft.print("Text Entry");
}

void drawSevenIcon(int yAdj) {
  draw7Ball(yAdj);
  tft.setTextSize(0);
  tft.setCursor(85, 145);
  tft.print("Saucy 7 Ball");
}

void draw7Ball(int yAdj) {// yAdj parameter is for adjusting vertical alignment for App screen
  tft.fillCircle(120, 110+yAdj, 23, CHERRY);
  tft.fillCircle(120, 103 + yAdj, 10, WHITE);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(115, 96 + yAdj);
  tft.print("7");
}

void read7Touch() {// for this app, pressing ANYWHERE on the screen will activate the response
  char*  response[] = {"You're JOKING!",// array of response strings
                       "Not advisable",
                       "ARE YOU CRAZY?",
                       "  I LOVE IT!",
                       "     no...",
                       "   no way...",
                       "YES! Do it NOW!"
                      };

  if (millis() - lastTouch > tThresh) {// if genuine touch anywhere on screen
    TSPoint p = ts.getPoint();

    if ((p.z > MINPRESSURE && p.z < MAXPRESSURE)) {

      if (abs(p.x - 236) < 70 && abs(p.y - 117 < 20)) {// return to main menu

        mode = 0;
      }
      else {
        int choice = (int)random(1, 7);// generate a random choice
        show7Response(response[choice]); //call method to display that choice on screen
      }
    }
    lastTouch = millis();
  }

}

void sevenScreen() {
  pinMode(YP, OUTPUT);  //restore the TFT control pins
  pinMode(XM, OUTPUT);// for display after detecting a touch
  tft.fillScreen(BLACK);
  tft.setFont();// back to standard font
  tft.setTextSize(2);
  tft.setCursor(15, 10);
  tft.setTextColor(WHITE);
  tft.print("The Saucy 7 Ball!");
  draw7Ball(-35);// draw 7 ball higher up on screen
  sevenInstr();// show the app instructions

}

void sevenInstr() {
  tft.setTextColor(WHITE);
  tft.setCursor(20, 250);
  tft.print("Ask your question,");
  tft.setCursor(25, 280);
  tft.print("press 'Respond'");
  tft.fillRect(60, 180,120, 40, CHERRY  );// draw the Respond button
  tft.setCursor(77, 190);
  tft.setTextColor(BLACK);
  tft.print("Respond");
}

void show7Response(String theMsg) {
  pinMode(YP, OUTPUT);  //restore the TFT control pins
  pinMode(XM, OUTPUT);// for display after detecting a touch
  tft.fillRect(0, 250, 240, 50, BLACK);

  for (int j = 1; j <= 3; j++) {
    tft.setCursor(40, 145);
    tft.setTextColor(BLUE);
    tft.print("thinking");
    for (int i = 1; i <= 4; i++) {
      delay(350);
      tft.print(".");
    }
    tft.fillRect(0, 130, 240, 38, BLACK);
    delay(250);
  }
  tft.setTextColor(YELLOW);
  tft.setCursor(30, 133);
  tft.print(theMsg); // show the response
  delay(2000);// allow time for response to be read
  tft.fillRect(0, 130, 240, 38, BLACK);// erase the response
  sevenInstr();// show the instructions again
}
