#include "sensors.h"
#include <Arduino.h>
#include "./globals/globals.h"
#include "./env/env.h"
#include "mqtt_json/mqtt_json.h"
#include "rtc/rtc.h"

//#define DEBUG
//#define TEST_MODE


float readTemperature(int pin, int id) {
#ifdef TEST_MODE
    float temp = random(10, 40);
    #ifdef DEBUG
    Serial.printf("[MOCK] readTemperature(pin=%d, id=%d) = %.2f\n", pin, id, temp);
    #endif
    return temp;
#else
    u_int16_t rawValue = 0;
    for (int i = 0; i < 10; i++) {
        rawValue += analogRead(pin);
    }
    float voltage = (rawValue / 4095.0) * 3300 / 10; // mV average
    float temperature = voltage / 10; // Celsius
    float correction = (id == 1) ? c1_temp : c2_temp;
    #ifdef DEBUG
    Serial.printf("readTemperature(pin=%d, id=%d): voltage=%.2f, temp=%.2f, correction=%.2f\n", pin, id, voltage, temperature, correction);
    #endif
    return temperature + correction;
#endif
}

float readLuminosity(int pin) {
#ifdef TEST_MODE
    float lum = random(0, 100);
    #ifdef DEBUG
    Serial.printf("[MOCK] readLuminosity(pin=%d) = %.2f\n", pin, lum);
    #endif
    return lum;
#else
    u_int16_t rawValue = 0;
    for (int i = 0; i < 10; i++) {
        rawValue += analogRead(pin);
    }
    float voltage = (rawValue / 4095.0) * 3300 / 10;
    float lum = fmap(voltage, 3300.0, 0.0, 0.0, 100.0); // Inverse mapping
    #ifdef DEBUG
    Serial.printf("readLuminosity(pin=%d): voltage=%.2f, lum=%.2f\n", pin, voltage, lum);
    #endif
    return lum;
#endif
}

float readHumidity(int pin, int id) {
#ifdef TEST_MODE
    float hum = random(0, 100);
    #ifdef DEBUG
    Serial.printf("[MOCK] readHumidity(pin=%d) = %.2f\n", pin, hum);
    #endif
    return hum;
#else
    u_int16_t rawValue = 0;
    for (int i = 0; i < 10; i++) {
        rawValue += analogRead(pin);
    }
    float voltage = (rawValue / 4095.0) * 3300 / 10;
    float hum = fmap(voltage, 3300.0, 0.0, 0.0, 100.0); // Direct mapping
    #ifdef DEBUG
    Serial.printf("readHumidity(pin=%d): voltage=%.2f, hum=%.2f\n", pin, voltage, hum);
    #endif
    float correction = (id == 1) ? c1_hum : c2_hum;
    hum += correction; // Apply correction
    return hum;
#endif
}

void updateSensorReadings() {
#ifdef TEST_MODE
    // Simulate sensor values
    luminosity1_state = random(0, 100);
    humidity1_state = random(0, 100);
    temp_1 = random(0, 60);

    luminosity2_state = random(0, 100); 
    humidity2_state = random(0, 100);
    temp_2 = random(0, 60);

    error_1 = (temp_1 > 50 || temp_1 < 5);
    error_2 = (temp_2 > 50 || temp_2 < 5);
#else
    // Real sensor code
    luminosity1_state = readLuminosity(LDR_1);
    humidity1_state = readHumidity(SEN0114_1, 1);
    temp_1 = readTemperature(LM35_1, 1);

    luminosity2_state = readLuminosity(LDR_2);
    humidity2_state = readHumidity(SEN0114_2, 2);
    temp_2 = readTemperature(LM35_2, 2);

    error_1 = (temp_1 > 50 || temp_1 < 5);
    error_2 = (temp_2 > 50 || temp_2 < 5);
#endif

    if(mqttConnected){
        updateIsoTime();
        writeJsonEntry(luminosity1_state, humidity1_state, temp_1, 1, isoTime, valve1_state, pump_state, led_intensity1_state, mode1_state);
        writeJsonEntry(luminosity2_state, humidity2_state, temp_2, 2, isoTime, valve2_state, pump_state, led_intensity2_state, mode2_state);

        if(error_1){
            updatePlantError(1, true);
        }else{
            updatePlantError(1, false);
        }

        if(error_2){
            updatePlantError(2, true);
        }else{
            updatePlantError(2, false);
        }

#ifdef DEBUG
        Serial.println("Event variables:");
        Serial.print("luminosity1_event: "); Serial.println(luminosity1_event);
        Serial.print("humidity1_event: "); Serial.println(humidity1_event);
        Serial.print("valve1_event: "); Serial.println(valve1_event);
        Serial.print("led_intensity1_event: "); Serial.println(led_intensity1_event);

        Serial.print("luminosity2_event: "); Serial.println(luminosity2_event);
        Serial.print("humidity2_event: "); Serial.println(humidity2_event);
        Serial.print("valve2_event: "); Serial.println(valve2_event);
        Serial.print("led_intensity2_event: "); Serial.println(led_intensity2_event);

        Serial.print("pump_event: "); Serial.println(pump_event);
        Serial.print("pump_state: "); Serial.println(pump_state);

        Serial.println("--------------------");
#endif
    }
}

// Helper for mapping (if not already defined elsewhere)
float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}