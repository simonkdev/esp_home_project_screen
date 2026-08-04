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

#define GRID_TOP_Y 46
#define HOUR_HEIGHT 25
#define DAY_WIDTH 108
#define DAY_START_X 44
#define GRID_TOP_Y 46
#define HOUR_HEIGHT 25

const int COLUMN_X[7] =
{
    44,
    152,
    260,
    368,
    476,
    584,
    692
};

static const unsigned char PROGMEM image_paint_64_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xc0,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xc0,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xc0,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xc0,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


void drawCalendar(const Calendar& calendar1, const Calendar& calendar2)
{
    epaper.fillScreen(TFT_WHITE);

    time_t now = time(nullptr);

    struct tm nowTm;
    localtime_r(&now, &nowTm);

    drawCalendarGrid(nowTm);
    drawCalendarEvents(calendar1, now);
    drawCalendarEvents(calendar2, now);

    epaper.update();
}

void drawCalendarGrid(struct tm nowTm)
{
    const char* days[] =
    {
        "Mon",
        "Tue",
        "Wed",
        "Thu",
        "Fri",
        "Sat",
        "Sun"
    };

    for(int i = 0; i < 7; i++)
    {
        epaper.drawLine(COLUMN_X[i], -1, COLUMN_X[i], 479, TFT_BLACK);
    }

    epaper.drawLine(800, -1, 800, 479, TFT_BLACK);

    for(int h = 6; h <= 22; h++)
    {
        int y = GRID_TOP_Y + (h - 6) * HOUR_HEIGHT;

        epaper.drawLine(30, y, 830, y, TFT_BLACK);

        epaper.setTextColor(TFT_BLACK);
        epaper.setTextSize(1);

        if(h < 10)
            epaper.setCursor(4, y - 4);
        else
            epaper.setCursor(0, y - 4);

        epaper.print(h);
        epaper.print(":00");
    }

    struct tm monday = nowTm;

    int offset = (nowTm.tm_wday + 6) % 7;

    monday.tm_mday -= offset;
    mktime(&monday);

    epaper.setTextSize(2);

    for(int i = 0; i < 7; i++)
    {
        struct tm day = monday;
        day.tm_mday += i;
        mktime(&day);

        int x = COLUMN_X[i];

        epaper.setCursor(x + 34, 8);
        epaper.print(days[i]);

        epaper.setTextSize(2);

        char buffer[3];
        sprintf(buffer, "%d", day.tm_mday);

        int centerX = x + DAY_WIDTH / 2;
        int textWidth = strlen(buffer) * 12;
        int textX = centerX - textWidth / 2;

        if(day.tm_mday == nowTm.tm_mday &&
           day.tm_mon == nowTm.tm_mon &&
           day.tm_year == nowTm.tm_year)
        {
            epaper.drawRect(
                centerX - 11,
                23,
                22,
                22,
                TFT_BLACK
            );
        }

        epaper.setCursor(textX, 27);
        epaper.print(buffer);

        epaper.setTextSize(2);
    }

    int currentDay = (nowTm.tm_wday + 6) % 7;

    int minutesSince6 =
        nowTm.tm_hour * 60 +
        nowTm.tm_min -
        360;

    minutesSince6 = constrain(minutesSince6, 0, 16 * 60);

    int bitmapY =
        GRID_TOP_Y +
        (minutesSince6 * HOUR_HEIGHT) / 60 -
        5;

    int bitmapX =
        COLUMN_X[currentDay] - 3;

    epaper.drawBitmap(
        bitmapX,
        bitmapY,
        image_paint_64_bits,
        115,
        10,
        TFT_BLACK
    );
}

void drawCalendarEvents(
    const Calendar& calendar,
    time_t now)
{
    epaper.setTextSize(1);

    for(uint8_t i = 0; i < calendar.eventCount(); i++)
    {
        const Calendar::Event& e = calendar.event(i);

        struct tm startTm;
        struct tm endTm;

        localtime_r(&e.start, &startTm);
        localtime_r(&e.end, &endTm);

        int day = (startTm.tm_wday + 6) % 7;

        if(day < 0 || day > 6)
            continue;

        int startMinutes =
            startTm.tm_hour * 60 +
            startTm.tm_min;

        int endMinutes =
            endTm.tm_hour * 60 +
            endTm.tm_min;

        int y =
            GRID_TOP_Y +
            ((startMinutes - 360) * HOUR_HEIGHT) / 60;

        int h =
            ((endMinutes - startMinutes) * HOUR_HEIGHT) / 60;

        if(h < 12)
            h = 12;

        int x = COLUMN_X[day] + 3;

        bool finished = e.end < now;

        if(finished)
        {
            epaper.drawRect(
                x,
                y,
                102,
                h,
                TFT_BLACK);

            epaper.setTextColor(TFT_BLACK);
        }
        else
        {
            epaper.fillRect(
                x,
                y,
                102,
                h,
                TFT_BLACK);

            epaper.setTextColor(TFT_WHITE);
        }

        epaper.setCursor(
            x + 3,
            y + 3);

        epaper.print(e.title);

        epaper.setTextColor(TFT_BLACK);
    }
}
