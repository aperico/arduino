#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10
#define NEUTRAL_FACTOR 50
#define X_MID_POINT 126
#define Y_MID_POINT 121


#define trigPin A1 //roxo
#define echoPin A0 // azul

#define leftSideEN 5 //PWM ENB Yellow
#define leftSideFWD 7 // green // IN 4
#define leftSideBWD 8 // blue // in3

#define rightSideEN 6 //PWM ENA // GREEN
#define rightSideFWD 4 // pink //IN 2
#define rightSideBWD 2 // Gray // IN 1
#define SPEED_FACT (255/100)
#define PROXIMITY_LIMIT 20

typedef enum : uint8_t {
  DRV_COAST,
  DRV_FWD,
  DRV_BWD,
  DRV_BREAK
} DRIVE_DIRECTION_t;

float getDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration;
  float distance;
  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  //Serial.print("Distance: ");
  //Serial.println(distance);
  return distance;
}


void leftWheels(DRIVE_DIRECTION_t cmd, uint8_t speed);



const uint64_t pipe = 0xE8E8F0F0E1LL;

RF24 radio(CE_PIN, CSN_PIN); 

int data[9]; 

void initMotors(){
  
  pinMode(leftSideEN, OUTPUT); // ENA
  pinMode(leftSideFWD, OUTPUT);
  pinMode(leftSideBWD, OUTPUT);

  pinMode(rightSideEN, OUTPUT); 
  pinMode(rightSideFWD, OUTPUT);
  pinMode(rightSideEN, OUTPUT); // ENB
}

void initRadio(){
  radio.begin();
  radio.openReadingPipe(0,pipe);

  //radio.setAutoAck(false); //(true|false) 
  radio.setDataRate(RF24_250KBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS) 
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
  
  delay(500);
  if(radio.isChipConnected()){
    Serial.println("CHIP CONNECTED");
  }else{
    Serial.println("CHIP NOT CONNECTED");
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);  
	pinMode(echoPin, INPUT);  

  
  initRadio();
  initMotors();
}

void processRadio(){
  if ( radio.available() ) //Eğer sinyal algılarsan...
  {
    radio.read( data, sizeof(data) );       
    /*Serial.print(data[0]);
    Serial.print(" ");
    Serial.print(data[1]);
    Serial.print(" ");
    Serial.print(data[2]);
    Serial.print(" ");
    Serial.print(data[3]);
    Serial.print(" ");
    Serial.print(data[4]);
    Serial.print(" ");
    Serial.print(data[5]);
    Serial.print(" ");
    Serial.print(data[6]);
    Serial.print(" ");
    Serial.print(data[7]);
    Serial.print(" ");
    Serial.print(data[8]);*/

    hareket(data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8]);
    //Serial.print("\n");
  }
}

static int x_hiz=0;
static int y_hiz=0;


void rightWheels(DRIVE_DIRECTION_t cmd, uint8_t speed){
  switch(cmd){
    case(DRV_FWD):
      digitalWrite(rightSideFWD, HIGH);
      digitalWrite(rightSideBWD, LOW);  
      analogWrite(rightSideEN, speed);
    break;
    case(DRV_BWD):
      digitalWrite(rightSideFWD, LOW);
      digitalWrite(rightSideBWD, HIGH);  
      analogWrite(rightSideEN, speed);
    break;
    case(DRV_BREAK):
      digitalWrite(rightSideEN, LOW);
      digitalWrite(rightSideFWD, HIGH);
      digitalWrite(rightSideBWD, HIGH);
    break;
    default:
      // coast
      digitalWrite(rightSideEN, LOW);
      digitalWrite(rightSideFWD, LOW);
      digitalWrite(rightSideBWD, LOW);  
      break;
  }  
}

void leftWheels(DRIVE_DIRECTION_t cmd, uint8_t speed){
  switch(cmd){
    case(DRV_FWD):
      digitalWrite(leftSideFWD, HIGH);
      digitalWrite(leftSideBWD, LOW);
      analogWrite(leftSideEN, speed);
    break;
    case(DRV_BWD):
      digitalWrite(leftSideFWD, LOW);
      digitalWrite(leftSideBWD, HIGH);  
      analogWrite(leftSideEN, speed);
    break;
    case(DRV_BREAK):
      digitalWrite(leftSideEN, LOW);
      digitalWrite(leftSideFWD, HIGH);
      digitalWrite(leftSideBWD, HIGH);
    break;
    default:
      // coast
      digitalWrite(leftSideEN, LOW);
      digitalWrite(leftSideFWD, LOW);
      digitalWrite(leftSideBWD, LOW);  
      break;
  }  
}


void driveFWD(uint8_t speed){
  rightWheels(DRV_FWD, speed);
  leftWheels(DRV_FWD, speed);
}
void driveBWD(uint8_t speed){
  rightWheels(DRV_BWD, speed);
  leftWheels(DRV_BWD, speed);
}
void driveLFT(uint8_t speed){
  rightWheels(DRV_BWD, speed);
  leftWheels(DRV_FWD, speed);
}
void driveRGT(uint8_t speed){
  rightWheels(DRV_FWD, speed);
  leftWheels(DRV_BWD, speed);
}
void driveZero(){
  rightWheels(DRV_BREAK, 0);
  leftWheels(DRV_BREAK, 0);
}

void controlSpeed(float distance){
  if(distance < 20){
    if(y_hiz <  (Y_MID_POINT-NEUTRAL_FACTOR)){
      rightWheels(DRV_BWD, 255);
      leftWheels(DRV_BWD, 255);
    }else{
      rightWheels(DRV_BREAK, 0);
      leftWheels(DRV_BREAK, 0);
    }
  }else{
    // speed
    if(y_hiz > (Y_MID_POINT+NEUTRAL_FACTOR)){
      rightWheels(DRV_FWD, 255);
      leftWheels(DRV_FWD, 255);
    }else if(y_hiz <  (Y_MID_POINT-NEUTRAL_FACTOR)){
      rightWheels(DRV_BWD, 255);
      leftWheels(DRV_BWD, 255);
    }else{
      rightWheels(DRV_BREAK, 0);
      leftWheels(DRV_BREAK, 0);
    }
  }
  
}

void processMotors(float distance){
  
  // direction
  if(x_hiz > (X_MID_POINT+NEUTRAL_FACTOR)){
    rightWheels(DRV_FWD, 255);
    leftWheels(DRV_BWD, 255);
  }else if(x_hiz < (X_MID_POINT-NEUTRAL_FACTOR)){
    rightWheels(DRV_BWD, 255);
    leftWheels(DRV_FWD, 255);
  }else{
    //rightWheels(DRV_BREAK, 0);
    //leftWheels(DRV_BREAK, 0);
    controlSpeed(distance);
  }



/*
  */
}

void loop() {
  float dist = getDistance();
  Serial.println(dist);
  processRadio();
  processMotors(dist);
  delay(50);
}

void hareket(int x_axis, int y_axis, int button1, int button2, int button3, int button4, int button5, int button6, int button7) {
  x_hiz= map(x_axis,0,1023,0,255);
  //Serial.print(" x=");
  //Serial.print(x_hiz);
  
  y_hiz= map(y_axis,0,1023,0,255);
  //Serial.print(" y=");
  //Serial.print(y_hiz);
}












/*


// SimpleRx - the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

char text[13] = "AAAAAAAAAAAA\0";

void setup() {
  
  pinMode(CE_PIN, OUTPUT);
  pinMode(CSN_PIN, OUTPUT);
  
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  //radio.setAutoAck(false); //(true|false) 
  radio.setDataRate(RF24_250KBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS) 
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
  Serial.println(text);
  delay(500);
  if(radio.isChipConnected()){
    Serial.println("CHIP CONNECTED");
  }else{
    Serial.println("CHIP NOT CONNECTED");
  }
}


void loop() {
  if(!radio.isChipConnected()){
     Serial.println("CHIP NOT CONNECTED");
  }else{
    if (radio.available()) {
      radio.read(&text, sizeof(text));
      Serial.println(text);
    }else{
      Serial.println("..");
    }
    //radio.flush_rx();
  }
  delay(500);
}


#include "SPI.h" 
#include "RF24.h" 
#include "nRF24L01.h" 
#define CE_PIN 9
#define CSN_PIN 10
// SCK => 13 
// MOSI => 11
// MISO => 12
#define INTERVAL_MS_SIGNAL_LOST 1000 
#define INTERVAL_MS_SIGNAL_RETRY 250 
RF24 radio(CE_PIN, CSN_PIN); 
const byte address[6] = "00001"; 
//NRF24L01 buffer limit is 32 bytes (max struct size) 
struct payload { 
	 byte data1; 
	 char data2; 
}; 
payload payload; 
unsigned long lastSignalMillis = 0; 


#define trigPin 2  
#define echoPin 4
#define leftSideEN 10
#define leftSideFWD 9
#define leftSideBWD 8
#define rightSideEN 5
#define rightSideFWD 7
#define rightSideBWD 6
#define LED 13
#define SPEED_FACT (255/100)
#define PROXIMITY_LIMIT 20

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115200); 
  

  pinMode(trigPin, OUTPUT);  
	pinMode(echoPin, INPUT);  
  
  pinMode(leftSideEN, OUTPUT); // ENA
  pinMode(leftSideFWD, OUTPUT);
  pinMode(leftSideBWD, OUTPUT);

  pinMode(rightSideEN, OUTPUT); 
  pinMode(rightSideFWD, OUTPUT);
  pinMode(rightSideEN, OUTPUT); // ENB
}



typedef enum : uint8_t{
  IRCMD_NONE,
  IRCMD_UP,
  IRCMD_DOWN,
  IRCMD_RIGHT,
  IRCMD_LEFT,
  IRCMD_OK,
  IRCMD_NOT_SUPPORTED
} RemoteCommand_t;


RemoteCommand_t getIRCommand(void){
  
  return IRCMD_NONE;
}


typedef enum : uint8_t {
  DS_IDLE,
  DS_FWD,
  DS_BWD,
  DS_LFT,
  DS_RGT,
  DS_OBSTACLE_DETECTED,
  DS_OBSTRACLE_RETREAT
  
} DRIVE_STATE_t;



uint8_t convertSpeed(uint8_t speed){
  float v = (float)SPEED_FACT * (float)speed;
  Serial.println(v);
  return (uint8_t)v;
}






// the loop function runs over and over again forever
static DRIVE_STATE_t driveState = DS_IDLE;
uint8_t rawSpeed = 155;
uint8_t speed = 255;

void processDriveStateTransition(RemoteCommand_t irCMD, float distSensorValue){
  if(irCMD != IRCMD_NONE){
    if(irCMD == IRCMD_UP){
        driveState = DS_FWD;
      }else if(irCMD == IRCMD_DOWN){
        driveState = DS_BWD;
      }else if(irCMD == IRCMD_LEFT){
        driveState = DS_LFT;
      }else if(irCMD == IRCMD_RIGHT){
        driveState = DS_RGT;
      }else if(irCMD == IRCMD_OK){
        driveState = DS_IDLE;
      }
  }
  if(distSensorValue <= PROXIMITY_LIMIT){
    driveState = DS_OBSTACLE_DETECTED;
  }

}

void processSpeed(float distSensorValue, RemoteCommand_t irCMD){
   
  switch(driveState){
      case(DS_IDLE):
        driveZero();
        processDriveStateTransition(irCMD, distSensorValue);
        break;
      case(DS_FWD):
        driveFWD(speed);
        processDriveStateTransition(irCMD, distSensorValue);
        break;
      case(DS_BWD):
        driveBWD(speed);
        processDriveStateTransition(irCMD, distSensorValue);
        break;
      case(DS_LFT):
        driveLFT(speed);
        processDriveStateTransition(irCMD, distSensorValue);
        break;
      case(DS_RGT):
        driveRGT(speed);
        processDriveStateTransition(irCMD, distSensorValue);
        break;
      case(DS_OBSTRACLE_RETREAT):
        
        if(distSensorValue >= PROXIMITY_LIMIT){
          driveState = DS_IDLE;
        }else{
          driveBWD(150);
        }
        break;
      case(DS_OBSTACLE_DETECTED):
        if(distSensorValue >= PROXIMITY_LIMIT){
          driveState = DS_IDLE;
        }else{
          if(irCMD == IRCMD_DOWN){
            driveState = DS_OBSTRACLE_RETREAT;
          }
          driveZero();
        }
        
        
        break;
      default:
        driveZero();
        processDriveStateTransition(irCMD, distSensorValue);
      break;
  }
}

*/