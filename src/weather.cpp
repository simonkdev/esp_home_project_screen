#include "weather.h"

String urlBuildTest()
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

    if (!http.begin(client, urlBuildTest()))
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

int Weather::lastHttpStatus() const
{
    return m_lastHttpStatus;
}

const Weather::ForecastDay& Weather::forecast(uint8_t day) const
{
    return m_forecast[day];
}

void Weather::testWeatherClass()
{
    Serial.println("Starting weather test...");

    if (!begin())
    {
        Serial.println("Weather begin failed");
        return;
    }

    Serial.println("Downloading weather...");

    if (!update())
    {
        Serial.println("Weather update failed");
        Serial.print("HTTP status: ");
        Serial.println(lastHttpStatus());
        return;
    }

    Serial.println("Weather update successful");

    const CurrentConditions& currentConditions = current();

    Serial.println("--- Current ---");

    Serial.print("Temperature: ");
    Serial.print(currentConditions.temperature);
    Serial.println(" C");

    Serial.print("Feels like: ");
    Serial.print(currentConditions.feelsLike);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(currentConditions.humidity);
    Serial.println(" %");

    Serial.print("Wind speed: ");
    Serial.print(currentConditions.windSpeed);
    Serial.println(" km/h");

    Serial.print("Weather code: ");
    Serial.println(currentConditions.weatherCode);


    Serial.println("--- Forecast ---");

    for (int i = 0; i < 3; i++)
    {
        const ForecastDay& day = forecast(i);

        Serial.print("Day ");
        Serial.println(i);

        Serial.print("High: ");
        Serial.println(day.highTemperature);

        Serial.print("Low: ");
        Serial.println(day.lowTemperature);

        Serial.print("Rain: ");
        Serial.println(day.rainfallAmount);

        Serial.print("Weather code: ");
        Serial.println(day.weatherCode);

        Serial.println();
    }

    idle();

    Serial.println("Weather test finished");
}


Weather::Weather() : m_valid(false), m_lastUpdate(0), m_lastHttpStatus(0), m_lastAttempt(0), m_alert(), m_idle(true), m_client(nullptr), m_http(nullptr)
{
    m_latitude = LATITUDE;
    m_longitude = LONGITUDE;
}

Weather::~Weather()
{
    idle();
}

bool Weather::begin()
{
    m_client = new WiFiClientSecure();
    m_http = new HTTPClient();
    m_client->setInsecure();

    m_idle = false;
    return true;
}

bool Weather::update()
{
    if (m_idle)
        return false;

    m_lastAttempt = time(nullptr);

    CurrentConditions tempCurrent;
    ForecastDay tempForecast[3];
    WeatherAlert tempAlert;

    String response = download();

    if(parseWeather(response, tempCurrent, tempForecast, tempAlert))
    {
        m_current = tempCurrent;
        for (int i = 0; i < 3; i++)
            m_forecast[i] = tempForecast[i];
        m_alert = tempAlert;
        m_valid = true;
        m_lastUpdate = time(nullptr);
        return true;
    }
    return false;
}

void Weather::idle()
{
    if(m_http)
    {
        m_http->end();
        delete m_http;
        m_http = nullptr;
    }

    if(m_client)
    {
        m_client->stop();
        delete m_client;
        m_client = nullptr;
    }
    m_idle = true;
}

bool Weather::isValid() const
{
    return m_valid;
}

const Weather::CurrentConditions& Weather::current() const
{
    return m_current;
}

const Weather::ForecastDay& Weather::today() const
{
    return m_forecast[0];
}


String Weather::download()
{
    String url = buildWeatherURL();

    if (!m_http->begin(*m_client, url))
        return "";

    int status = m_http->GET();
    m_lastHttpStatus = status;

    Serial.print("HTTP status: ");
    Serial.println(status);

    if (status != HTTP_CODE_OK)
    {
        Serial.println(m_http->getString());
        return "";
    }

    return m_http->getString();
}

bool Weather::parseWeather(
    const String& json,
    Weather::CurrentConditions& current,
    Weather::ForecastDay forecast[3],
    Weather::WeatherAlert& alert
)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);

    if (error)
        return false;

    current.temperature = doc["current"]["temperature_2m"];
    current.feelsLike = doc["current"]["apparent_temperature"];

    current.humidity = doc["current"]["relative_humidity_2m"];

    current.windSpeed = doc["current"]["wind_speed_10m"];
    current.windDirection = doc["current"]["wind_direction_10m"].as<uint16_t>();

    current.rainfallAmount = doc["current"]["rain"];

    current.weatherCode = doc["current"]["weather_code"];
    current.condition = decodeWeatherCode(current.weatherCode);

    current.observationTime = parseTimeStamp(doc["current"]["time"]);

    for(int i = 0; i < 3; i++)
    {
        forecast[i].date = parseTimeStamp(doc["daily"]["time"][i]);

        forecast[i].highTemperature = doc["daily"]["temperature_2m_max"][i];
        forecast[i].lowTemperature = doc["daily"]["temperature_2m_min"][i];

        forecast[i].windSpeed = doc["daily"]["wind_speed_10m_max"][i];
        forecast[i].windDirection = doc["daily"]["wind_direction_10m_dominant"][i].as<uint16_t>();

        forecast[i].rainfallAmount = doc["daily"]["rain_sum"][i];
        forecast[i].rainProbability = doc["daily"]["precipitation_probability_max"][i];

        forecast[i].weatherCode = doc["daily"]["weather_code"][i];
        forecast[i].condition = decodeWeatherCode(doc["daily"]["weather_code"][i]);

        forecast[i].sunrise = parseTimeStamp(doc["daily"]["sunrise"][i]);
        forecast[i].sunset = parseTimeStamp(doc["daily"]["sunset"][i]);
    }

    return true;
}

String Weather::buildWeatherURL()
{
    String url = "https://api.open-meteo.com/v1/forecast?";

    url += "latitude=";
    url += String(m_latitude, 6);

    url += "&longitude=";
    url += String(m_longitude, 6);

    url += "&current=temperature_2m,apparent_temperature,relative_humidity_2m,wind_speed_10m,wind_direction_10m,rain,weather_code";

    url += "&daily=temperature_2m_max,temperature_2m_min,wind_speed_10m_max,wind_direction_10m_dominant,rain_sum,precipitation_probability_max,weather_code,sunrise,sunset";

    url += "&forecast_days=3";

    url += "&timezone=auto";

    return url;
}

Weather::WeatherCondition Weather::decodeWeatherCode(uint8_t code)
{
    switch(code)
    {
        case 0:
            return WeatherCondition::ClearSky;

        case 1:
            return WeatherCondition::MainlyClear;
        case 2:
            return WeatherCondition::PartlyCloudy;
        case 3:
            return WeatherCondition::Overcast;

        case 45:
        case 48:
            return WeatherCondition::Fog;

        case 51:
            return WeatherCondition::LightDrizzle;
        case 53:
            return WeatherCondition::ModerateDrizzle;
        case 55:
            return WeatherCondition::DenseDrizzle;

        case 56:
            return WeatherCondition::LightFreezingDrizzle;
        case 57:
            return WeatherCondition::DenseFreezingDrizzle;

        case 61:
            return WeatherCondition::SlightRain;
        case 63:
            return WeatherCondition::ModerateRain;
        case 65:
            return WeatherCondition::HeavyRain;

        case 66:
            return WeatherCondition::LightFreezingRain;
        case 67:
            return WeatherCondition::HeavyFreezingRain;

        case 71:
            return WeatherCondition::SlightSnowfall;
        case 73:
            return WeatherCondition::ModerateSnowfall;
        case 75:
            return WeatherCondition::HeavySnowfall;

        case 77:
            return WeatherCondition::SnowGrains;

        case 80:
            return WeatherCondition::SlightRainShowers;
        case 81:
            return WeatherCondition::ModerateRainShowers;
        case 82:
            return WeatherCondition::ViolentRainShowers;

        case 85:
            return WeatherCondition::SlightSnowShowers;
        case 86:
            return WeatherCondition::HeavySnowShowers;

        case 95:
            return WeatherCondition::Thunderstorm;

        case 96:
            return WeatherCondition::ThunderstormWithSlightHail;
        case 99:
            return WeatherCondition::ThunderstormWithHeavyHail;

        default:
            return WeatherCondition::Unknown;
    }
}

String Weather::weatherConditionToString(Weather::WeatherCondition condition)
{
    switch(condition)
    {
        case WeatherCondition::Unknown:
            return "Unknown";

        case WeatherCondition::ClearSky:
            return "Clear sky";

        case WeatherCondition::MainlyClear:
            return "Mainly clear";
        case WeatherCondition::PartlyCloudy:
            return "Partly cloudy";
        case WeatherCondition::Overcast:
            return "Overcast";

        case WeatherCondition::Fog:
            return "Fog";
        case WeatherCondition::DepositingRimeFog:
            return "Depositing rime fog";

        case WeatherCondition::LightDrizzle:
            return "Light drizzle";
        case WeatherCondition::ModerateDrizzle:
            return "Moderate drizzle";
        case WeatherCondition::DenseDrizzle:
            return "Dense drizzle";

        case WeatherCondition::LightFreezingDrizzle:
            return "Light freezing drizzle";
        case WeatherCondition::DenseFreezingDrizzle:
            return "Dense freezing drizzle";

        case WeatherCondition::SlightRain:
            return "Slight rain";
        case WeatherCondition::ModerateRain:
            return "Moderate rain";
        case WeatherCondition::HeavyRain:
            return "Heavy rain";

        case WeatherCondition::LightFreezingRain:
            return "Light freezing rain";
        case WeatherCondition::HeavyFreezingRain:
            return "Heavy freezing rain";

        case WeatherCondition::SlightSnowfall:
            return "Slight snowfall";
        case WeatherCondition::ModerateSnowfall:
            return "Moderate snowfall";
        case WeatherCondition::HeavySnowfall:
            return "Heavy snowfall";

        case WeatherCondition::SnowGrains:
            return "Snow grains";

        case WeatherCondition::SlightRainShowers:
            return "Slight rain showers";
        case WeatherCondition::ModerateRainShowers:
            return "Moderate rain showers";
        case WeatherCondition::ViolentRainShowers:
            return "Violent rain showers";

        case WeatherCondition::SlightSnowShowers:
            return "Slight snow showers";
        case WeatherCondition::HeavySnowShowers:
            return "Heavy snow showers";

        case WeatherCondition::Thunderstorm:
            return "Thunderstorm";
        case WeatherCondition::ThunderstormWithSlightHail:
            return "Thunderstorm with slight hail";
        case WeatherCondition::ThunderstormWithHeavyHail:
            return "Thunderstorm with heavy hail";

        default:
            return "Unknown";
    }
}

time_t Weather::parseTimeStamp(const char* value)
{
    if (value == nullptr)
        return 0;

    struct tm timeinfo = {};

    timeinfo.tm_year = (value[0] - '0') * 1000 +
                        (value[1] - '0') * 100 +
                        (value[2] - '0') * 10 +
                        (value[3] - '0') - 1900;

    timeinfo.tm_mon = (value[5] - '0') * 10 +
                        (value[6] - '0') - 1;

    timeinfo.tm_mday = (value[8] - '0') * 10 +
                        (value[9] - '0');

    timeinfo.tm_hour = (value[11] - '0') * 10 +
                        (value[12] - '0');

    timeinfo.tm_min = (value[14] - '0') * 10 +
                        (value[15] - '0');

    timeinfo.tm_sec = 0;

    return mktime(&timeinfo);
}
