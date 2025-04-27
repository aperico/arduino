#pragma once

#include <stdint.h>
#include <Arduino.h>

typedef struct {
  uint8_t brakeLightPin;
} LightsInitStruct_t;

typedef enum : uint8_t {
  LIGHT_OFF = 0,
  LIGHT_ON = 1
} LIGHT_STATE_t;

typedef enum : uint8_t {
  LIGHT_BRAKE = 0,
  LIGHT_TURN_LEFT = 1,
  LIGHT_TURN_RIGHT = 2,
  LIGHT_HAZARD = 3
} LIGHT_TYPE_t;

class Lights {
private:
  // Pin definitions
  const LightsInitStruct_t *iff;
  void setLightState(const LIGHT_TYPE_t lightType, const LIGHT_STATE_t state, const uint8_t pin);
  
public:
  Lights(const LightsInitStruct_t * iff);
  void init(void);
  void task(void);
  void brakeLightOn(void);
  void brakeLightOff(void);
};

