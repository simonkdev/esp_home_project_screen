#include <Arduino.h>
#include "sht3x.h"
#include "TFT_eSPI.h"
#include "driver.h"
#include "display.h"
#include "WiFi.h"
#include "networking.h"
#include "secrets.h"

const int BUTTON_PIN = 44; // the number of the pushbutton pin
int lastState = HIGH; // the previous state from the input pin
int currentState;

void setup()
{
    Serial.begin(115200);
    while(!Serial){};
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    initDisplay();
    initSensor();
    testWiFi();
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
            displayMeasurementTest(temp);
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


    delay(200);
}
