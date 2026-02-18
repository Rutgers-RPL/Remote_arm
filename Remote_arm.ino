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

      switch (command) {
        case "ARM_1":
          digitalWrite(PIN_ONE, HIGH);
          SerialBT.print("ARMED_1");
          break;
        case "ARM_2":
          digitalWrite(PIN_TWO, HIGH);
          SerialBT.print("ARMED_2");
          break;
        case "ARM_3":
          digitalWrite(PIN_THREE, HIGH);
          SerialBT.print("ARMED_3");
          break;
        case "ARM_4":
          digitalWrite(PIN_FOUR, HIGH);
          SerialBT.print("ARMED_4");
          break;
        case "DISARM_1":
          digitalWrite(PIN_ONE, LOW);
          SerialBT.print("DISARMED_1");
          break;
        case "DISARM_2":
          digitalWrite(PIN_TWO, LOW);
            SerialBT.print("DISARMED_2");
          break;
        case "DISARM_3":
          digitalWrite(PIN_THREE, LOW);
          SerialBT.print("DISARMED_3");
          break;
        case "DISARM_4":
          digitalWrite(PIN_FOUR, LOW);
          SerialBT.print("DISARMED_4");
          break;
        default:
          SerialBT.print("UNKNOWN_COMMAND");
      }
    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
