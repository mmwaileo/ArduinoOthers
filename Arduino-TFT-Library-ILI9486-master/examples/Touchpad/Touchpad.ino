
#include "Arduino.h"
#include "SPI.h"
#include "tft.h"


#define TFT_CS        10    // do not use GPI032 or GPIO33 here
#define TFT_DC        7    // do not use GPI032 or GPIO33 here
#define SPI_MOSI      11
#define SPI_MISO      12
#define SPI_SCK       13
#define TP_IRQ        3
#define TP_CS         4
#define SD_CS         5

TFT tft;
TP tp(TP_CS, TP_IRQ);

uint16_t tp_x, tp_y;

void setup() {
    Serial.begin(115200);
    SPI.begin();
    tft.begin(TFT_CS, TFT_DC, SPI_MOSI, SPI_MISO, SPI_SCK);
    tft.setRotation(3); // Use landscape format
    tp.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREENYELLOW);
    tft.setTextSize(2);
}

//-------------------------------------------------------------------------------------
void loop(void) {
    tp.loop();
}
//-------------------------------------------------------------------------------------

// Event from TouchPad
void tp_pressed(uint16_t x, uint16_t y){
    tp_x=x;  tp_y=y;
}
void tp_released(){
    tft.fillRect(100, 100, 100, 40, TFT_BLACK);
    tft.setCursor(100, 100);
    tft.print("PosX="); tft.println(tp_x);
    tft.print("PosY="); tft.println(tp_y);
}
// TFT info
void tft_info(const char* info){
    Serial.print(info);
}





