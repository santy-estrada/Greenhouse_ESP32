#pragma once

// Connection
extern bool mqttConnected;

//RTC
extern char isoTime[32]; // ISO8601 time string, updated by RTC

// Plant 1 state variables
extern float luminosity1_state;
extern float humidity1_state;
extern float temp_1;
extern bool valve1_state;
extern int led_intensity1_state;
extern int mode1_state;

// Plant 1 event variables
extern float luminosity1_event;
extern float humidity1_event;
extern bool valve1_event;
extern int led_intensity1_event;
extern int mode1_event;

// Plant 2 state variables
extern float luminosity2_state;
extern float humidity2_state;
extern float temp_2;
extern bool valve2_state;
extern int led_intensity2_state;
extern int mode2_state;

// Plant 2 event variables
extern float luminosity2_event;
extern float humidity2_event;
extern bool valve2_event;
extern int led_intensity2_event;
extern int mode2_event;

// Error flags
extern bool error_1;
extern bool error_2;

// Pump
extern bool pump_state;
extern bool pump_event;

// Change flags
extern bool plant1_changed;
extern bool plant2_changed;
extern bool pump_changed;

// Correction and calibration
extern float c1_temp;
extern float c2_temp;
extern float c1_hum;
extern float c2_hum;

// IDs
extern int IoT_id;
extern int plant1_id;
extern int plant2_id;