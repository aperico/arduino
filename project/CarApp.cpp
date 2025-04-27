#include "CarApp.h"
#include <Arduino.h>

static const uint8_t PROXIMITY_LIMIT = 20;

static const uint8_t NEUTRAL_FACTOR = 40;
static const uint8_t X_MID_POINT = 126;
static const uint8_t Y_MID_POINT = 121;
static const uint8_t X_MID_UPPER = (X_MID_POINT + NEUTRAL_FACTOR); // 127+50= 78
static const uint8_t X_MID_LOWER = (X_MID_POINT - NEUTRAL_FACTOR); // 127-50= 77
static const uint8_t Y_MID_UPPER = (Y_MID_POINT + NEUTRAL_FACTOR); // 127+50= 78
static const uint8_t Y_MID_LOWER = (Y_MID_POINT - NEUTRAL_FACTOR); // 127-50= 77

CarApp::CarApp(const CartAppInitStruct_t * iff): motorRight(iff->motorRight), motorLeft(iff->motorLeft), radio(iff->radio), distSensorFront(iff->distSensorFront){

}

void CarApp::processRemoteControlBuffer(void) {
  //const int x_axis, const int y_axis, const int button1, const int button2, const int button3, const int button4, const int button5, const int button6, const int button7
  const uint16_t * buffer = this->radio->getBufferPtr();

  this->remoteCtrlData.x = map(buffer[0], 0, 1023, 0, 255);
  this->remoteCtrlData.y = map(buffer[1], 0, 1023, 0, 255);
  
}

bool CarApp::isXActive(const uint8_t x){
  return (x >= X_MID_UPPER) || (x <= X_MID_LOWER);
}
bool CarApp::isYActive(const uint8_t y){
  return (y >= Y_MID_UPPER) || (y <= Y_MID_LOWER);
}

DRIVE_DIRECTION_t CarApp::getDriveDirection(const uint8_t x, const uint8_t y){
    DRIVE_DIRECTION_t dir = DRV_COAST;
    if(this->isXActive(x)){
      // process turns
      if (x >= X_MID_UPPER) {
        dir = DRV_RIGHT;
      } else if (x <= X_MID_LOWER) {
        dir = DRV_LEFT;
      } else {
        dir = DRV_COAST;
      }
    }else if(this->isYActive(y)){
      // process direction
      if (y >= Y_MID_UPPER) {
        dir = DRV_FWD;
      } else if (y <= Y_MID_LOWER) {
        dir = DRV_BWD;
      } else {
        dir = DRV_COAST;
      }
    }else{
      // coast
       dir = DRV_COAST;
    }

    return dir;

}


void CarApp::driveMotors(const DRIVE_DIRECTION_t direction){
  Serial.println(direction);
  switch(direction){
    case DRV_COAST:
      this->driveCoast();
      break;
    case DRV_BREAK:
      this->driveZero();
      break;
    case DRV_FWD:
      this->driveFWD(255);
      break;
    case DRV_BWD:
      this->driveBWD(255);
      break;
    case DRV_LEFT:
      this->driveLFT(255);
      break;
    case DRV_RIGHT:
      this->driveRGT(255);
      break;
    default:
      this->driveCoast();
      break;
  }
}

void CarApp::driveFWD(const uint8_t speed) {
  Serial.println("driveFWD");
  this->motorRight->rotateForward(speed);
  this->motorLeft->rotateForward(speed);
}
void CarApp::driveBWD(const uint8_t speed) {
  Serial.println("driveBWD");
  this->motorRight->rotateBackward(speed);
  this->motorLeft->rotateBackward(speed);
}
void CarApp::driveLFT(const uint8_t speed) {
  Serial.println("driveLFT");
  this->motorLeft->rotateForward(speed);
  this->motorRight->rotateBackward(speed);
}
void CarApp::driveRGT(const uint8_t speed) {
  Serial.println("driveRGT");
  this->motorLeft->rotateBackward(speed);
  this->motorRight->rotateForward(speed);
}
void CarApp::driveZero() {
  // TODO: "break" not implemented in motor app yet
  Serial.println("driveZero");
  this->motorRight->coast();
  this->motorLeft->coast();
}
void CarApp::driveCoast(){
  Serial.println("driveCoast");
  this->motorRight->coast();
  this->motorLeft->coast();
}

void CarApp::init(){
  this->radio->init();
  this->motorRight->init();
  this->motorLeft->init();
}

void CarApp::task(){
  this->radio->task();
  this->processRemoteControlBuffer();
  Serial.print("x:");
  Serial.println(this->remoteCtrlData.x);
  Serial.print("y:");
  Serial.println(this->remoteCtrlData.y);

    
  this->distSensorFront->task();
  const float front_sensor_distance = this->distSensorFront->getDistance(); //= 2.0: ~2cm
  Serial.print("Dist: ");  
  Serial.println(front_sensor_distance);  
 
  const DRIVE_DIRECTION_t driveDirection = this->getDriveDirection(this->remoteCtrlData.x, this->remoteCtrlData.y);
  if(this->radio->isConnected()){
    this->driveMotors(driveDirection);
  }else{
    Serial.print("isConnected: ");  
    Serial.println(this->radio->isConnected());  
    this->driveZero();
  }
  /*
  delay(2000);
  this->driveFWD(255);
  delay(1000);
  this->driveCoast();
  delay(2000);

  this->driveBWD(255);
  delay(1000);
  this->driveCoast();
  delay(2000);

  this->driveRGT(255);
  delay(1000);
  this->driveCoast();
  delay(2000);

  this->driveLFT(255);
  delay(1000);
  this->driveCoast();
  delay(2000);
  */

}





