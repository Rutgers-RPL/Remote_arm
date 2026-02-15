#include "BluetoothSerial.h"

#define PIN_ONE 1
#define PIN_TWO 10
#define PIN_THREE 3
#define PIN_FOUR 4

BluetoothSerial SerialBT;

void setup() {
  pinMode(PIN_ONE, OUTPUT);
  pinMode(PIN_TWO, OUTPUT);
  pinMode(PIN_THREE, OUTPUT);
  pinMode(PIN_FOUR, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  SerialBT.begin("RRPL Rocket");
}

void loop() {
  if (SerialBT.hasClient()) {
    digitalWrite(LED_BUILTIN, HIGH);

    if (SerialBT.available()) {
      String command = SerialBT.readStringUntil('\n');
      command.trim();

      if (command == "ARM") {
        arm_rocket();
        SerialBT.println("ROCKET_ARMED");
      } else if (command == "DISAEM") {
        disarm_rocket();
        SerialBT.println("ROCKET_DISARMED");
      } else {
        SerialBT.print("UNKNOWN_COMMAND");
      }
    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void arm_rocket() {
  digitalWrite(PIN_ONE, HIGH);
  digitalWrite(PIN_TWO, HIGH);
  digitalWrite(PIN_THREE, HIGH);
  digitalWrite(PIN_FOUR, HIGH);
}

void disarm_rocket() {
  digitalWrite(PIN_ONE, LOW);
  digitalWrite(PIN_TWO, LOW);
  digitalWrite(PIN_THREE, LOW);
  digitalWrite(PIN_FOUR, LOW);
}

