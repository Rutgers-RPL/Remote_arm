#include "BluetoothSerial.h"

#define ARM_COMMAND "ARM"
#define UNARM_COMMAND "UNARM"

#define PIN_ONE 1
#define PIN_TWO 2
#define PIN_THREE 3
#define PIN_FOUR 4

BluetoothSerial SerialBT;

void setup() {
  pinMode(PIN_ONE, OUTPUT);
  pinMode(PIN_TWO, OUTPUT);
  pinMode(PIN_THREE, OUTPUT);
  pinMode(PIN_FOUR, OUTPUT);

  SerialBT.begin("RRPL Rocket");
}

void loop() {
  if (SerialBT.hasClient() && SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim();

    if (command == ARM_COMMAND) {
      arm_rocket();
      SerialBT.println("ROCKET_ARMED")
    } else if (command == UNARM_COMMAND) {
      unarm_rocket();
      SerialBT.println("ROCKET_UNARMED")
    } else {
      SerialBT.print("UNKNOWN_COMMAND");
    }
  }
}

void arm_rocket() {
  digitalWrite(PIN_ONE, HIGH);
  digitalWrite(PIN_TWO, HIGH);
  digitalWrite(PIN_THREE, HIGH);
  digitalWrite(PIN_FOUR, HIGH);
}

void unarm_rocket() {
  digitalWrite(PIN_ONE, LOW);
  digitalWrite(PIN_TWO, LOW);
  digitalWrite(PIN_THREE, LOW);
  digitalWrite(PIN_FOUR, LOW);
}

