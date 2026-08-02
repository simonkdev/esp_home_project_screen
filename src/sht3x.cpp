#include <Arduino.h>
#include <Wire.h>
#include "sht3x.h"

#define SENSOR_ADDR 0x44

void initSensor()
{
    Wire.begin();
}

void getReadings(float& temp, float& hum)
{
    Wire.beginTransmission(SENSOR_ADDR);
    Wire.write(0x24);
    Wire.write(0x00);
    Wire.endTransmission();

    delay(15);

    Wire.requestFrom(SENSOR_ADDR, 6);
    if (Wire.available() == 6)
    {
        uint16_t rawT = (Wire.read() << 8) | Wire.read();
        Wire.read();              // CRC (ignore)
        uint16_t rawH = (Wire.read() << 8) | Wire.read();
        Wire.read();              // CRC (ignore)

        temp = -45 + (175 * (rawT / 65535.0));
        hum  = 100 * (rawH / 65535.0);
    }
    else
    {
        temp = 1;
        hum = 1;
    }

}
