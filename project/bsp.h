#pragma once

#include <stdint.h> 

//--------------------------
// UNO/Nano SPI pins:
// SCK 13
// MISO 12
// MOSI 11
// SS 10
// --------------------------

extern const uint64_t RADIO_PIPE_ID;
extern const uint8_t NRF24L01_CE_PIN;
extern const uint8_t NRF24L01_CSN_PIN;
extern const uint8_t RADIO_BUFFER_SIZE;
extern const uint8_t LM324_TrigPin;
extern const uint8_t LM324_EchoPin;
extern const uint8_t L298N_HB_RightSideEN;
extern const uint8_t L298N_HB_RightSideFWD;
extern const uint8_t L298N_HB_RightSideBWD;
extern const uint8_t L298N_HB_LeftSideEN;
extern const uint8_t L298N_HB_LeftSideFWD;
extern const uint8_t L298N_HB_LeftSideBWD;
extern const uint8_t GPIO_BRAKE_LIGHT;

