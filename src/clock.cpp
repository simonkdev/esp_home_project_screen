#include "clock.h"

const char* ntpServer = "pool.ntp.org";
const char* tz = "CET-1CEST,M3.5.0,M10.5.0/3";   // Germany with DST

void syncClock() {
    setupWiFi();

    configTzTime(tz, ntpServer);

    Serial.println("Waiting for time...");

    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nTime synchronized!");
    Serial.println(&timeinfo, "%A, %d %B %Y %H:%M:%S");

    disableWiFi();
}
