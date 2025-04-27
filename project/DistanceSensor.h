#ifndef DistanceSensor_h
#define DistanceSensor_h

#include <stdint.h>

class DistanceSensor {
public:
	DistanceSensor(const uint8_t trigPin, const uint8_t echoPin);
  void task(void);
  float getDistance(void);
  void init(void);
private:
	float distance = 0.0;
  const uint8_t PIN_TRIG;
  const uint8_t PIN_ECHO;

};

#endif
