
#include "Arduino.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "tft.h"

#define TFT_CS        10    // do not use GPI032 or GPIO33 here
#define TFT_DC        7    // do not use GPI032 or GPIO33 here
#define SPI_MOSI      11
#define SPI_MISO      12
#define SPI_SCK       13
#define TP_IRQ        3
#define TP_CS         4
#define SD_CS         5


#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

TFT tft;

void setup() {
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    tft.begin(TFT_CS, TFT_DC, SPI_MOSI, SPI_MISO, SPI_SCK);
    SD.begin(SD_CS);
//  SD.begin(SD_CS,SPI,16000000); // faster speed
}

//-------------------------------------------------------------------------------------
void loop(void) {
        tft.setRotation(0); //portait
        tft.drawBmpFile(SD, "/wall_e.bmp", 0, 0);
        delay(2000);
        tft.setRotation(3); //landscape
        tft.drawJpgFile(SD,"/wallpaper1.jpg", 0,0);
        delay(2000);
        tft.drawJpgFile(SD,"/arduino.jpg", 100,50);
        delay(2000);

}
