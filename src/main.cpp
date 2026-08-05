#include <Arduino.h>
#include "sht3x.h"
#include "TFT_eSPI.h"
#include "driver.h"
#include "display.h"
#include "WiFi.h"
#include "networking.h"
#include "secrets.h"
#include "weather.h"
#include "clock.h"
#include "calender.h"


const int BUTTON_PIN = 44; // the number of the pushbutton pin
const int BUTTON_PIN_2 = 43;

static constexpr uint32_t WEB_UPDATE_INTERVAL_MS = 3UL * 60UL * 60UL * 1000UL;
static constexpr uint32_t SENSOR_UPDATE_INTERVAL_MS = 15UL * 60UL * 1000UL;
static constexpr uint32_t DISPLAY_UPDATE_INTERVAL_MS = 5000UL;
static constexpr uint32_t BUTTON_DEBOUNCE_MS = 40UL;
static constexpr uint32_t BUTTON_CLICK_WINDOW_MS = 350UL;

volatile uint8_t button1ClickCount = 0;
volatile uint8_t button2ClickCount = 0;
volatile uint32_t button1LastClickMs = 0;
volatile uint32_t button2LastClickMs = 0;
volatile uint32_t button1LastInterruptMs = 0;
volatile uint32_t button2LastInterruptMs = 0;

uint32_t lastWebUpdateMs = 0;
uint32_t lastSensorUpdateMs = 0;
uint32_t lastDisplayUpdateMs = 0;

float insideTemperature = 0;
float insideHumidity = 0;
bool sensorReadingsValid = false;
uint8_t activeScreenIndex = 0;

const DisplayScreen screenOrder[] =
{
    DisplayScreen::AccurateClock,
    DisplayScreen::UIClock,
    DisplayScreen::Forecast,
    DisplayScreen::Calendar
};

static constexpr uint8_t SCREEN_COUNT = sizeof(screenOrder) / sizeof(screenOrder[0]);

Weather weather;
Calendar calendar1(CALENDAR1_URL);
Calendar calendar2(CALENDAR2_URL);

void IRAM_ATTR handleButton1Interrupt()
{
    uint32_t now = millis();
    if (now - button1LastInterruptMs < BUTTON_DEBOUNCE_MS)
        return;

    if (button1ClickCount < SCREEN_COUNT - 1)
        button1ClickCount++;

    button1LastInterruptMs = now;
    button1LastClickMs = now;
}

void IRAM_ATTR handleButton2Interrupt()
{
    uint32_t now = millis();
    if (now - button2LastInterruptMs < BUTTON_DEBOUNCE_MS)
        return;

    if (button2ClickCount < SCREEN_COUNT - 1)
        button2ClickCount++;

    button2LastInterruptMs = now;
    button2LastClickMs = now;
}

bool refreshSensorReadings()
{
    float temp;
    float hum;
    getReadings(temp, hum);

    sensorReadingsValid = !(temp == 1.0f && hum == 1.0f);
    if (!sensorReadingsValid)
    {
        Serial.println("ERR: Sensor unavailable.");
        return false;
    }

    insideTemperature = temp;
    insideHumidity = hum;
    lastSensorUpdateMs = millis();
    return true;
}

bool refreshWebData()
{
    bool ok = true;

    if (!setupWiFi())
    {
        disableWiFi();
        lastWebUpdateMs = millis();
        return false;
    }

    if (!syncClock())
        ok = false;

    weather.idle();
    if (!weather.begin() || !weather.update())
        ok = false;
    weather.idle();

    if (!calendar1.begin() || !calendar1.update())
        ok = false;

    if (!calendar2.begin() || !calendar2.update())
        ok = false;

    disableWiFi();

    lastWebUpdateMs = millis();
    return ok;
}

void redrawActiveScreen()
{
    switch (currentDisplayScreen())
    {
        case DisplayScreen::Forecast:
            if (weather.isValid())
                drawForecastScreen(weather.current(), weather.today(), weather.tomorrow(), weather.dayAfterTomorrow());
            break;

        case DisplayScreen::Calendar:
            drawCalendar(calendar1, calendar2);
            break;

        case DisplayScreen::AccurateClock:
            drawAccurateClock();
            break;

        case DisplayScreen::UIClock:
            if (sensorReadingsValid && weather.isValid())
            {
                drawUIClock(
                    insideTemperature,
                    weather.current().temperature,
                    insideHumidity,
                    weather.current().humidity
                );
            }
            break;

        case DisplayScreen::None:
        default:
            break;
    }
}

void showScreen(DisplayScreen screen)
{
    switch (screen)
    {
        case DisplayScreen::AccurateClock:
            drawAccurateClock();
            break;

        case DisplayScreen::UIClock:
            refreshSensorReadings();

            if (!weather.isValid())
                refreshWebData();

            if (sensorReadingsValid && weather.isValid())
            {
                drawUIClock(
                    insideTemperature,
                    weather.current().temperature,
                    insideHumidity,
                    weather.current().humidity
                );
            }
            break;

        case DisplayScreen::Forecast:
            if (!weather.isValid())
                refreshWebData();

            if (weather.isValid())
                drawForecastScreen(weather.current(), weather.today(), weather.tomorrow(), weather.dayAfterTomorrow());
            break;

        case DisplayScreen::Calendar:
            drawCalendar(calendar1, calendar2);
            break;

        case DisplayScreen::None:
        default:
            break;
    }
}

void navigateScreens(int8_t direction)
{
    int8_t nextIndex = activeScreenIndex + direction;

    if (nextIndex < 0)
        nextIndex = 0;
    else if (nextIndex >= SCREEN_COUNT)
        nextIndex = SCREEN_COUNT - 1;

    if (nextIndex == activeScreenIndex)
        return;

    activeScreenIndex = nextIndex;
    showScreen(screenOrder[activeScreenIndex]);
}

void processButtonRequests()
{
    uint8_t moveLeftCount = 0;
    uint8_t moveRightCount = 0;
    uint32_t leftLastClickMs;
    uint32_t rightLastClickMs;
    uint32_t now = millis();

    noInterrupts();
    leftLastClickMs = button1LastClickMs;
    rightLastClickMs = button2LastClickMs;

    if (button1ClickCount > 0 && now - leftLastClickMs >= BUTTON_CLICK_WINDOW_MS)
    {
        moveLeftCount = button1ClickCount;
        button1ClickCount = 0;
    }

    if (button2ClickCount > 0 && now - rightLastClickMs >= BUTTON_CLICK_WINDOW_MS)
    {
        moveRightCount = button2ClickCount;
        button2ClickCount = 0;
    }
    interrupts();

    if (moveLeftCount > 0)
        navigateScreens(-moveLeftCount);

    if (moveRightCount > 0)
        navigateScreens(moveRightCount);
}

void setup()
{
    Serial.begin(115200);
    while(!Serial){};
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton1Interrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_2), handleButton2Interrupt, FALLING);
    initDisplay();
    initSensor();
    refreshSensorReadings();
    refreshWebData();
    showScreen(screenOrder[activeScreenIndex]);
}

void loop()
{
    uint32_t now = millis();

    processButtonRequests();

    if (now - lastWebUpdateMs >= WEB_UPDATE_INTERVAL_MS)
    {
        refreshWebData();
        redrawActiveScreen();
    }

    if (now - lastSensorUpdateMs >= SENSOR_UPDATE_INTERVAL_MS)
    {
        refreshSensorReadings();
        if (currentDisplayScreen() == DisplayScreen::UIClock)
            redrawActiveScreen();
    }

    if (now - lastDisplayUpdateMs >= DISPLAY_UPDATE_INTERVAL_MS)
    {
        updateDisplay();
        lastDisplayUpdateMs = now;
    }

    delay(20);
}
