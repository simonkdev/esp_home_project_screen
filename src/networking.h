#pragma once

#include <WiFi.h>
#include <Arduino.h>

bool setupWiFi(uint32_t timeoutMs = 20000);
void testWiFi(void);
void disableWiFi(void);
