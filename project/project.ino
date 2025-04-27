
#include <stdint.h> // For C-style includes
#include "bsp.h"
#include "typedefs.h"
#include "Motor.h"
#include "Radio.h"
#include "DistanceSensor.h"
#include "CarApp.h"


// System private variables --------
static const Motor motorLeft(L298N_HB_RightSideEN, L298N_HB_RightSideFWD, L298N_HB_RightSideBWD);
static const Motor motorRight(L298N_HB_LeftSideEN, L298N_HB_LeftSideBWD, L298N_HB_LeftSideFWD);
static const Radio radio(RADIO_PIPE_ID, NRF24L01_CE_PIN, NRF24L01_CSN_PIN);
static const DistanceSensor distSensorFront(LM324_TrigPin, LM324_EchoPin);

const static CartAppInitStruct_t initStruct = {
 &motorRight,
 &motorLeft,
 &radio,
 &distSensorFront
};

static CarApp carApp(&initStruct);

void setup() {
  Serial.begin(115200);
  carApp.init();
}

void loop() {
  carApp.task();
  delay(50);
}



