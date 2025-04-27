#ifndef CarApp_h
#define CarApp_h
#include <stdint.h>
#include "Motor.h"
#include "CarApp.h"
#include "Radio.h"
#include "DistanceSensor.h"

typedef struct{
  const Motor * motorRight;
  const Motor * motorLeft;
  const Radio * radio;
  const DistanceSensor * distSensorFront;
} CartAppInitStruct_t;

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t bt_1;
  uint8_t bt_2;
  uint8_t bt_3;
  uint8_t bt_4;
  uint8_t bt_5;
  uint8_t bt_6;
  uint8_t bt_7;
} REMOTE_CTRL_DATA_t;

typedef enum : uint8_t {
  DRV_COAST=0,
  DRV_BREAK,
  DRV_FWD,
  DRV_BWD,
  DRV_LEFT,
  DRV_RIGHT,
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
    REMOTE_CTRL_DATA_t remoteCtrlData;
    void driveMotors(const DRIVE_DIRECTION_t direction);

    void processRemoteControlBuffer(void);
    void driveFWD(const uint8_t speed);
    void driveBWD(const uint8_t speed);
    void driveLFT(const uint8_t speed);
    void driveRGT(const uint8_t speed);
    void driveCoast(void);
    void driveZero(void);

    DRIVE_DIRECTION_t getDriveDirection(const uint8_t x, const uint8_t y);
    bool isXActive(const uint8_t x);
    bool isYActive(const uint8_t y);

};



#endif

