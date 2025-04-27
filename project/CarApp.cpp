#include "CarApp.h"
#include <Arduino.h>

static const uint8_t PROXIMITY_LIMIT = 20;

static const float DIAGONAL_SCALE = 0.5f;
static const uint8_t MAX_SPEED = 255;
static const uint8_t NEUTRAL_FACTOR = 40;
static const uint8_t X_MID_POINT = 126;
static const uint8_t Y_MID_POINT = 121;
static const uint8_t X_MID_UPPER = (X_MID_POINT + NEUTRAL_FACTOR); 
static const uint8_t X_MID_LOWER = (X_MID_POINT - NEUTRAL_FACTOR); 
static const uint8_t Y_MID_UPPER = (Y_MID_POINT + NEUTRAL_FACTOR); 
static const uint8_t Y_MID_LOWER = (Y_MID_POINT - NEUTRAL_FACTOR); 
static const uint8_t BUTTON_PRESSED = 0;
static const uint8_t BUTTON_RELEASED = 1;

CarApp::CarApp(const CartAppInitStruct_t *iff) : 
  motorRight(iff->motorRight), 
  motorLeft(iff->motorLeft),
  radio(iff->radio),
  distSensorFront(iff->distSensorFront),
  lights(iff->lights)
{
}

void CarApp::processRemoteControlBuffer(uint16_t buffer[RADIO_BUFFER_SIZE])
{
  this->remoteCtrlData.x = map(buffer[0], 0, 1023, 0, MAX_SPEED);
  this->remoteCtrlData.y = map(buffer[1], 0, 1023, 0, MAX_SPEED);

  this->remoteCtrlData.bt_1 = buffer[2];
  this->remoteCtrlData.bt_2 = buffer[3];
  this->remoteCtrlData.bt_3 = buffer[4];
  this->remoteCtrlData.bt_4 = buffer[5];
  this->remoteCtrlData.bt_5 = buffer[6];
  this->remoteCtrlData.bt_6 = buffer[7];
  this->remoteCtrlData.bt_7 = buffer[8];
}

bool CarApp::isXActive(const uint8_t x)
{
  return (x >= X_MID_UPPER) || (x <= X_MID_LOWER);
}
bool CarApp::isYActive(const uint8_t y)
{
  return (y >= Y_MID_UPPER) || (y <= Y_MID_LOWER);
}

/*DRIVE_DIRECTION_t CarApp::getDriveDirection(const uint8_t x, const uint8_t y)
{
  if (isYActive(y) && isXActive(x))
  {
    if (y >= Y_MID_UPPER && x <= X_MID_LOWER)
      return DRV_FWD_LEFT;
    if (y >= Y_MID_UPPER && x >= X_MID_UPPER)
      return DRV_FWD_RIGHT;
    if (y <= Y_MID_LOWER && x <= X_MID_LOWER)
      return DRV_BWD_LEFT;
    if (y <= Y_MID_LOWER && x >= X_MID_UPPER)
      return DRV_BWD_RIGHT;
  }
  if (isYActive(y))
  {
    if (y >= Y_MID_UPPER)
      return DRV_FWD;
    if (y <= Y_MID_LOWER)
      return DRV_BWD;
  }
  if (isXActive(x))
  {
    if (x >= X_MID_UPPER)
      return DRV_RIGHT;
    if (x <= X_MID_LOWER)
      return DRV_LEFT;
  }
  return DRV_COAST;
}*/


DRIVE_DIRECTION_t CarApp::getDriveDirection(const uint8_t x, const uint8_t y)
{
  if (isYActive(y))
  {
    if (y >= Y_MID_UPPER)
      return DRV_FWD;
    if (y <= Y_MID_LOWER)
      return DRV_BWD;
  }
  
  if (isXActive(x))
  {
    if (x >= X_MID_UPPER)
      return DRV_RIGHT;
    if (x <= X_MID_LOWER)
      return DRV_LEFT;
  }
  return DRV_COAST;
}

void CarApp::driveMotors(const DRIVE_DIRECTION_t direction)
{
  Serial.println(direction);
  switch (direction)
  {
  case DRV_COAST:
    this->driveCoast();
    break;
  case DRV_BRAKE:
    this->driveMotorStop();
    break;
  case DRV_FWD:
    this->driveFWD(MAX_SPEED);
    break;
  case DRV_BWD:
    this->driveBWD(MAX_SPEED);
    break;
  case DRV_LEFT:
    this->driveLFT(MAX_SPEED);
    break;
  case DRV_RIGHT:
    this->driveRGT(MAX_SPEED);
    break;
  case DRV_FWD_LEFT:
    this->driveFWD_LEFT(MAX_SPEED);
    break;
  case DRV_FWD_RIGHT:
    this->driveFWD_RIGHT(MAX_SPEED);
    break;
  case DRV_BWD_LEFT:
    this->driveBWD_LEFT(MAX_SPEED);
    break;
  case DRV_BWD_RIGHT:
    this->driveBWD_RIGHT(MAX_SPEED);
    break;
  default:
    this->driveCoast();
    break;
  }
}

void CarApp::driveFWD(const uint8_t speed)
{
  Serial.println("driveFWD");
  this->motorRight->rotateForward(speed);
  this->motorLeft->rotateForward(speed);
}
void CarApp::driveBWD(const uint8_t speed)
{
  Serial.println("driveBWD");
  this->motorRight->rotateBackward(speed);
  this->motorLeft->rotateBackward(speed);
}
void CarApp::driveLFT(const uint8_t speed)
{
  Serial.println("driveLFT");
  this->motorLeft->rotateForward(speed);
  this->motorRight->rotateBackward(speed);
}
void CarApp::driveRGT(const uint8_t speed)
{
  Serial.println("driveRGT");
  this->motorLeft->rotateBackward(speed);
  this->motorRight->rotateForward(speed);
}
void CarApp::driveFWD_LEFT(const uint8_t speed) {
  this->motorLeft->rotateForward(speed * DIAGONAL_SCALE);
  this->motorRight->rotateForward(speed);
}
void CarApp::driveFWD_RIGHT(const uint8_t speed) {
  this->motorLeft->rotateForward(speed);
  this->motorRight->rotateForward(speed * DIAGONAL_SCALE);
}
void CarApp::driveBWD_LEFT(const uint8_t speed) {
  this->motorLeft->rotateBackward(speed * DIAGONAL_SCALE);
  this->motorRight->rotateBackward(speed);
}
void CarApp::driveBWD_RIGHT(const uint8_t speed) {
  this->motorLeft->rotateBackward(speed);
  this->motorRight->rotateBackward(speed * DIAGONAL_SCALE);
}
void CarApp::driveMotorStop()
{
  // TODO: "break" not implemented in motor app yet
  this->motorRight->stop();
  this->motorLeft->stop();
}
void CarApp::driveCoast()
{

  this->motorRight->coast();
  this->motorLeft->coast();
}

void CarApp::init()
{
  this->radio->init();
  this->motorRight->init();
  this->motorLeft->init();
  this->lights->init();
  this->distSensorFront->init();
}
void CarApp::task()
{

  // if connection lost, car cannot be driven
  this->radio->task();
  if (!this->radio->isConnected())
  {
    Serial.println("Radio not connected!");
    this->driveMotorStop();
    return;
  }

  
  uint16_t *buffer = this->radio->getBufferPtr();
  this->processRemoteControlBuffer(buffer);

  // print all button states
  /*Serial.print("Button 1: ");
  Serial.print(this->remoteCtrlData.bt_1);
  Serial.print(" Button 2: ");
  Serial.print(this->remoteCtrlData.bt_2);
  Serial.print(" Button 3: ");
  Serial.print(this->remoteCtrlData.bt_3);
  Serial.print(" Button 4: ");
  Serial.print(this->remoteCtrlData.bt_4);
  Serial.print(" Button 5: ");
  Serial.print(this->remoteCtrlData.bt_5);
  Serial.print(" Button 6: ");
  Serial.print(this->remoteCtrlData.bt_6);
  Serial.print(" Button 7: ");
  Serial.print(this->remoteCtrlData.bt_7);
  Serial.print(" X: ");
  Serial.print(this->remoteCtrlData.x);
  Serial.print(" Y: ");
  Serial.print(this->remoteCtrlData.y);*/
  

    // UP = BT_2
    // RIGHT = BT_3
    // DOWN = BT_4
    // LEFT = BT_5

  // break if button 1 is pressed
  if (this->remoteCtrlData.bt_4 == BUTTON_PRESSED)
  {
    Serial.println("Button 4 pressed, stopping motors!");
    this->lights->brakeLightOn();
    this->driveMotorStop();
    return;
  }else{
    this->lights->brakeLightOff();
  }
  

  this->distSensorFront->task();
  const float front_sensor_distance = this->distSensorFront->getDistance(); //= 2.0: ~2cm
  
  const DRIVE_DIRECTION_t driveDirection = this->getDriveDirection(this->remoteCtrlData.x, this->remoteCtrlData.y);
  if (front_sensor_distance < PROXIMITY_LIMIT)
  {
    if (driveDirection == DRV_BWD || driveDirection == DRV_LEFT || driveDirection == DRV_RIGHT)
    {
      this->driveMotors(driveDirection);
    }
    else
    {
      this->lights->brakeLightOn();
      this->driveMotorStop();
    }
    return;
  }
  else
  {
    // if distance is ok, drive motors
    Serial.println("Distance OK!");
    this->driveMotors(driveDirection);
  }
}
