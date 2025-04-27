#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10
#define x_axis A0 // x axis
#define y_axis A1 // y axis
#define button1 8 // joystick button
#define button2 2 // A button
#define button3 3 // B button
#define button4 4 // C button
#define button5 5 // D button
#define button6 6 // E button
#define button7 7 // F button

const uint64_t pipe = 0xE8E8F0F0E1LL;
RF24 radio(CE_PIN, CSN_PIN);
uint16_t data[9];

void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.setDataRate(RF24_250KBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
  radio.setPALevel(RF24_PA_MIN);
  radio.setAutoAck(false); //(true|false)
  //radio.setRetries(3, 15); // up to 3 retries, 15*250us delay between

  radio.openWritingPipe(pipe);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  pinMode(button6, INPUT_PULLUP);
  pinMode(button7, INPUT_PULLUP);
  radio.stopListening();
}

unsigned long lastTask = 0;
const unsigned long interval = 10; // 10ms for fast response

void readJoystick()
{
  data[0] = analogRead(x_axis);
  data[1] = analogRead(y_axis);
  data[2] = !digitalRead(button1);
  data[3] = !digitalRead(button2);
  data[4] = !digitalRead(button3);
  data[5] = !digitalRead(button4);
  data[6] = !digitalRead(button5);
  data[7] = !digitalRead(button6);
  data[8] = !digitalRead(button7);
}

void loop()
{

  static unsigned long lastReadTime = 0;
  if (millis() - lastReadTime >= 100) {
    lastReadTime = millis();
    readJoystick();
  }

  // transmitting  
  if (millis() - lastTask >= interval)
  {    
    
    lastTask = millis();
    data[0] = analogRead(x_axis);
    data[1] = analogRead(y_axis);
    data[2] = !digitalRead(button1);
    data[3] = !digitalRead(button2);
    data[4] = !digitalRead(button3);
    data[5] = !digitalRead(button4);
    data[6] = !digitalRead(button5);
    data[7] = !digitalRead(button6);
    data[8] = !digitalRead(button7);
    static uint8_t failCount = 0;
    if (!radio.write(data, sizeof(data))) {
      Serial.println("Transmission failed");
      failCount++;
      if (failCount > 10) {
        radio.begin();
        radio.openWritingPipe(pipe);
        radio.stopListening();
        failCount = 0;
      }
    } else {
      failCount = 0;
    }
  }
}
