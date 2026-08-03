#include "weather.h"

String buildWeatherURL()
{
    String url = "https://api.open-meteo.com/v1/forecast?";

    url += "latitude=";
    url += String(LATITUDE, 6);

    url += "&longitude=";
    url += String(LONGITUDE, 6);

    url += "&current=temperature_2m,relative_humidity_2m,weather_code";

    return url;
}

bool fetchWeather()
{
    WiFiClientSecure client;
    client.setInsecure();      // Fine for prototyping

    HTTPClient http;

    if (!http.begin(client, buildWeatherURL()))
        return false;

    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK)
    {
        Serial.printf("HTTP Error: %d\n", httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();

    Serial.println(payload);
    http.end();

    return true;
}

void testWeather()
{
    setupWiFi();
    if (!fetchWeather())
        Serial.println("Failed to fetch weather");
    else
        Serial.println("Weather fetched successfully");
    disableWiFi();
}
