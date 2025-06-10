#pragma once

void setPWM(int channel, int duty_cycle);
void setValve(int id, bool open);
void setLedIntensity(int id, int intensity);
void updatePumpState(bool pump);
void adjustLedForLuminosity(int id, float target_luminosity);
void adjustValveForHumidity(int id, float target_humidity);
void updateActuatorStates(int mode, int id);