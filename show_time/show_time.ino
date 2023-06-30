//#include <ST7789_kbv.h>
//ST7789_kbv tft(0x9341, 240, 320);
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#include <Fonts/FreeSansBold24pt7b.h>

void setup()
{
    tft.begin(tft.readID());
    tft.setRotation(1);
    tft.fillScreen(TFT_DARKGREY);
}

// draw a filled char @ (x, y).  return width of char.
int printfilled(char ch, int x, int y, const GFXfont *f_P)
{
    if (f_P ==  NULL) return 0;
    uint8_t w, h;          //filled rectangle
    int8_t datum;          //TLC to base line
    bool first = true, is_space = ch == ' ';
    GFXfont f_M, *f = &f_M;
    GFXglyph g_M, *g = &g_M;
    // copy structs to SRAM. determine standard rectangle for '8'
    memcpy_P(&f_M, f_P, sizeof(f_M));
    memcpy_P(&g_M, f->glyph + '8' - f->first, sizeof(g_M));
    h = g->height;        //'8' is biggest character
    datum = 0 - g->yOffset;
    if (is_space) {
        g->width = 0;     //i.e. all background
    }
    else {                //find glyph for actual character
        memcpy_P(&g_M, f->glyph + ch - f->first, sizeof(g_M));
    }
    uint8_t *p = f->bitmap + g->bitmapOffset;
    w = g->xAdvance;
    tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
    uint8_t mask = 0, bits;
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            uint16_t color = TFT_BLACK;
            if (row >= datum + g->yOffset && row < datum + g->yOffset + g->height) {
                if (col >= g->xOffset && col < g->xOffset + g->width) {
                    if (mask == 0) {
                        mask = 0x80;
                        bits = pgm_read_byte(p++);
                    }
                    if (bits & mask) color = TFT_BLUE;
                    mask >>= 1;
                }
            }
            tft.pushColors(&color, 1, first);
            first = false;
        }
    }
    //restore whole screen
    tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1);
    return w;
}

void fmt_number_hw(int x, int y, double value, int width, int prec, const GFXfont * f)
{
    uint8_t xofs = 0;
    char buf[20];
    dtostrf(value, width, prec, buf);
    for (int i = 0; i < width; i++) {
        // draw one char at a time
        xofs += printfilled(buf[i], x + xofs, y, f);
    }
}

void fmt_number(int x, int y, double value, int width, int prec, const GFXfont * f_P)
{
    uint8_t w, h;
    int8_t datum, sz;
    tft.setFont(f_P);
    if (f_P) {
        GFXfont f_M, *f = &f_M;
        GFXglyph g_M, *g = &g_M;
        // copy structs to SRAM. determine standard rectangle for '8'
        memcpy_P(&f_M, f_P, sizeof(f_M));
        memcpy_P(&g_M, f->glyph + '8' - f->first, sizeof(g_M));
        w = g->xAdvance;
        h = g->height;  //f->yAdvance allows inter-line background
        datum = g->yOffset;
        sz = 1;
    } else {
        sz = 5;
        w = 6 * sz, h = 8 * sz, datum = 0;
    }

    char buf[20];
    dtostrf(value, width, prec, buf);
    tft.setTextSize(sz);
    for (int i = 0; i < width; i++) {
        tft.fillRect(x + i * w, y, w, h, TFT_BLACK);
        tft.setCursor(x + i * w, y - datum);  //base line
        tft.print(buf[i]);
    }
}

void show_time(int x, int y, uint32_t t)
{
    tft.setTextColor(TFT_WHITE, TFT_BLACK);    //5x7 can rubout
    tft.setFont(NULL);
    tft.setTextSize(1);
    tft.setCursor(x, y);
    tft.print(t);
    tft.print(" ms ");
}

void loop()
{
    float number = 0.01 * random(0, 99999);
    uint32_t t;
    uint16_t color;
    int x = 50, y;

    tft.setTextColor(color = TFT_GREEN, TFT_BLACK);    //5x7 can rubout
    t = millis();
    fmt_number(x, y = 50, number, 6, 2, NULL);
    show_time(0, y, millis() - t);

    tft.setTextColor(color = TFT_RED);    //Free Fonts always transparent
    t = millis();
    fmt_number(x, y = 100, number, 6, 2, &FreeSansBold24pt7b);
    show_time(0, y, millis() - t);

    t = millis();
    fmt_number_hw(x, y = 150, number, 6, 2, &FreeSansBold24pt7b);
    show_time(0, y, millis() - t);

    delay(1000);
}