#ifndef BSP_h
#define BSP_h
#include <stdint.h> 

//--------------------------
// UNO/Nano SPI pins:
// SCK 13
// MISO 12
// MOSI 11
// SS 10
// --------------------------

// Radio pins NRF24L01 transceiver --------
const uint64_t RADIO_PIPE_ID = 0xE8E8F0F0E1LL;
const uint8_t NRF24L01_CE_PIN = 9; // white
const uint8_t NRF24L01_CSN_PIN = 10; // brown
const uint8_t RADIO_BUFFER_SIZE = 9;
// ----------------------------------------
// Ultra-Sonic sensor LM324 - (HC-SR04) ---
const uint8_t LM324_TrigPin = A1;  // white
const uint8_t LM324_EchoPin = A0;  // orange
// ----------------------------------------
// H-Bridge - Motor Controls -----------------------------
const uint8_t L298N_HB_RightSideEN  = 3;  // PWM ENA [GRAY]
const uint8_t L298N_HB_RightSideFWD = 4;  // Green   [IN1]
const uint8_t L298N_HB_RightSideBWD = 2;  // Blue    [IN2]
//-
const uint8_t L298N_HB_LeftSideEN   = 6;  // PWM ENB [red]
const uint8_t L298N_HB_LeftSideFWD  = 8;  // Grey    [IN4]
const uint8_t L298N_HB_LeftSideBWD  = 7;  // Purple  [IN3]
// --------------------------------------------------------
#endif