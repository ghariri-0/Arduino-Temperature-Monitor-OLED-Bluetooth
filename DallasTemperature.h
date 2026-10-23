#pragma once
#include "OneWire.h"
#define DEVICE_DISCONNECTED_C -127
class DallasTemperature {
public:
  DallasTemperature(OneWire*);
  void begin();
  void setResolution(uint8_t);
  void setWaitForConversion(bool);
  void requestTemperatures();
  float getTempCByIndex(uint8_t);
};
