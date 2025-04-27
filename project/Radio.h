#ifndef Radio_h
#define Radio_h

#include <stdint.h>
#include <nRF24L01.h>
#include <RF24.h>

#ifndef RADIO_BUFFER_SIZE
#define RADIO_BUFFER_SIZE 9
#endif

class Radio {
public:
	Radio(const uint64_t id, const uint8_t ce, const uint8_t csn);
	void init(void);
  void task(void);
  uint16_t * getBufferPtr(void);
  void lostConnection(void);
  bool isConnected();
private:
	const uint64_t PIPE_ID;

  uint16_t lastSignalMs=0;
  uint16_t buffer[RADIO_BUFFER_SIZE];
  const uint16_t BUFF_SIZE;
  RF24 rf24;
  const uint16_t INTERVAL_MS_SIGNAL_LOST = 2000;
  const uint16_t INTERVAL_MS_SIGNAL_RETRY = 1000;

  void resetBuffer(void);
};

#endif


