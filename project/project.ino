//#include <IRremote.h> // include the library
//#define DECODE_DENON        // Includes Sharp
//#define DECODE_JVC
//#define DECODE_KASEIKYO
//#define DECODE_PANASONIC    // alias for DECODE_KASEIKYO
//#define DECODE_LG
#define DECODE_NEC          // Includes Apple and Onkyo
//#define DECODE_SAMSUNG
//#define DECODE_SONY
//#define DECODE_RC5
//#define DECODE_RC6
//#define DECODE_BOSEWAVE
//#define DECODE_LEGO_PF
//#define DECODE_MAGIQUEST
//#define DECODE_WHYNTER
//#define DECODE_FAST
//#define DECODE_DISTANCE_WIDTH // Universal decoder for pulse distance width protocols
//#define DECODE_HASH         // special decoder for all protocols
//#define DECODE_BEO          // This protocol must always be enabled manually, i.e. it is NOT enabled if no protocol is defined. It prevents decoding of SONY!

#define DEBUG               // Activate this for lots of lovely debug output from the decoders.

//#define RAW_BUFFER_LENGTH  180  // Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.

#include <Arduino.h>
#include <IRremote.hpp> // include the library

#define leftSideEN 10
#define leftSideFWD 9
#define leftSideBWD 8
#define rightSideEN 5
#define rightSideFWD 7
#define rightSideBWD 6
#define IR_RECEIVE_PIN 3
#define LED 13
#define SPEED_FACT (255/100)

typedef enum : uint8_t{
  IRCMD_NONE,
  IRCMD_UP,
  IRCMD_DOWN,
  IRCMD_RIGHT,
  IRCMD_LEFT,
  IRCMD_OK,
  IRCMD_NOT_SUPPORTED
} IRCommand_t;

typedef enum : uint8_t {
  DRV_COAST,
  DRV_FWD,
  DRV_BWD,
  DRV_BREAK
} DRIVE_DIRECTION_t;

void rightWheels(DRIVE_DIRECTION_t cmd, uint8_t speed){
  switch(cmd){
    case(DRV_FWD):
      digitalWrite(rightSideFWD, HIGH);
      analogWrite(rightSideEN, speed);
      digitalWrite(rightSideBWD, LOW);  
    break;
    case(DRV_BWD):
      digitalWrite(rightSideFWD, LOW);
      analogWrite(rightSideEN, speed);
      digitalWrite(rightSideBWD, HIGH);  
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
      digitalWrite(leftSideBWD, LOW);
      digitalWrite(leftSideFWD, HIGH);
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

uint8_t convertSpeed(uint8_t speed){
  float v = (float)SPEED_FACT * (float)speed;
  Serial.println(v);
  return (uint8_t)v;
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);
  delay(3000);

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  printActiveIRProtocols(&Serial);
  
  pinMode(leftSideEN, OUTPUT); // ENA
  pinMode(leftSideFWD, OUTPUT);
  pinMode(leftSideBWD, OUTPUT);

  pinMode(rightSideEN, OUTPUT); 
  pinMode(rightSideFWD, OUTPUT);
  pinMode(rightSideEN, OUTPUT); // ENB
}

IRCommand_t getIRCommand(void){
  if (IrReceiver.decode()) {
    //IrReceiver.printIRResultShort(&Serial);
    //IrReceiver.printIRSendUsage(&Serial);
    IrReceiver.resume(); // Enable receiving of the next value

    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
        return IRCMD_NONE;
        // We have an unknown protocol here, print more info
        //IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    if (IrReceiver.decodedIRData.command == 70) { 
        return IRCMD_UP;
    } else if (IrReceiver.decodedIRData.command == 21) {
        return IRCMD_DOWN;
    } else if (IrReceiver.decodedIRData.command == 67) {
        return IRCMD_RIGHT;
    } else if (IrReceiver.decodedIRData.command == 68) {
        return IRCMD_LEFT;
    } else if (IrReceiver.decodedIRData.command == 64) {
        return IRCMD_OK;
    }else{
      Serial.print("Not supported: ");
      Serial.println(IrReceiver.decodedIRData.command);
        return IRCMD_NOT_SUPPORTED;
    }
  }
  return IRCMD_NONE;
}


typedef enum : uint8_t {
  DS_IDLE,
  DS_FWD,
  DS_BWD
} DRIVE_STATE_t;


// the loop function runs over and over again forever
static DRIVE_STATE_t driveState = DS_IDLE;
static int controlSpeed = 0;
uint8_t rawSpeed = 155;
uint8_t speed = 255;
void loop() {
  IRCommand_t irCMD = getIRCommand();
  
  if(irCMD != IRCMD_NONE){
    Serial.print(irCMD);
    Serial.print(" ");
    //speed = convertSpeed(rawSpeed);
    if(irCMD == IRCMD_UP){
      rightWheels(DRV_FWD, speed);
      leftWheels(DRV_FWD, speed);
    }else if(irCMD == IRCMD_DOWN){
      rightWheels(DRV_BWD, speed);
      leftWheels(DRV_BWD, speed);
    }else if(irCMD == IRCMD_LEFT){
      rightWheels(DRV_BWD, speed);
      leftWheels(DRV_FWD, speed);
    }else if(irCMD == IRCMD_RIGHT){
      rightWheels(DRV_FWD, speed);
      leftWheels(DRV_BWD, speed);
    }else if(irCMD == IRCMD_OK){
      rightWheels(DRV_FWD, 0);
      leftWheels(DRV_BWD, 0);
    }
    Serial.print(speed);
    Serial.println();
  }
 
 
  
  delay(100);

}
