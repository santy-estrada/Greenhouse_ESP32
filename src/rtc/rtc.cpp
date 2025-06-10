#include "rtc.h"
#include <WiFi.h>
#include "RTClib.h"
#include <time.h>
#include "../globals/globals.h"

RTC_DS3231 rtc;

void rtcInitAndSync() {
    // Init NTP
    configTime(0, 0, "pool.ntp.org"); // UTC
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        Serial.println("Waiting for NTP...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    time_t ntpTime = mktime(&timeinfo);

    // Init RTC (must be connected to I2C pins: SDA -> GPIO 21, SCL -> GPIO 22)
    if (!rtc.begin()) {
        Serial.println("RTC module is NOT found");
        Serial.flush();
        while (1);
    }
    DateTime rtcNow = rtc.now();
    time_t rtcTime = rtcNow.unixtime();

    // Compare and update if needed
    if (abs(ntpTime - rtcTime) > 60) {
        rtc.adjust(DateTime(ntpTime));
        Serial.println("RTC updated from NTP");
    } else {
        Serial.println("RTC is accurate");
    }
    updateIsoTime();
}

void updateIsoTime() {
    DateTime now = rtc.now();
    snprintf(
        isoTime, sizeof(isoTime),
        "%04d-%02d-%02dT%02d:%02d:%02dZ",
        now.year(), now.month(), now.day(),
        now.hour(), now.minute(), now.second()
    );
}