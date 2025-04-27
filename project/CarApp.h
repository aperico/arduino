#ifndef CarApp_h
#define CarApp_h

#include <stdint.h>
#include "Motor.h"
#include "Radio.h"
#include "Lights.h"
#include "DistanceSensor.h"

typedef struct{
  const Motor * motorRight;
  const Motor * motorLeft;
  const Radio * radio;
  const DistanceSensor * distSensorFront;
  const Lights * lights;
} CartAppInitStruct_t;

typedef struct {
  uint8_t x = 126; // joystick potentiomeder midpoint
  uint8_t y = 126; // joystick potentiomeder midpoint
  uint8_t bt_1 = 0;
  uint8_t bt_2 = 0;
  uint8_t bt_3 = 0;
  uint8_t bt_4 = 0;
  uint8_t bt_5 = 0;
  uint8_t bt_6 = 0;
  uint8_t bt_7 = 0;
} REMOTE_CTRL_DATA_t;

typedef enum : uint8_t {
  DRV_COAST=0,
  DRV_BRAKE,
  DRV_FWD,
  DRV_BWD,
  DRV_LEFT,
  DRV_RIGHT,
  DRV_FWD_LEFT,
  DRV_FWD_RIGHT,
  DRV_BWD_LEFT,
  DRV_BWD_RIGHT
} DRIVE_DIRECTION_t;


class CarApp{
  public:
    CarApp(const CartAppInitStruct_t * iff);
    void init(void);
    void task(void);

  private:
    const Motor * motorRight;
    const Motor * motorLeft;
    const Radio * radio;
    const DistanceSensor * distSensorFront;
    const Lights * lights;
    REMOTE_CTRL_DATA_t remoteCtrlData;
    void driveMotors(const DRIVE_DIRECTION_t direction);

    void processRemoteControlBuffer(uint16_t buffer[RADIO_BUFFER_SIZE]);
    void driveFWD(const uint8_t speed);
    void driveBWD(const uint8_t speed);
    void driveLFT(const uint8_t speed);
    void driveRGT(const uint8_t speed);
    void driveFWD_LEFT(const uint8_t speed);
    void driveFWD_RIGHT(const uint8_t speed);
    void driveBWD_LEFT(const uint8_t speed);
    void driveBWD_RIGHT(const uint8_t speed);
    void driveCoast(void);
    void driveMotorStop(void);

    DRIVE_DIRECTION_t getDriveDirection(const uint8_t x, const uint8_t y);
    bool isXActive(const uint8_t x);
    bool isYActive(const uint8_t y);

};



#endif

