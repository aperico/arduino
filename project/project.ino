#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


// UNO/Nano :
// SCK 13
// MISO 12
// MOSI 11
// SS 10

// Radio pins NRF24L01 transceiver --------
const unsigned char NRF24L01_CE_PIN = 9; // white
const unsigned char NRF24L01_CSN_PIN = 10; // brown
// ----------------------------------------

// Ultra-Sonic sensor LM324 - (HC-SR04) ---
const unsigned char LM324_TrigPin = A1;  // white
const unsigned char LM324_EchoPin = A0;  // orange
// ----------------------------------------

// H-Bridge - Motor Controls -----------------------------
const unsigned char L298N_HB_RightSideEN  = 3;  // PWM ENA [GRAY]
const unsigned char L298N_HB_RightSideFWD = 4;  // Green   [IN1]
const unsigned char L298N_HB_RightSideBWD = 2;  // Blue    [IN2]

const unsigned char L298N_HB_LeftSideEN   = 6;  // PWM ENB [orange]
const unsigned char L298N_HB_LeftSideFWD  = 8;  // Grey    [IN4]
const unsigned char L298N_HB_LeftSideBWD  = 7;  // Purple  [IN3]
// --------------------------------------------------------
 

// System global constants -------------
const unsigned char NEUTRAL_FACTOR = 40;
const unsigned char X_MID_POINT = 126;
const unsigned char Y_MID_POINT = 121;
const float SPEED_FACT = (255 / 100);
const unsigned char PROXIMITY_LIMIT = 20;
const uint64_t RADIO_PIPE_ID = 0xE8E8F0F0E1LL;
const unsigned int RADIO_INTERVAL_MS_SIGNAL_LOST = 1000;
const unsigned int RADIO_INTERVAL_MS_SIGNAL_RETRY = 250;
// -------------


typedef enum : uint8_t {
  DRV_COAST,
  DRV_FWD,
  DRV_BWD,
  DRV_BREAK
} DRIVE_DIRECTION_t;

typedef enum : uint8_t {
  TRN_IDLE,
  TRN_LEFT,
  TRN_RIGHT,
} TURN_DIRECTION_t;

// system "private" interfaces -----
static void _initRadio();
static float _LM324_ProcessFrontDistance();
static void _processRadio();
static void _radio_lostConnection();
static void _radio_remote_control_signal(int x_axis, int y_axis, int button1, int button2, int button3, int button4, int button5, int button6, int button7);

static void _initMotors();
static void _motor_ctrl_RightWheels(const DRIVE_DIRECTION_t cmd, const uint8_t speed);
static void _motor_ctrl_LeftWheels(DRIVE_DIRECTION_t cmd, uint8_t speed);
static void driveZero();
static void driveFWD(const uint8_t speed);
static void driveBWD(const uint8_t speed);
static void driveLFT(const uint8_t speed);
static void driveRGT(const uint8_t speed);
// ---------------------------------

// sytem "public" interfaces -------
void setup();
void loop();
// ---------------------------------

// System private variables --------
static RF24 _radio_instance(NRF24L01_CE_PIN, NRF24L01_CSN_PIN);
static int _radio_data[9];
static unsigned long _radio_lastSignalMillis = 0; 
static int _x_hiz = 0;
static int _y_hiz = 0;
static const unsigned char X_MID_UPPER = (X_MID_POINT + NEUTRAL_FACTOR); // 127+50= 78
static const unsigned char X_MID_LOWER = (X_MID_POINT - NEUTRAL_FACTOR); // 127-50= 77
static const unsigned char Y_MID_UPPER = (Y_MID_POINT + NEUTRAL_FACTOR); // 127+50= 78
static const unsigned char Y_MID_LOWER = (Y_MID_POINT - NEUTRAL_FACTOR); // 127-50= 77
// -----------------------------------

static float _LM324_ProcessFrontDistance() {
  digitalWrite(LM324_TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(LM324_TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(LM324_TrigPin, LOW);
  const float duration = pulseIn(LM324_EchoPin, HIGH);
  const float distance = (duration * .0343) / 2;
  return distance;
}

static void _initMotors() {
  pinMode(L298N_HB_LeftSideEN, OUTPUT); // B
  pinMode(L298N_HB_LeftSideFWD, OUTPUT);
  pinMode(L298N_HB_LeftSideBWD, OUTPUT);

  pinMode(L298N_HB_RightSideEN, OUTPUT); // A
  pinMode(L298N_HB_RightSideFWD, OUTPUT);
  pinMode(L298N_HB_RightSideBWD, OUTPUT);  

}

static void _initUltraSonic(){
  pinMode(LM324_TrigPin, OUTPUT);
  pinMode(LM324_EchoPin, INPUT);
}

static void _initRadio() {
  _radio_instance.begin();
  _radio_instance.openReadingPipe(0, RADIO_PIPE_ID);

  //radio.setAutoAck(false); //(true|false)
  _radio_instance.setDataRate(RF24_250KBPS);  //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
  _radio_instance.setPALevel(RF24_PA_HIGH);
  _radio_instance.setAutoAck(false); //(true|false) 
  _radio_instance.startListening();

  delay(500);
  if (_radio_instance.isChipConnected()) {
    Serial.println("CHIP CONNECTED");
  } else {
    Serial.println("CHIP NOT CONNECTED");
  }
}

static void _radio_lostConnection() 
{ 
	 Serial.println("We have lost connection, preventing unwanted behavior"); 
	 delay(RADIO_INTERVAL_MS_SIGNAL_RETRY); 
} 

static void _radio_remote_control_signal(const int x_axis, const int y_axis, const int button1, const int button2, const int button3, const int button4, const int button5, const int button6, const int button7) {
  _x_hiz = map(x_axis, 0, 1023, 0, 255);
  _y_hiz = map(y_axis, 0, 1023, 0, 255);
}

static void _processRadio() {

  const unsigned long currentMillis = millis(); 

  if (_radio_instance.available()) {

    _radio_instance.read(_radio_data, sizeof(_radio_data));
    Serial.println(_radio_data[0]);
    _radio_remote_control_signal(_radio_data[0], _radio_data[1], _radio_data[2], _radio_data[3], _radio_data[4], _radio_data[5], _radio_data[6], _radio_data[7], _radio_data[8]);

    Serial.print("X:");
    Serial.println(_x_hiz);
    Serial.print("Y:");
    Serial.println(_y_hiz);


    _radio_lastSignalMillis = currentMillis; 
     if (currentMillis - _radio_lastSignalMillis > RADIO_INTERVAL_MS_SIGNAL_LOST) { 
        _radio_lostConnection(); 
      } 
    
  }
}


static void _motor_ctrl_RightWheels(const DRIVE_DIRECTION_t cmd, const uint8_t speed) {
  switch (cmd) {
    case (DRV_FWD):
      digitalWrite(L298N_HB_RightSideFWD, HIGH);
      digitalWrite(L298N_HB_RightSideBWD, LOW);
      analogWrite(L298N_HB_RightSideEN, speed);
      break;
    case (DRV_BWD):
      digitalWrite(L298N_HB_RightSideFWD, LOW);
      digitalWrite(L298N_HB_RightSideBWD, HIGH);
      analogWrite(L298N_HB_RightSideEN, speed);
      break;
    case (DRV_BREAK):
      digitalWrite(L298N_HB_RightSideEN, LOW);
      digitalWrite(L298N_HB_RightSideFWD, HIGH);
      digitalWrite(L298N_HB_RightSideBWD, HIGH);
      break;
    default:
      // coast
      analogWrite(L298N_HB_RightSideEN, 0);
      analogWrite(L298N_HB_RightSideEN, 0);
      digitalWrite(L298N_HB_RightSideEN, LOW);
      digitalWrite(L298N_HB_RightSideFWD, LOW);
      digitalWrite(L298N_HB_RightSideBWD, LOW);
      break;
  }
}

static void _motor_ctrl_LeftWheels(const DRIVE_DIRECTION_t cmd, const uint8_t speed) {
  switch (cmd) {
    case (DRV_FWD):
      digitalWrite(L298N_HB_LeftSideFWD, HIGH);
      digitalWrite(L298N_HB_LeftSideBWD, LOW);
      analogWrite(L298N_HB_LeftSideEN, speed);
      break;
    case (DRV_BWD):
      digitalWrite(L298N_HB_LeftSideFWD, LOW);
      digitalWrite(L298N_HB_LeftSideBWD, HIGH);
      analogWrite(L298N_HB_LeftSideEN, speed);
      break;
    case (DRV_BREAK):
      digitalWrite(L298N_HB_LeftSideEN, LOW);
      digitalWrite(L298N_HB_LeftSideFWD, HIGH);
      digitalWrite(L298N_HB_LeftSideBWD, HIGH);
      break;
    default:
      // coast
      digitalWrite(L298N_HB_LeftSideEN, LOW);
      digitalWrite(L298N_HB_LeftSideFWD, LOW);
      digitalWrite(L298N_HB_LeftSideBWD, LOW);
      break;
  }
}

static void driveFWD(const uint8_t speed) {
  _motor_ctrl_RightWheels(DRV_FWD, speed);
  _motor_ctrl_LeftWheels(DRV_FWD, speed);
}
static void driveBWD(const uint8_t speed) {
  _motor_ctrl_RightWheels(DRV_BWD, speed);
  _motor_ctrl_LeftWheels(DRV_BWD, speed);
}
static void driveLFT(const uint8_t speed) {
  _motor_ctrl_RightWheels(DRV_BWD, speed);
  _motor_ctrl_LeftWheels(DRV_FWD, speed);
}
static void driveRGT(const uint8_t speed) {
  _motor_ctrl_RightWheels(DRV_FWD, speed);
  _motor_ctrl_LeftWheels(DRV_BWD, speed);
}
static void driveZero() {
  _motor_ctrl_RightWheels(DRV_BREAK, 0);
  _motor_ctrl_LeftWheels(DRV_BREAK, 0);
}
static void driveCoast(){
  _motor_ctrl_RightWheels(DRV_COAST, 0);
  _motor_ctrl_LeftWheels(DRV_COAST, 0);
}

static TURN_DIRECTION_t get_turning_direction(){
  TURN_DIRECTION_t tdir = TRN_IDLE;
  if (_x_hiz > (X_MID_POINT + NEUTRAL_FACTOR)) {
    Serial.println("Turning right");
    tdir = TRN_RIGHT;
  } else if (_x_hiz < (X_MID_POINT - NEUTRAL_FACTOR)) {
    Serial.println("Turning left");
    tdir = TRN_LEFT;
  } else {
    tdir = TRN_IDLE;
  }
}


static unsigned char get_speed(){
  // TODO: CALC SPEED proportionally
  unsigned char speed = 0;
  if (_y_hiz > Y_MID_UPPER) {
    speed = 255;
  } else if (_y_hiz < Y_MID_LOWER) {
    speed = 255;
  } else {
    speed = 0;
  }
  return speed;
}

static DRIVE_DIRECTION_t get_direction(){
  DRIVE_DIRECTION_t dir = DRV_COAST;
  if (_y_hiz > Y_MID_UPPER) {
    dir = DRV_FWD;
  } else if (_y_hiz < Y_MID_LOWER) {
    dir = DRV_BWD;
  } else {
    dir = DRV_COAST;
  }
  return dir;
}


static void processMotors(const float front_sensor_distance, const DRIVE_DIRECTION_t drive_direction, const TURN_DIRECTION_t turn_direction, const unsigned char speed) {


  if(front_sensor_distance <= 2.0){
  
  }

  if(drive_direction == DRV_FWD){
    driveFWD(speed);
  }else if(drive_direction == DRV_BWD){
    driveBWD(speed);
  }else{
    driveCoast();
  }

  
  
}

// PUBLIC ================================================

void setup() {
  Serial.begin(9600);
  _initUltraSonic();
  _initRadio();
  _initMotors();
}


void loop() {
  //Serial.println(dist); // dist <= 2.0 (stop)
  _processRadio();
  
  const float front_sensor_distance = _LM324_ProcessFrontDistance(); //= 2.0: ~2cm
  const DRIVE_DIRECTION_t drive_direction = get_direction();
  const TURN_DIRECTION_t turn_direction = get_turning_direction();
  const unsigned char speed = get_speed();
  processMotors(front_sensor_distance, drive_direction, turn_direction, speed);

  delay(100);
   
}


