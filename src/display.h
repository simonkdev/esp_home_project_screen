#pragma once

#include "driver.h"
#include "TFT_eSPI.h"
#include "weather.h"
#include <Arduino.h>

void initDisplay(void);
void displayMeasurementTest(float value);
void showWeather(const Weather* weather);
void drawScreen_1(void);

#ifdef EPAPER_ENABLE
extern EPaper epaper;
#endif
