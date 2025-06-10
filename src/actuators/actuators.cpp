#include "actuators.h"
#include <Arduino.h>
#include "./globals/globals.h"
#include "./env/env.h"

//#define DEBUG
//#define TEST_MODE

void setPWM(int channel, int duty_cycle) {
#ifdef TEST_MODE
    Serial.printf("[MOCK] setPWM(channel=%d, duty_cycle=%d)\n", channel, duty_cycle);
#else
    ledcWrite(channel, map(duty_cycle, 0, 100, 0, 255));
#endif
}

void setValve(int id, bool open) {
#ifdef TEST_MODE
    Serial.printf("[MOCK] setValve(id=%d, open=%s)\n", id, open ? "true" : "false");
    if (id == 1) valve1_state = open;
    else if (id == 2) valve2_state = open;
#else
    if (id == 1) {
        digitalWrite(VALVE_1, open ? HIGH : LOW);
        valve1_state = open;
    } else if (id == 2) {
        digitalWrite(VALVE_2, open ? HIGH : LOW);
        valve2_state = open;
    }
#endif
}

void setLedIntensity(int id, int intensity) {
#ifdef TEST_MODE
    Serial.printf("[MOCK] setLedIntensity(id=%d, intensity=%d)\n", id, intensity);
    if (id == 1) led_intensity1_state = intensity;
    else if (id == 2) led_intensity2_state = intensity;
#else
    if (id == 1) {
        setPWM(channel1, intensity);
        led_intensity1_state = intensity;
    } else if (id == 2) {
        setPWM(channel2, intensity);
        led_intensity2_state = intensity;
    }
#endif
}

void updatePumpState(bool pump) {
    if(!valve1_state && !valve2_state){
        pump_state = false;
        pump = false;
    }
#ifdef TEST_MODE
    Serial.printf("[MOCK] updatePumpState(pump=%s)\n", pump ? "true" : "false");
    pump_state = pump;
    Serial.printf("pump_state: %s, pump_event: %s, pump: %s\n", pump_state ? "true" : "false",  pump_event ? "true" : "false", pump ? "true" : "false");
#else
    digitalWrite(PUMP, pump ? HIGH : LOW);
    pump_state = pump;
#endif
}

void adjustLedForLuminosity(int id, float target_luminosity) {
#ifdef TEST_MODE
    Serial.printf("[MOCK] adjustLedForLuminosity(id=%d, target_luminosity=%.2f)\n", id, target_luminosity);
#endif
    float current_luminosity = (id == 1) ? luminosity1_state : luminosity2_state;
    int current_intensity = (id == 1) ? led_intensity1_state : led_intensity2_state;
    int new_intensity = current_intensity + (int)(target_luminosity - current_luminosity);

    if (new_intensity < 0) new_intensity = 0;
    if (new_intensity > 100) new_intensity = 100;

    setLedIntensity(id, new_intensity);
}

void adjustValveForHumidity(int id, float target_humidity) {
#ifdef TEST_MODE
    Serial.printf("[MOCK] adjustValveForHumidity(id=%d, target_humidity=%.2f)\n", id, target_humidity);
#endif
    float current_humidity = (id == 1) ? humidity1_state : humidity2_state;
    if (target_humidity > current_humidity) {
        setValve(id, true);
    } else {
        setValve(id, false);
    }
}

void updateActuatorStates(int mode, int id) {
    switch (mode) {
    case 1:
        setValve(id, (id == 1) ? valve1_event : valve2_event);
        setLedIntensity(id, (id == 1) ? led_intensity1_event : led_intensity2_event);
        break;
    case 2:
        setValve(id, (id == 1) ? valve1_event : valve2_event);
        adjustLedForLuminosity(id, (id == 1) ? luminosity1_event : luminosity2_event);
        break;
    case 3:
        adjustValveForHumidity(id, (id == 1) ? humidity1_event : humidity2_event);
        setLedIntensity(id, (id == 1) ? led_intensity1_event : led_intensity2_event);
        break;
    case 4:
        adjustValveForHumidity(id, (id == 1) ? humidity1_event : humidity2_event);
        adjustLedForLuminosity(id, (id == 1) ? luminosity1_event : luminosity2_event);
        break;
    default:
        break;
    }

    if(id == 1){
      mode1_state = mode;
    }else if(id == 2){
      mode2_state = mode;
    }
    updatePumpState(pump_event);

#ifdef DEBUG
    Serial.printf("updateActuatorStates: mode=%d, id=%d\n", mode, id);
    Serial.printf("Valve1: %d, Valve2: %d, LED1: %d, LED2: %d, Pump: %d\n",
        valve1_state, valve2_state, led_intensity1_state, led_intensity2_state, pump_state);
#endif
}