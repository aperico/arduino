
#include <stdint.h> 
#include "typedefs.h"
#include "bsp.h"
#include "Motor.h"
#include "Radio.h"
#include "DistanceSensor.h"
#include "Lights.h"
#include "CarApp.h"


// System private variables --------
static const Motor motorLeft(L298N_HB_RightSideEN, L298N_HB_RightSideFWD, L298N_HB_RightSideBWD);
static const Motor motorRight(L298N_HB_LeftSideEN, L298N_HB_LeftSideBWD, L298N_HB_LeftSideFWD);
static const Radio radio(RADIO_PIPE_ID, NRF24L01_CE_PIN, NRF24L01_CSN_PIN);
static const DistanceSensor distSensorFront(LM324_TrigPin, LM324_EchoPin);

LightsInitStruct_t lightsInitStruct = {
  GPIO_BRAKE_LIGHT
};
static const Lights lights(&lightsInitStruct);

const static CartAppInitStruct_t initStruct = {
 &motorRight,
 &motorLeft,
 &radio,
 &distSensorFront,
 &lights
};

static CarApp carApp(&initStruct);

void setup() {
  Serial.begin(9600);
  carApp.init();
}


unsigned long lastTask = 0;
const unsigned long interval = 10; // 10ms for fast response

void loop() {
  if (millis() - lastTask >= interval) {
    lastTask = millis();
    carApp.task();
  }
}



