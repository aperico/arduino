#include "DistanceSensor.h"
#include <Arduino.h>

DistanceSensor::DistanceSensor(const uint8_t trigPin, const uint8_t echoPin): PIN_TRIG(trigPin), PIN_ECHO(echoPin){
  
}

void DistanceSensor::task(){
  digitalWrite(this->PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(this->PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(this->PIN_TRIG, LOW);

  const float duration = pulseIn(this->PIN_ECHO, HIGH);
  this->distance = (duration * .0343) * 0.5;
}

void DistanceSensor::init(void){
  pinMode(this->PIN_TRIG, OUTPUT);
  pinMode(this->PIN_ECHO, INPUT);
}
float DistanceSensor::getDistance(void){
  return this->distance;
}
