#include "Lights.h"

#include "bsp.h"
#include <Arduino.h>


Lights::Lights(const LightsInitStruct_t *iff) : iff(iff)
{
  
}

void Lights::init(void)
{
  pinMode(this->iff->brakeLightPin, OUTPUT);
  digitalWrite(this->iff->brakeLightPin, LOW);
  this->setLightState(LIGHT_BRAKE, LIGHT_OFF, iff->brakeLightPin);
}

void  Lights::setLightState(const LIGHT_TYPE_t lightType, const LIGHT_STATE_t state, const uint8_t pin) {
  if (state == LIGHT_ON) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

void Lights::brakeLightOn(void) {
  this->setLightState(LIGHT_BRAKE, LIGHT_ON, this->iff->brakeLightPin);
}
void Lights::brakeLightOff(void) {
  this->setLightState(LIGHT_BRAKE, LIGHT_OFF, this->iff->brakeLightPin);
}