#pragma once

#include <Arduino.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>


class Calendar
{

public:

    struct Event
    {
        String title;

        time_t start;
        time_t end;

        bool allDay;
    };


public:

    Calendar(const char* url);

    bool begin();

    bool update();

    uint8_t eventCount() const;

    const Event& event(uint8_t index) const;

    void testCalendar();


private:

    bool downloadICS(String& data);

    bool parseICS(const String& data);

    time_t parseICSDate(const String& value);

    void sortEvents();

    time_t startOfWeek();
    time_t endOfWeek();


private:

    const char* calendarUrl;

    static constexpr uint8_t MAX_EVENTS = 32;

    Event events[MAX_EVENTS];

    uint8_t count = 0;

    static constexpr time_t HISTORY_WINDOW = 7 * 24 * 60 * 60;
};
