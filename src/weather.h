#pragma once

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "secrets.h"
#include "networking.h"

void testWeather(void);
bool fetchWeather(void);

class Weather
{
public:

    enum class WeatherCondition
    {
        Unknown,

        ClearSky,

        MainlyClear,
        PartlyCloudy,
        Overcast,

        Fog,
        DepositingRimeFog,

        LightDrizzle,
        ModerateDrizzle,
        DenseDrizzle,

        LightFreezingDrizzle,
        DenseFreezingDrizzle,

        SlightRain,
        ModerateRain,
        HeavyRain,

        LightFreezingRain,
        HeavyFreezingRain,

        SlightSnowfall,
        ModerateSnowfall,
        HeavySnowfall,

        SnowGrains,

        SlightRainShowers,
        ModerateRainShowers,
        ViolentRainShowers,

        SlightSnowShowers,
        HeavySnowShowers,

        Thunderstorm,
        ThunderstormWithSlightHail,
        ThunderstormWithHeavyHail
    };

    struct CurrentConditions
    {
        float temperature;
        float feelsLike;

        float humidity;

        float windSpeed;
        uint16_t windDirection;

        float rainfallAmount;
        float rainProbability;

        WeatherCondition condition;
        uint8_t weatherCode;

        time_t observationTime;
    };

    struct ForecastDay
    {
        time_t date;

        float highTemperature;
        float lowTemperature;

        float windSpeed;
        uint16_t windDirection;

        float rainfallAmount;
        float rainProbability;

        WeatherCondition condition;
        uint8_t weatherCode;

        time_t sunrise;
        time_t sunset;
    };

    struct WeatherAlert
    {
        bool active = false;

        String title;
        String severity;
        String description;

        time_t startTime;
        time_t endTime;
    };


public:

    Weather();

    ~Weather();

    bool begin();

    bool update();

    void idle();

    bool isValid() const;

    const CurrentConditions& current() const;

    const ForecastDay& today() const;

    const ForecastDay& tomorrow() const;

    const ForecastDay& dayAfterTomorrow() const;

    const ForecastDay& forecast(uint8_t day) const;

    bool hasAlerts() const;

    const WeatherAlert& alert() const;

    time_t lastUpdate() const;

    int lastHttpStatus() const;

    void testWeatherClass(void);


private:

    String download();

    bool parseWeather(
        const String& json,
        CurrentConditions& current,
        ForecastDay forecast[3],
        WeatherAlert& alert
    );

    String buildWeatherURL();

    WeatherCondition decodeWeatherCode(uint8_t code);

    String weatherConditionToString(WeatherCondition condition);

    time_t parseTimeStamp(const char* value);


private:

    CurrentConditions m_current;

    ForecastDay m_forecast[3];

    WeatherAlert m_alert;

    bool m_valid;

    bool m_idle;

    time_t m_lastUpdate;

    time_t m_lastAttempt;

    int m_lastHttpStatus;

    WiFiClientSecure* m_client;

    HTTPClient* m_http;

    float m_latitude;

    float m_longitude;
};
