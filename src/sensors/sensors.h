#pragma once

float readTemperature(int pin, int id);
float readLuminosity(int pin);
float readHumidity(int pin);
void updateSensorReadings();
float fmap(float x, float in_min, float in_max, float out_min, float out_max);
