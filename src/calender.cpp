#include "calender.h"

Calendar::Calendar(const char* url) : calendarUrl(url)
{
}

bool Calendar::begin()
{
    count = 0;
    return true;
}

bool Calendar::downloadICS(String& data)
{
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    if(!http.begin(client, calendarUrl))
    {
        Serial.println("Calendar HTTPS begin failed");
        return false;
    }
    int status = http.GET();

    if(status != HTTP_CODE_OK)
    {
        Serial.print("Calendar HTTP error: ");
        Serial.println(status);

        http.end();
        return false;
    }

    data = http.getString();
    http.end();

    Serial.print("Downloaded calendar bytes: ");
    Serial.println(data.length());

    return true;
}

bool Calendar::update()
{
    String data;

    if (!downloadICS(data))
        {
            return false;
        }


    count = 0;


    if (!parseICS(data))
    {
        return false;
    }


    sortEvents();


    return true;
}

uint8_t Calendar::eventCount() const
{
    return count;
}


const Calendar::Event& Calendar::event(uint8_t index) const
{
    return events[index];
}

void Calendar::testCalendar()
{
    Serial.println("Starting calendar test...");

    if (!begin())
    {
        Serial.println("Calendar begin failed");
        return;
    }

    if (!update())
    {
        Serial.println("Calendar update failed");
        return;
    }

    Serial.println("Calendar update OK");

    for (uint8_t i = 0; i < eventCount(); i++)
    {
        const Event& e = event(i);

        Serial.println("----------------");

        Serial.print("Title: ");
        Serial.println(e.title);

        Serial.print("All day: ");
        Serial.println(e.allDay ? "YES" : "NO");

        Serial.print("Start: ");
        Serial.println(ctime(&e.start));

        Serial.print("End: ");
        Serial.println(ctime(&e.end));
    }

    Serial.println("Calendar test finished");
}

bool Calendar::parseICS(const String& ics)
{
    count = 0;

    int pos = 0;

    while (count < MAX_EVENTS)
    {
        int eventStart = ics.indexOf("BEGIN:VEVENT", pos);

        if (eventStart < 0)
            break;


        int eventEnd = ics.indexOf("END:VEVENT", eventStart);

        if (eventEnd < 0)
            break;


        String block = ics.substring(eventStart, eventEnd);


        Event event;

        event.allDay = false;
        event.start = 0;
        event.end = 0;


        // SUMMARY
        int summaryPos = block.indexOf("SUMMARY:");

        if (summaryPos >= 0)
        {
            int lineEnd = block.indexOf("\n", summaryPos);

            if (lineEnd < 0)
                lineEnd = block.length();

            event.title = block.substring(summaryPos + 8, lineEnd);
            event.title.trim();
            event.title.replace("\\,", ",");
            event.title.replace("\\;", ";");
            event.title.replace("\\n", " ");
            event.title.replace("\\\\", "\\");
        }


        // DTSTART
        int startPos = block.indexOf("DTSTART");

        if (startPos >= 0)
        {
            int colon = block.indexOf(":", startPos);

            if (colon >= 0)
            {
                String value = block.substring(colon + 1);
                value.trim();

                if (value.length() == 8)
                {
                    // YYYYMMDD = all day event
                    event.allDay = true;
                }

                event.start = parseICSDate(value);
            }
        }


        // DTEND
        int endPos = block.indexOf("DTEND");

        if (endPos >= 0)
        {
            int colon = block.indexOf(":", endPos);

            if (colon >= 0)
            {
                String value = block.substring(colon + 1);
                value.trim();

                event.end = parseICSDate(value);
            }
        }

        // Detect Proton-style all-day events
        if (event.start != 0 && event.end != 0)
        {
            time_t duration = event.end - event.start;

            if (duration == 86400)
            {
                struct tm startTm;
                struct tm endTm;

                localtime_r(&event.start, &startTm);
                localtime_r(&event.end, &endTm);

                if (startTm.tm_min == 0 &&
                    startTm.tm_sec == 0 &&
                    endTm.tm_min == 0 &&
                    endTm.tm_sec == 0 &&
                    startTm.tm_hour == endTm.tm_hour)
                {
                    event.allDay = true;
                }
            }
        }


        time_t weekStart = startOfWeek();
        time_t weekEnd   = endOfWeek();


        if (event.end >= weekStart &&
            event.start <= weekEnd)
        {
            if (count < MAX_EVENTS)
            {
                events[count++] = event;
            }
        }


        pos = eventEnd + 10;
    }


    return count > 0;
}

void Calendar::sortEvents()
{
    for (uint8_t i = 0; i < count - 1; i++)
    {
        for (uint8_t j = i + 1; j < count; j++)
        {
            if (events[j].start < events[i].start)
            {
                Event temp = events[i];

                events[i] = events[j];

                events[j] = temp;
            }
        }
    }
}

time_t Calendar::parseICSDate(const String& value)
{
    struct tm tm;

    memset(&tm, 0, sizeof(tm));


    tm.tm_year = value.substring(0,4).toInt() - 1900;
    tm.tm_mon  = value.substring(4,6).toInt() - 1;
    tm.tm_mday = value.substring(6,8).toInt();


    if (value.length() >= 15)
    {
        tm.tm_hour = value.substring(9,11).toInt() - 1;
        tm.tm_min  = value.substring(11,13).toInt();
        tm.tm_sec  = value.substring(13,15).toInt();
    }


    return mktime(&tm);
}

time_t Calendar::startOfWeek()
{
    time_t now = time(nullptr);

    struct tm tm;
    localtime_r(&now, &tm);

    // Move back to Monday
    int daysSinceMonday = (tm.tm_wday + 6) % 7;

    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    tm.tm_mday -= daysSinceMonday;

    return mktime(&tm);
}

time_t Calendar::endOfWeek()
{
    time_t start = startOfWeek();

    struct tm tm;
    localtime_r(&start, &tm);

    // Sunday 23:59:59
    tm.tm_mday += 7;
    tm.tm_sec -= 1;

    return mktime(&tm);
}
