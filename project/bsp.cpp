#include "bsp.h"
#include <Arduino.h>

const uint64_t RADIO_PIPE_ID = 0xE8E8F0F0E1LL;
const uint8_t NRF24L01_CE_PIN = 9;
const uint8_t NRF24L01_CSN_PIN = 10;
const uint8_t RADIO_BUFFER_SIZE = 9;
const uint8_t LM324_TrigPin = A1;
const uint8_t LM324_EchoPin = A0;
const uint8_t L298N_HB_RightSideEN  = 3;
const uint8_t L298N_HB_RightSideFWD = 4;
const uint8_t L298N_HB_RightSideBWD = 2;
const uint8_t L298N_HB_LeftSideEN   = 6;
const uint8_t L298N_HB_LeftSideFWD  = 8;
const uint8_t L298N_HB_LeftSideBWD  = 7;
const uint8_t GPIO_BRAKE_LIGHT = A2;