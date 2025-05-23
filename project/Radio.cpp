#include "Radio.h"
#include <Arduino.h>

Radio::Radio(const uint64_t id, const uint8_t ce, const uint8_t csn) : PIPE_ID(id), rf24(ce, csn), BUFF_SIZE(sizeof(this->buffer))
{
}

bool Radio::isConnected()
{
  const unsigned long currentMillis = millis();
  if (currentMillis - this->lastSignalMs > this->INTERVAL_MS_SIGNAL_LOST)
  {
    return false;
  }
  else
  {
    return true;
  }
}

uint16_t *Radio::getBufferPtr()
{
  return this->buffer;
}

void Radio::init()
{
  this->rf24.begin();
  this->rf24.setPALevel(RF24_PA_HIGH);
  this->rf24.setDataRate(RF24_250KBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
  this->rf24.openReadingPipe(0, this->PIPE_ID);
  this->rf24.setAutoAck(false); //(true|false)
  this->rf24.startListening();

  delay(500);
  if (this->rf24.isChipConnected())
  {
    Serial.println("CHIP CONNECTED");
  }
  else
  {
    Serial.println("CHIP NOT CONNECTED");
  }
  this->resetBuffer();
}

void Radio::task(void)
{
  const unsigned long currentMillis = millis();
  if (this->rf24.available())
  {
    this->rf24.read(this->buffer, this->BUFF_SIZE);

    if (currentMillis - this->lastSignalMs > this->INTERVAL_MS_SIGNAL_LOST)
    {
      this->lostConnection();
    }
    this->lastSignalMs = currentMillis;
  }
}

void Radio::resetBuffer(void)
{
  memset(this->buffer, 0, this->BUFF_SIZE);
}

void Radio::lostConnection()
{
  this->resetBuffer();
  this->init();
  delay(this->INTERVAL_MS_SIGNAL_RETRY);
}