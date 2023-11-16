#define leftSideEN 10
#define leftSideFWD 9
#define leftSideBWD 8

#define rightSideEN 5
#define rightSideFWD 7
#define rightSideBWD 6

#define SPEED_FACT (255/100)

typedef enum : uint8_t {
  DRV_COAST,
  DRV_FWD,
  DRV_BWD,
  DRV_BREAK
} DRIVE_DIRECTION_t;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(leftSideEN, OUTPUT); // ENA
  pinMode(leftSideFWD, OUTPUT);
  pinMode(leftSideBWD, OUTPUT);

  pinMode(rightSideEN, OUTPUT); 
  pinMode(rightSideFWD, OUTPUT);
  pinMode(rightSideEN, OUTPUT); // ENB
}

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
  return SPEED_FACT * speed;
}

// the loop function runs over and over again forever
void loop() {
  //
  
  uint8_t speed = convertSpeed(100);
  
  //leftWheels(DRV_COAST, 0);
  //rightWheels(DRV_COAST, 0);

  rightWheels(DRV_BWD, speed);
  leftWheels(DRV_FWD, speed);
  delay(1000);                      

  rightWheels(DRV_BREAK, 0);
  leftWheels(DRV_BREAK, 0);

  delay(1000);                      

  rightWheels(DRV_FWD, speed);
  leftWheels(DRV_BWD, speed);

  delay(1000);  

  rightWheels(DRV_BREAK, 0);
  leftWheels(DRV_BREAK, 0);

  delay(1000);                      

  /*
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)

  leftWheels(DRV_COAST, 0);
  rightWheels(DRV_COAST, 0);

  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1500);         
  
  leftWheels(DRV_BWD, speed);
  rightWheels(DRV_BWD, speed);

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED off by making the voltage LOW
  delay(1000);  

  leftWheels(DRV_COAST, 0);
  rightWheels(DRV_COAST, 0);

  delay(1500);  
  */


}
