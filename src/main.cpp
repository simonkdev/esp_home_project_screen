#include <Arduino.h>
#include "sht3x.h"
#include "TFT_eSPI.h"
#include "driver.h"

#ifdef EPAPER_ENABLE
EPaper epaper;
#endif

const int BUTTON_PIN = 44; // the number of the pushbutton pin

int lastState = HIGH; // the previous state from the input pin
int currentState;

void setup()
{
    Serial.begin(115200);
    while(!Serial){};
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    initSensor();

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

void loop()
{
    currentState = digitalRead(BUTTON_PIN);

    if(lastState == LOW && currentState == HIGH)
    {
        float temp, hum;
        getReadings(temp, hum);
        if (temp == 1.0 && hum == 1.0)
        {
            Serial.print("ERR: Sensor unavailable.");
        } else
        {
            Serial.print("Temperature: ");
            Serial.print(temp);
            Serial.println(" °C");
            Serial.print("Humidity: ");
            Serial.print(hum);
            Serial.println(" %");
        }
    };

    // save the last state
    lastState = currentState;


    //delay(200);
}
