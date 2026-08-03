#include "display.h"

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


void showWeather(const Weather* weather)
{
    if (weather == nullptr)
    {
        return;
    }

    Serial.println("Drawing weather screen");

    epaper.fillScreen(TFT_WHITE);

    epaper.setTextColor(TFT_BLACK, TFT_WHITE);
    epaper.setTextDatum(TL_DATUM);


    /*
        Header
    */
    epaper.setTextFont(4);
    epaper.drawString("Weather Forecast", 20, 15);


    /*
        Current conditions
    */
    const Weather::CurrentConditions& current = weather->current();

    epaper.setTextFont(2);

    int y = 75;

    epaper.drawString("Current", 20, y);

    y += 30;

    epaper.drawString("Temperature:", 30, y);
    epaper.drawFloat(current.temperature, 1, 180, y);

    epaper.drawString(" C", 240, y);


    y += 25;

    epaper.drawString("Feels like:", 30, y);
    epaper.drawFloat(current.feelsLike, 1, 180, y);
    epaper.drawString(" C", 240, y);


    y += 25;

    epaper.drawString("Humidity:", 30, y);
    epaper.drawNumber(current.humidity, 180, y);
    epaper.drawString(" %", 230, y);


    y += 25;

    epaper.drawString("Wind:", 30, y);
    epaper.drawFloat(current.windSpeed, 1, 180, y);
    epaper.drawString(" km/h", 240, y);


    /*
        Forecast cards
    */

    epaper.drawLine(20, 230, 780, 230, TFT_BLACK);

    epaper.setTextFont(2);
    epaper.drawString("Forecast", 20, 245);


    const int cardWidth = 240;
    const int cardHeight = 150;

    const int startX = 20;
    const int startY = 280;


    for (uint8_t i = 0; i < 3; i++)
    {
        const Weather::ForecastDay& day = weather->forecast(i);


        int x = startX + (i * cardWidth);


        epaper.drawRect(
            x,
            startY,
            cardWidth - 10,
            cardHeight,
            TFT_BLACK
        );


        epaper.setTextDatum(MC_DATUM);


        epaper.drawString(
            "DAY " + String(i + 1),
            x + (cardWidth / 2) - 5,
            startY + 20
        );


        epaper.setTextDatum(TL_DATUM);


        int tx = x + 15;
        int ty = startY + 50;


        epaper.drawString(
            "High:",
            tx,
            ty
        );

        epaper.drawFloat(
            day.highTemperature,
            1,
            tx + 90,
            ty
        );

        epaper.drawString(
            " C",
            tx + 145,
            ty
        );


        ty += 25;

        epaper.drawString(
            "Low:",
            tx,
            ty
        );

        epaper.drawFloat(
            day.lowTemperature,
            1,
            tx + 90,
            ty
        );

        epaper.drawString(
            " C",
            tx + 145,
            ty
        );


        ty += 25;

        epaper.drawString(
            "Rain:",
            tx,
            ty
        );

        epaper.drawFloat(
            day.rainfallAmount,
            1,
            tx + 90,
            ty
        );

        epaper.drawString(
            " mm",
            tx + 145,
            ty
        );


        ty += 25;

        epaper.drawString(
            "Code:",
            tx,
            ty
        );

        epaper.drawNumber(
            day.weatherCode,
            tx + 90,
            ty
        );
    }


    /*
        Push framebuffer to ePaper
        (required for ePaper extension)
    */

#ifdef EPAPER_ENABLE
    epaper.update();
#endif


    Serial.println("Weather screen complete");
}

// [BEGIN Seeed_GFX converted]
void drawScreen_1(void)
{
    epaper.fillScreen(TFT_WHITE);

    // rect 1
    epaper.drawRect(
        134,
        23,
        533,
        298,
        TFT_BLACK
    );

    // string 2
    epaper.setTextColor(TFT_BLACK);
    epaper.setTextSize(4);
    epaper.setTextWrap(false);
    epaper.setCursor(354, 70);
    epaper.print("Text");

    // Commit framebuffer to ePaper
    epaper.update();
}
// [END Seeed_GFX converted]
