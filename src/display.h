#pragma once

#include "driver.h"
#include "TFT_eSPI.h"
#include "weather.h"
#include <Arduino.h>
#include "calender.h"

void initDisplay(void);
void displayMeasurementTest(float value);
void showWeather(const Weather* weather);
void drawScreen_1(void);
void drawCalendar(const Calendar& calendar1, const Calendar& calendar2);
void drawCalendarGrid(struct tm nowTm);
void drawCalendarEvents(
    const Calendar& calendar,
    time_t now
);
void drawAccurateClock(void);
void drawUIClock(int insideValue, int outsideValue, int insideHumidity, int outsideHumidity);
void drawForecastScreen(Weather::CurrentConditions current, Weather::ForecastDay today, Weather::ForecastDay tomorrow, Weather::ForecastDay dayAfterTomorrow);

#ifdef EPAPER_ENABLE
extern EPaper epaper;
#endif
