#pragma once

#include <stdint.h>

class Motor {
public:
	Motor(const uint8_t en, const uint8_t in1, const uint8_t in2);
	void init(void);
	void rotateForward(const uint8_t speed);
	void rotateBackward(const uint8_t speed);
	void coast(void);
	void stop(void);
private:
	const uint8_t PIN_EN;
	const uint8_t PIN_IN1;
	const uint8_t PIN_IN2;
};






