#include <Arduino.h>
#include <WiFi.h>
#include "./env/env.h"
#include "globals/globals.h"
#include "sensors/sensors.h"
#include "actuators/actuators.h"
#include "mqtt_json/mqtt_json.h"
#include "rtc/rtc.h"

//#define DEBUG
//#define TEST_MODE  // Comment this line to use real hardware

WiFiClient espClient;

// Forward declarations for FreeRTOS tasks
void mqttTask(void * parameter);
void sensorTask(void * parameter);
void actuatorTask(void * parameter);

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      vTaskDelay(500 / portTICK_PERIOD_MS);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the Wi-Fi network");

    rtcInitAndSync();


    pinMode(LM35_1, INPUT);
    pinMode(LDR_1, INPUT);
    pinMode(SEN0114_1, INPUT);
    pinMode(LM35_2, INPUT);
    pinMode(LDR_2, INPUT);
    pinMode(SEN0114_2, INPUT);

    pinMode(PUMP, OUTPUT);
    pinMode(VALVE_1, OUTPUT);
    pinMode(VALVE_2, OUTPUT);
    digitalWrite(PUMP, LOW);
    digitalWrite(VALVE_1, LOW);
    digitalWrite(VALVE_2, LOW);

    ledcSetup(channel1, 5000, 8);
    ledcAttachPin(LED_1, channel1);
    ledcSetup(channel2, 5000, 8);
    ledcAttachPin(LED_2, channel2);

    ledcWrite(channel1, 0);
    ledcWrite(channel2, 0);


    vTaskDelay(1000 / portTICK_PERIOD_MS);

    xTaskCreate(mqttTask, "mqttTask", 8192, NULL, 2, NULL);
    xTaskCreate(sensorTask, "sensorTask", 2048, NULL, 1, NULL);
    xTaskCreate(actuatorTask, "actuatorTask", 2048, NULL, 3, NULL);
}

void loop() {
    // Nothing here, all logic is handled by tasks
}

// --- Tasks ---

void mqttTask(void * parameter) {
  mqttJsonInit(MQTT_BROKER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD, espClient);
  while (1) {
      mqttJsonLoop();
      vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void sensorTask(void * parameter) {
  while (1) {
      updateSensorReadings();
      vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

void actuatorTask(void * parameter) {
    while (1) {
#ifdef DEBUG
      Serial.print("mode1_event: "); Serial.println(mode1_event);
      Serial.print("mode1_state: "); Serial.println(mode1_state);
      Serial.print("mode2_event: "); Serial.println(mode2_event);
      Serial.print("mode2_state: "); Serial.println(mode2_state);
#endif
    if(mode1_event > 0 && mode1_event < 5 && mode1_event != mode1_state){
        updateActuatorStates(mode1_event, 1);
        plant1_changed = false;
    }
    if(mode2_event > 0 && mode2_event < 5 && mode2_event != mode2_state){
        updateActuatorStates(mode2_event, 2);
        plant2_changed = false;
    }
    if(plant1_changed){
        updateActuatorStates(mode1_state, 1);
        plant1_changed = false;
    }
    if(plant2_changed){
        updateActuatorStates(mode2_state, 2);
        plant2_changed = false;
    }
    if(pump_changed){
        updatePumpState(pump_event);
        pump_changed = false;
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}
}