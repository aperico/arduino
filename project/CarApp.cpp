#include "CarApp.h"
#include <Arduino.h>

static const float PROXIMITY_LIMIT = 5.0;

static const float DIAGONAL_SCALE = 0.5f;
static const uint8_t MAX_SPEED = 255;
static const uint8_t NEUTRAL_FACTOR = 40;
static const uint8_t X_MID_POINT = 126;
static const uint8_t Y_MID_POINT = 121;
static const uint8_t X_MID_UPPER = (X_MID_POINT + NEUTRAL_FACTOR); 
static const uint8_t X_MID_LOWER = (X_MID_POINT - NEUTRAL_FACTOR); 
static const uint8_t Y_MID_UPPER = (Y_MID_POINT + NEUTRAL_FACTOR); 
static const uint8_t Y_MID_LOWER = (Y_MID_POINT - NEUTRAL_FACTOR); 
static const uint8_t BUTTON_PRESSED = 1;
static const uint8_t BUTTON_RELEASED = 0;

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
  this->motorRight->rotateForward(speed);
  this->motorLeft->rotateForward(speed);
}
void CarApp::driveBWD(const uint8_t speed)
{
  this->motorRight->rotateBackward(speed);
  this->motorLeft->rotateBackward(speed);
}
void CarApp::driveLFT(const uint8_t speed)
{
  this->motorLeft->rotateBackward(speed);
  this->motorRight->rotateForward(speed);
}
void CarApp::driveRGT(const uint8_t speed)
{
  this->motorLeft->rotateForward(speed);
  this->motorRight->rotateBackward(speed);
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

  // STARTING STATE
  this->lights->brakeLightOn();
  this->driveMotorStop();

  uint16_t *buffer = this->radio->getBufferPtr();
  // initialize buffer with zeros
  // joystick values are 0-1023, so 512 is a good starting point
  buffer[0] = 512;
  buffer[1] = 512;
  this->processRemoteControlBuffer(buffer);
}

void CarApp::task()
{

  unsigned long currentTime = millis();
  
  // if connection lost, car cannot be driven
  static unsigned long lastRadioTaskTime = 0;
  if (currentTime - lastRadioTaskTime >= 15) {
    this->radio->task();
    lastRadioTaskTime = currentTime;
  }
  
  if (!this->radio->isConnected())
  {
    //Serial.println("[ERROR] Radio not connected!");
    this->lights->brakeLightOn();
    this->driveMotorStop();
    return;
  }

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
  ;*/

  
  uint16_t *buffer = this->radio->getBufferPtr();
  this->processRemoteControlBuffer(buffer);

  // UP = BT_2
  // RIGHT = BT_3
  // DOWN = BT_4
  // LEFT = BT_5
  
  const float front_sensor_distance = this->distSensorFront->getDistance(); //= 2.0: ~2cm

  static unsigned long lastPrintTime = 0;
  if (currentTime - lastPrintTime >= 500) {
    Serial.print(" X: ");
    Serial.print(this->remoteCtrlData.x);
    Serial.print(" Y: ");
    Serial.print(this->remoteCtrlData.y);
    Serial.print(" Front: ");
    Serial.print(front_sensor_distance);
    Serial.println();
    lastPrintTime = currentTime;
  }
  

  // break if button 1 is pressed
  if (this->remoteCtrlData.bt_4 == BUTTON_PRESSED)
  {
    //Serial.println("Button 4 pressed, stopping motors!");
    this->lights->brakeLightOn();
    this->driveMotorStop();
    return;
  }else{
    this->lights->brakeLightOff();
  }
  
  static unsigned long lastSensorTaskTime = 0;
  if (currentTime - lastSensorTaskTime >= 50) {
    this->distSensorFront->task();
    lastSensorTaskTime = currentTime;
  }
  
  const DRIVE_DIRECTION_t driveDirection = this->getDriveDirection(this->remoteCtrlData.x, this->remoteCtrlData.y);
  if (front_sensor_distance <= PROXIMITY_LIMIT)
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
    this->driveMotors(driveDirection);
  }
}
