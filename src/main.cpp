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
int lastState = HIGH; // the previous state from the input pin
int currentState;

int lastState2 = HIGH;
int currentState2;

Weather weather;
Calendar calendar1(CALENDAR1_URL);
Calendar calendar2(CALENDAR2_URL);

void setup()
{
    Serial.begin(115200);
    while(!Serial){};
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    initDisplay();
    initSensor();
    testWiFi();
    syncClock();
    setupWiFi();
    delay(1000);
}

void loop()
{
    currentState = digitalRead(BUTTON_PIN);
    currentState2 = digitalRead(BUTTON_PIN_2);

    if(lastState == LOW && currentState == HIGH)
    {
        weather.begin();
        weather.update();
        drawForecastScreen(weather.current(), weather.today(), weather.tomorrow(), weather.dayAfterTomorrow());

        // float tempIn, humIn;
        // getReadings(tempIn, humIn);
        // weather.begin();
        // weather.update();
        // float tempOut = weather.current().temperature;
        // float humOut = weather.current().humidity;
        // drawUIClock(tempIn, tempOut, humIn, humOut);

        //drawAccurateClock();


        // float temp, hum;
        // getReadings(temp, hum);
        // if (temp == 1.0 && hum == 1.0)
        // {
        //     Serial.print("ERR: Sensor unavailable.");
        // } else
        // {
        //     displayMeasurementTest(temp);
        //     Serial.print("Temperature: ");
        //     Serial.print(temp);
        //     Serial.println(" °C");
        //     Serial.print("Humidity: ");
        //     Serial.print(hum);
        //     Serial.println(" %");
        // }
    };

    if(lastState2 == LOW && currentState2 == HIGH)
    {
        calendar1.begin();
        calendar2.begin();
        calendar1.update();
        calendar2.update();
        drawCalendar(calendar1, calendar2);
        //drawScreen_1();
        // weather.begin();
        // if(weather.update())
        // {
        //     showWeather(&weather);
        // }
        // weather.idle();
    }

    lastState = currentState;
    lastState2 = currentState2;
}
