#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

// Extern declarations for topics and client
extern PubSubClient client;
extern const char *messageTopic;
extern const char *sendDataTopic;
extern const char *errorStatusTopic;
extern const char *syncTopic;
extern const char *changesTopic;

extern bool mqttConnected;

// Event variables
extern float luminosity1_event;
extern float humidity1_event;
extern bool valve1_event;
extern int led_intensity1_event;
extern int mode1_event;

extern float luminosity2_event;
extern float humidity2_event;
extern bool valve2_event;
extern int led_intensity2_event;
extern int mode2_event;

extern bool pump_event;

extern bool plant1_changed;
extern bool plant2_changed;
extern bool pump_changed;

// Function declarations
void mqttJsonInit(const char* mqtt_broker, int mqtt_port, const char* mqtt_username, const char* mqtt_password, WiFiClient& espClient);
void mqttJsonLoop();
void updateIoTError(int id);
void updatePlantError(int id, bool error);
void writeJsonEntry(float luminosity_state, float humidity_state, float temperature, int plant_id, const char* entryCreation_time, bool valve_state, bool pump_state, int led_intensity_state, int mode_state);