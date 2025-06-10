#pragma once

// Wifi
#define WIFI_SSID           "Nuremberg Trials" //"ESTRADA"
#define WIFI_PASSWORD       "War Crimes" //"71700622LF"

// MQTT
#define MQTT_BROKER         "192.168.228.132"//"broker.emqx.io"
#define MQTT_USERNAME       "emqx"
#define MQTT_PASSWORD       "public"
#define MQTT_PORT           1883

// MQTT Topics
#define MQTT_TEST_TOPIC     "iot/gh/states"
#define MQTT_REAL_TOPIC     "iot/gh/error"
#define MQTT_MESSAGE_TOPIC  "iot/gh/message"
#define MQTT_COMMAND_TOPIC  "iot/gh/command"
#define MQTT_SYNC_TOPIC     "iot/gh/sync"

// GPIO Pins
#define LM35_1 36
#define LDR_1 39
#define SEN0114_1 34
#define LM35_2 35
#define LDR_2 32
#define SEN0114_2 33

#define PUMP 4
#define VALVE_1 5
#define LED_1 18
#define VALVE_2 16
#define LED_2 17
extern int channel1;
extern int channel2;