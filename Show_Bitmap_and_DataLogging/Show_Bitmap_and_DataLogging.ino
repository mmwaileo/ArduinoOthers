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
MCUFRIEND_kbv tft;

#define SD_CS     10  //CS Pin
#define NAMEMATCH ""         // "" matches any name
//#define NAMEMATCH "tiger"    // *tiger*.bmp
#define PALETTEDEPTH   8     // support 256-colour Palette

char namebuf[32] = "/";   //BMP files in root directory
//char namebuf[32] = "/bitmaps/";  //BMP directory e.g. files in /bitmaps/*.bmp

File root;
int pathlen;

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

uint8_t iRoom_temperature = 21;
 uint8_t iSet_temperature = 20;

void setup()
{
    uint16_t ID;
    Serial.begin(9600);
    Serial.print("Show BMP files on TFT with ID:0x");
    ID = tft.readID();
    Serial.println(ID, HEX);
    if (ID == 0x0D3D3) ID = 0x9481;
    tft.begin(ID);
    tft.fillScreen(WHITE);
    tft.setTextColor(0xFFFF, 0x0000);
    bool good = SD.begin(SD_CS);
    if (!good) {
        Serial.print(F("cannot start SD"));
        while (1);
    }
    root = SD.open(namebuf);
    pathlen = strlen(namebuf);
    
}

void loop()
{
    showBMP("sit_logo.bmp", 5, 10);
    tft.drawRoundRect(45, 260, 150, 20, 5, BLACK);
    tft.setCursor(52, 290);
    tft.setTextSize(2);
    tft.setTextColor(BLACK);
    tft.println("Loading...");
    delay(1000);
    for(int i=0; i<150; i+=10){
      tft.fillRoundRect(45, 260, 10+i, 20, 5,  BLACK);
      delay(100);
      }
    
    delay(5000);
    tft.fillScreen(WHITE);

    //dataLogging();

}

/********************************************************************//**
 * @brief     Drawing of the main program screen
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_main_screen()
{
  tft.fillScreen(BLACK);
  
  // draw circles
  update_circle_color();

  // draw temperature up/dwn buttons
  draw_up_down_button();

  // draw icons
  tft.drawRGBBitmap(10,290, fan_blue_24,24,24);
  tft.drawRGBBitmap(200,282, fan_blue_32,32,32);
  tft.drawRGBBitmap(10,10, wrench, 24,24);  
  
  // draw default fan level
  draw_fan_level(50,312,iFan_level);

  update_SET_temp();
 
}

/********************************************************************//**
 * @brief     Drawing of the screen for Options menu
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_option_screen()
{
  tft.fillScreen(BLACK);

  tft.setTextColor(BLACK);
  //tft.setFont(&FreeSansBold9pt7b);  
  /*
  // Modbus Address adjustment
  tft.setCursor(10, 20);
  tft.print("MODBUS address");
  tft.setFont(&FreeSansBold24pt7b);
  tft.setCursor(30, 65);
  tft.print("-");
  tft.setCursor(190, 65);
  tft.print("+");
  tft.drawLine(5,80,235,80, ILI9341_WHITE);
  

  // Screen cleaning idle timer
  //tft.setFont(&FreeSansBold12pt7b);  
  tft.setCursor(26, 130);
  tft.print("Screen cleaning");
  tft.drawLine(5,160,235,160, WHITE);

  // OK Button
  tft.setFont(&FreeSansBold24pt7b);
  tft.drawLine(5,260,235,260, ILI9341_WHITE);
  tft.setCursor(90, 310);
  tft.print("OK");  
  update_Modbus_addr();
 */
}

/********************************************************************//**
 * @brief     update of the color of the big circle according the 
 *            difference between set and room temperature 
 * @param[in] None
 * @return    None
 *********************************************************************/
void update_circle_color()
{
  /*
  // HEATING 
  if ((iRoom_temperature < iSet_temperature) && (Thermostat_mode != HEATING)) {
    Thermostat_mode = HEATING;
    draw_circles();
  }

  // COOLING 
  if ((iRoom_temperature > iSet_temperature) && (Thermostat_mode != COOLING)) {
    Thermostat_mode = COOLING;
    draw_circles();
  }

  // Temperature ok 
  if ((iRoom_temperature == iSet_temperature) && (Thermostat_mode != TEMP_OK)) {
    Thermostat_mode = TEMP_OK;
    */
    draw_circles();
  }
}

/********************************************************************//**
 * @brief     drawing of the circles in main screen including the value 
 *            of room temperature
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_circles()
{

  //draw big circle 
  unsigned char i;
  if (iRoom_temperature < iSet_temperature) {
    // heating - red
    for(i=0; i < 10; i++) tft.drawCircle(120, 120, 80 + i, RED);
  } else if (iRoom_temperature > iSet_temperature) {
    // cooling - blue
    for(i=0; i < 10; i++) tft.drawCircle(120, 120, 80 + i, BLUE);    
  } else {
    // Temperature ok
    for(i=0; i < 10; i++) tft.drawCircle(120, 120, 80 + i, GREEN);       
  }

  //draw small 
  tft.fillCircle(60, 200, 40, CYAN);

  //draw °C in big circle
  tft.setTextColor(BLACK);
  //tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(130, 100);
  tft.print("o");
 // tft.setFont(&FreeSansBold24pt7b);
  tft.setCursor(140, 130);
  tft.print("C");

  // draw room and °C in small circle
  tft.setTextColor(CYAN);
//  tft.setFont(&FreeSansBold12pt7b);
  tft.setCursor(75, 220);
  tft.print("C");
  tft.drawCircle(69,204, 2, WHITE);
  tft.drawCircle(69,204, 3, WHITE);
 // tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(35, 190);
  tft.print("Room");
  update_Room_temp();

}


/********************************************************************//**
 * @brief     drawing of the both buttons for setting temperature up 
 *            and down
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_up_down_button()
{
  //up button 
  tft.fillTriangle(215,10,230,30,200,30, WHITE);
    
  //down button
  tft.fillTriangle(215,230,230,210,200,210, WHITE);
}



/********************************************************************//**
 * @brief     drawing of the fan level in main screen
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_fan_level(uint16_t x0, uint16_t y0,  uint8_t ilevel)
{
  unsigned char i;
  if (ilevel >= 5)  ilevel = 5;
  for(i=0; i < 5; i++) {
    if (i < ilevel)  {
      tft.fillRect(x0 + (30*i), y0- 10 -(i*8), 20, 10 + (i*8), WHITE);  
    } else {
      tft.fillRect(x0 + (30*i), y0- 10 -(i*8), 20, 10 + (i*8), BLACK);
      tft.drawRect(x0 + (30*i), y0- 10 -(i*8), 20, 10 + (i*8), WHITE); 
      
    }
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

#define BMPIMAGEOFFSET 54

#define BUFFPIXEL      20

uint16_t read16(File& f) {
    uint16_t result;         // read little-endian
    f.read(&result, sizeof(result));
    return result;
}

uint32_t read32(File& f) {
    uint32_t result;
    f.read(&result, sizeof(result));
    return result;
}

uint8_t showBMP(char *nm, int x, int y)
{
    File bmpFile;
    int bmpWidth, bmpHeight;    // W+H in pixels
    uint8_t bmpDepth;           // Bit depth (currently must be 24, 16, 8, 4, 1)
    uint32_t bmpImageoffset;    // Start of image data in file
    uint32_t rowSize;           // Not always = bmpWidth; may have padding
    uint8_t sdbuffer[3 * BUFFPIXEL];    // pixel in buffer (R+G+B per pixel)
    uint16_t lcdbuffer[(1 << PALETTEDEPTH) + BUFFPIXEL], *palette = NULL;
    uint8_t bitmask, bitshift;
    boolean flip = true;        // BMP is stored bottom-to-top
    int w, h, row, col, lcdbufsiz = (1 << PALETTEDEPTH) + BUFFPIXEL, buffidx;
    uint32_t pos;               // seek position
    boolean is565 = false;      //

    uint16_t bmpID;
    uint16_t n;                 // blocks read
    uint8_t ret;

    if ((x >= tft.width()) || (y >= tft.height()))
        return 1;               // off screen

    bmpFile = SD.open(nm);      // Parse BMP header
    bmpID = read16(bmpFile);    // BMP signature
    (void) read32(bmpFile);     // Read & ignore file size
    (void) read32(bmpFile);     // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile);       // Start of image data
    (void) read32(bmpFile);     // Read & ignore DIB header size
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    n = read16(bmpFile);        // # planes -- must be '1'
    bmpDepth = read16(bmpFile); // bits per pixel
    pos = read32(bmpFile);      // format
    if (bmpID != 0x4D42) ret = 2; // bad ID
    else if (n != 1) ret = 3;   // too many planes
    else if (pos != 0 && pos != 3) ret = 4; // format: 0 = uncompressed, 3 = 565
    else if (bmpDepth < 16 && bmpDepth > PALETTEDEPTH) ret = 5; // palette 
    else {
        bool first = true;
        is565 = (pos == 3);               // ?already in 16-bit format
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;
        if (bmpHeight < 0) {              // If negative, image is in top-down order.
            bmpHeight = -bmpHeight;
            flip = false;
        }

        w = bmpWidth;
        h = bmpHeight;
        if ((x + w) >= tft.width())       // Crop area to be loaded
            w = tft.width() - x;
        if ((y + h) >= tft.height())      //
            h = tft.height() - y;

        if (bmpDepth <= PALETTEDEPTH) {   // these modes have separate palette
            //bmpFile.seek(BMPIMAGEOFFSET); //palette is always @ 54
            bmpFile.seek(bmpImageoffset - (4<<bmpDepth)); //54 for regular, diff for colorsimportant
            bitmask = 0xFF;
            if (bmpDepth < 8)
                bitmask >>= bmpDepth;
            bitshift = 8 - bmpDepth;
            n = 1 << bmpDepth;
            lcdbufsiz -= n;
            palette = lcdbuffer + lcdbufsiz;
            for (col = 0; col < n; col++) {
                pos = read32(bmpFile);    //map palette to 5-6-5
                palette[col] = ((pos & 0x0000F8) >> 3) | ((pos & 0x00FC00) >> 5) | ((pos & 0xF80000) >> 8);
            }
        }

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
        for (row = 0; row < h; row++) { // For each scanline...
            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            uint8_t r, g, b, *sdptr;
            int lcdidx, lcdleft;
            if (flip)   // Bitmap is stored bottom-to-top order (normal BMP)
                pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
            else        // Bitmap is stored top-to-bottom
                pos = bmpImageoffset + row * rowSize;
            if (bmpFile.position() != pos) { // Need seek?
                bmpFile.seek(pos);
                buffidx = sizeof(sdbuffer); // Force buffer reload
            }

            for (col = 0; col < w; ) {  //pixels in row
                lcdleft = w - col;
                if (lcdleft > lcdbufsiz) lcdleft = lcdbufsiz;
                for (lcdidx = 0; lcdidx < lcdleft; lcdidx++) { // buffer at a time
                    uint16_t color;
                    // Time to read more pixel data?
                    if (buffidx >= sizeof(sdbuffer)) { // Indeed
                        bmpFile.read(sdbuffer, sizeof(sdbuffer));
                        buffidx = 0; // Set index to beginning
                        r = 0;
                    }
                    switch (bmpDepth) {          // Convert pixel from BMP to TFT format
                        case 24:
                            b = sdbuffer[buffidx++];
                            g = sdbuffer[buffidx++];
                            r = sdbuffer[buffidx++];
                            color = tft.color565(r, g, b);
                            break;
                        case 16:
                            b = sdbuffer[buffidx++];
                            r = sdbuffer[buffidx++];
                            if (is565)
                                color = (r << 8) | (b);
                            else
                                color = (r << 9) | ((b & 0xE0) << 1) | (b & 0x1F);
                            break;
                        case 1:
                        case 4:
                        case 8:
                            if (r == 0)
                                b = sdbuffer[buffidx++], r = 8;
                            color = palette[(b >> bitshift) & bitmask];
                            r -= bmpDepth;
                            b <<= bmpDepth;
                            break;
                    }
                    lcdbuffer[lcdidx] = color;

                }
                tft.pushColors(lcdbuffer, lcdidx, first);
                first = false;
                col += lcdidx;
            }           // end cols
        }               // end rows
        tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1); //restore full screen
        ret = 0;        // good render
    }
    bmpFile.close();
    return (ret);
}
