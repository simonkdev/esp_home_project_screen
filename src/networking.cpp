#include "networking.h"
#include "secrets.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

int WiFiStatus;

String Get_WiFiStatus(int Status){
    switch(Status){
        case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
        case WL_SCAN_COMPLETED:
        return "WL_SCAN_COMPLETED";
        case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
        case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST:
        return "WL_CONNECTION_LOST";
        case WL_CONNECTED:
        return "WL_CONNECTED";
        case WL_DISCONNECTED:
        return "WL_DISCONNECTED";
    }

    return "UNKNOWN";
}

bool setupWiFi(uint32_t timeoutMs)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFiStatus = WiFi.status();
    uint32_t startedAt = millis();

    while(WiFiStatus != WL_CONNECTED){
        if (millis() - startedAt >= timeoutMs)
        {
            Serial.print("WiFi connection timed out: ");
            Serial.println(Get_WiFiStatus(WiFiStatus));
            return false;
        }

        delay(250);
        WiFiStatus = WiFi.status();
    }

    Serial.println("WiFi connected");
    return true;
}

void testWiFi()
{
    if (setupWiFi())
    {
        Serial.print("Local IP: ");
        Serial.println(WiFi.localIP());
    }

    disableWiFi();
}

void disableWiFi()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi mode set to WIFI_OFF");
}
