#include "driver.h"
#include "TFT_eSPI.h"
#include "display.h"
#include <Arduino.h>

#ifdef EPAPER_ENABLE
EPaper epaper;
#endif

void initDisplay()
{
    #ifdef EPAPER_ENABLE
    epaper.begin();
    epaper.fillScreen(TFT_WHITE);
    epaper.fillCircle(25, 25, 15, TFT_BLACK);
    epaper.fillRect(epaper.width() - 40, 10, 30, 30, TFT_BLACK);
    for (int i = 0; i < epaper.height() / 80; i++)
    {
        epaper.setTextSize(i + 1);
        epaper.drawLine(10, 70 + 60 * i, epaper.width() - 10, 70 + 60 * i, TFT_BLACK);
        epaper.drawString("Hello ePaper", 10, 80 + 60 * i);
    }

    epaper.update();
    #endif
}

void displayMeasurementTest(float value)
{

    #ifdef EPAPER_ENABLE

    epaper.fillScreen(TFT_WHITE);
    epaper.setTextSize(2);
    epaper.drawString(String(value), 10, 10);
    epaper.update();
    #endif
}
