#include "clock.h"

const char* ntpServer = "pool.ntp.org";
const char* tz = "CET-1CEST,M3.5.0,M10.5.0/3";   // Germany with DST

bool syncClock(uint32_t timeoutMs) {
    configTzTime(tz, ntpServer);

    Serial.println("Waiting for time...");

    struct tm timeinfo;
    uint32_t startedAt = millis();
    while (!getLocalTime(&timeinfo)) {
        if (millis() - startedAt >= timeoutMs) {
            Serial.println("\nTime synchronization timed out");
            return false;
        }

        delay(500);
        Serial.print(".");
    }

    Serial.println("\nTime synchronized!");
    Serial.println(&timeinfo, "%A, %d %B %Y %H:%M:%S");

    return true;
}
