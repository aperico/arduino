
#include <SPI.h>
#include "Motor.h"
#include "bsp.h"
#include <Arduino.h>

Motor::Motor(const uint8_t en, const uint8_t in1, const uint8_t in2): PIN_EN(en), PIN_IN1(in1), PIN_IN2(in2){
  
}

void Motor::init(){
  pinMode(this->PIN_EN, OUTPUT); 
  pinMode(this->PIN_IN1, OUTPUT);
  pinMode(this->PIN_IN2, OUTPUT);
}
void Motor::rotateForward(const uint8_t speed){
  digitalWrite(this->PIN_IN1, LOW);
  digitalWrite(this->PIN_IN2, HIGH);
  analogWrite(this->PIN_EN, speed);
}

void Motor::rotateBackward(const uint8_t speed){
  digitalWrite(this->PIN_IN1, HIGH);
  digitalWrite(this->PIN_IN2, LOW);
  analogWrite(this->PIN_EN, speed);
}

void Motor::coast(void){
  digitalWrite(this->PIN_IN1, LOW);
  digitalWrite(this->PIN_IN2, LOW);
  analogWrite(this->PIN_EN, 0);
}
