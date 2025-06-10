#include "globals.h"

//Connections
bool mqttConnected = false;

//RTC
char isoTime[32] = "2025-05-21T00:00:00Z";

// Plant 1 state variables
float luminosity1_state = 0.0;
float humidity1_state = 0.0;
float temp_1 = 0.0;
bool valve1_state = false;
int led_intensity1_state = 0;
int mode1_state = 0;

// Plant 1 event variables
float luminosity1_event = 0.0;
float humidity1_event = 0.0;
bool valve1_event = false;
int led_intensity1_event = 0;
int mode1_event = 0;

// Plant 2 state variables
float luminosity2_state = 0.0;
float humidity2_state = 0.0;
float temp_2 = 0.0;
bool valve2_state = false;
int led_intensity2_state = 0;
int mode2_state = 0;

// Plant 2 event variables
float luminosity2_event = 0.0;
float humidity2_event = 0.0;
bool valve2_event = false;
int led_intensity2_event = 0;
int mode2_event = 0;

// Error flags
bool error_1 = false;
bool error_2 = false;

// Pump
bool pump_state = false;
bool pump_event = false;

// Change flags
bool plant1_changed = false;
bool plant2_changed = false;
bool pump_changed = false;

// Correction and calibration
float c1_temp = 10.01;
float c2_temp = 10.01;
float c1_hum = 0.0;
float c2_hum = 0.0;

// IDs
int IoT_id = 1;
int plant1_id = 1;
int plant2_id = 2;