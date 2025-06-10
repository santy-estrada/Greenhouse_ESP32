#include "mqtt_json.h"
#include <ArduinoJson.h>
#include "./env/env.h"

#define DEBUG

// MQTT Topics
const char *messageTopic = MQTT_MESSAGE_TOPIC;
const char *sendDataTopic = MQTT_TEST_TOPIC;
const char *errorStatusTopic = MQTT_REAL_TOPIC;
const char *syncTopic = MQTT_SYNC_TOPIC;
const char *changesTopic = MQTT_COMMAND_TOPIC;

// MQTT Client
PubSubClient client;

// Forward declaration
void readJson(char* json, char* topic);

void callback(char *topic, byte *payload, unsigned int length) {
    #ifdef DEBUG
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
    #endif
    if (strcmp(topic, MQTT_COMMAND_TOPIC) == 0 || strcmp(topic, MQTT_SYNC_TOPIC) == 0) {
      const size_t jsonBufferSize = 2048;
      char json[jsonBufferSize];
      size_t copyLen = (length < jsonBufferSize - 1) ? length : (jsonBufferSize - 1);
      memcpy(json, payload, copyLen);
      json[copyLen] = '\0'; // Null-terminate the string
      readJson(json, topic);
    }
}

void mqttJsonInit(const char* mqtt_broker, int mqtt_port, const char* mqtt_username, const char* mqtt_password, WiFiClient& espClient) {
    client.setClient(espClient);
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("MQTT broker connected");
            mqttConnected = true;
        } else {
            Serial.print("failed with state ");
            Serial.println(client.state());
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
    client.publish(messageTopic, "Hi, I'm ESP32 ^^");
    client.subscribe(messageTopic);
    client.subscribe(syncTopic);
    client.subscribe(changesTopic);
}

void mqttJsonLoop() {
    client.loop();
}

void updateIoTError(int id){
    StaticJsonDocument<64> iotErrorDoc;
    iotErrorDoc["iot_dev_id"] = id;
    char output[64];
    size_t len = serializeJson(iotErrorDoc, output, sizeof(output));
    output[len] = '\0';
    Serial.println("Error updated");
    Serial.println(output);
    client.publish(errorStatusTopic, output, len);
}

void updatePlantError(int id, bool error){
    StaticJsonDocument<64> plantErrorDoc;
    plantErrorDoc["plant_id"] = id;
    plantErrorDoc["error"] = error;
    char output[64];
    size_t len = serializeJson(plantErrorDoc, output, sizeof(output));
    output[len] = '\0';
    Serial.println("Error updated");
    Serial.println(output);
    client.publish(errorStatusTopic, output, len);
}

void writeJsonEntry(float luminosity_state, float humidity_state, float temperature, int plant_id, const char* entryCreation_time, bool valve_state, bool pump_state, int led_intensity_state, int mode_state){
    StaticJsonDocument<256> entryDoc;
    entryDoc["luminosity_state"] = luminosity_state;
    entryDoc["humidity_state"] = humidity_state;
    entryDoc["temperature"] = temperature;
    entryDoc["plant_id"] = plant_id;
    entryDoc["entryCreation_time"] = entryCreation_time;
    entryDoc["valve_state"] = valve_state;
    entryDoc["pump_state"] = pump_state;
    entryDoc["led_intensity_state"] = led_intensity_state;
    entryDoc["mode_state"] = mode_state;
    char buffer[256];
    size_t len = serializeJson(entryDoc, buffer, sizeof(buffer));
    buffer[len] = '\0';
    Serial.println("Entry created");
    Serial.println(buffer);
    client.publish(sendDataTopic, buffer, len);
}

// Your readJson function here (copy from your main.cpp)
void readJson(char* json, char* topic) {
  StaticJsonDocument<2048> inputDoc;
  DeserializationError error = deserializeJson(inputDoc, json);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  if (inputDoc.isNull()) {
    Serial.println(F("Deserialization failed"));
    return;
  }

  if(strcmp(topic, MQTT_COMMAND_TOPIC) == 0){
    // Handle the command topic which updates changes
    Serial.println("Received command topic");
    const char* type = inputDoc["type"];
    int id = inputDoc["id"];
    if(id == 1 && strcmp(type, "plantEvent") == 0) plant1_changed = true;
    else if(id == 2) plant2_changed = true;
    #ifdef DEBUG
    Serial.print("type: ");
    Serial.println(type);
    Serial.print("id: ");
    Serial.println(id);
    #endif
    
    if (strcmp(type, "plantEvent") == 0) {
      // Retain previous values if the passed values are 0
      float luminosity_event = inputDoc["changes"]["luminosity_event"];
      float humidity_event = inputDoc["changes"]["humidity_event"];
      bool valve_event = inputDoc["changes"]["valve_event"];
      int led_intensity_event = inputDoc["changes"]["led_intensity_event"];
      int mode = inputDoc["changes"]["mode"];

      // Use previous values if -1 is received
      if (luminosity_event == -1) {
        if (id == 1) luminosity_event = luminosity1_event;
        else if (id == 2) luminosity_event = luminosity2_event;
      }
      if (humidity_event == -1) {
        if (id == 1) humidity_event = humidity1_event;
        else if (id == 2) humidity_event = humidity2_event;
      }
      if (led_intensity_event == -1) {
        if (id == 1) led_intensity_event = led_intensity1_event;
        else if (id == 2) led_intensity_event = led_intensity2_event;
      }
      if (mode == 0) {
        if (id == 1) mode = mode1_event;
        else if (id == 2) mode = mode2_event;
      }
      // For valve_event, only update if the key exists (since bool can't distinguish 0/false from "not set")
      if (!inputDoc["changes"].containsKey("valve_event")) {
        if (id == 1) valve_event = valve1_event;
        else if (id == 2) valve_event = valve2_event;
      }
      // Update the correct plant's event variables based on id
      if (id == 1) {
        #ifdef DEBUG
        Serial.println("Updating plant 1 event variables");
        #endif
        luminosity1_event = luminosity_event;
        humidity1_event = humidity_event;
        valve1_event = valve_event;
        led_intensity1_event = led_intensity_event;
        mode1_event = mode;
      } else if (id == 2) {
        #ifdef DEBUG
        Serial.println("Updating plant 2 event variables");
        #endif
        luminosity2_event = luminosity_event;
        humidity2_event = humidity_event;
        valve2_event = valve_event;
        led_intensity2_event = led_intensity_event;
        mode2_event = mode;
      }
      #ifdef DEBUG
      Serial.print("luminosity_event: ");
      Serial.println(luminosity_event);
      Serial.print("humidity_event: ");
      Serial.println(humidity_event);
      Serial.print("valve_event: ");
      Serial.println(valve_event ? "true" : "false");
      Serial.print("led_intensity_event: ");
      Serial.println(led_intensity_event);
      Serial.print("mode: ");
      Serial.println(mode);
      #endif
    }else if (strcmp(type, "iotDevEvent") == 0){
      pump_event = inputDoc["changes"]["pump_event"];
      pump_changed = true;
      #ifdef DEBUG
      Serial.print("pump_event: ");
      Serial.println(pump_event ? "true" : "false");
      #endif
    }

  } else if(strcmp(topic, MQTT_SYNC_TOPIC) == 0){
    #ifdef DEBUG
    Serial.println("Received sync topic");
    #endif

    // --- Update plant event variables ---
    if (inputDoc.containsKey("plantEvents")) {
        for (JsonObject plantEvent : inputDoc["plantEvents"].as<JsonArray>()) {
            int id = plantEvent["id"];
            float luminosity_event = plantEvent["luminosity_event"];
            float humidity_event = plantEvent["humidity_event"];
            bool valve_event = plantEvent["valve_event"];
            int led_intensity_event = plantEvent["led_intensity_event"];
            int mode = plantEvent["mode"];

            if (id == 1) {
                luminosity1_event = luminosity_event;
                humidity1_event = humidity_event;
                valve1_event = valve_event;
                led_intensity1_event = led_intensity_event;
                mode1_event = mode;
                #ifdef DEBUG
                Serial.println("Updated plant 1 event variables from sync");
                #endif
            } else if (id == 2) {
                luminosity2_event = luminosity_event;
                humidity2_event = humidity_event;
                valve2_event = valve_event;
                led_intensity2_event = led_intensity_event;
                mode2_event = mode;
                #ifdef DEBUG
                Serial.println("Updated plant 2 event variables from sync");
                #endif
            }
        }
    }

    // --- Update pump event variable ---
    if (inputDoc.containsKey("iotDevEvents")) {
        for (JsonObject iotDevEvent : inputDoc["iotDevEvents"].as<JsonArray>()) {
            int id = iotDevEvent["id"];
            bool pump_event_val = iotDevEvent["pump_event"];
            if (id == 1) {
                pump_event = pump_event_val;
                pump_changed = true;
                #ifdef DEBUG
                Serial.println("Updated pump_event from sync");
                #endif
            }
        }
    }
  }
}