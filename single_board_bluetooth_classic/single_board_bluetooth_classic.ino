#include "BluetoothSerial.h"

#define CHANNEL_1 14
#define CHANNEL_2 27
#define CHANNEL_3 26
#define CHANNEL_4 25
#define LED_BUILTIN 2

BluetoothSerial SerialBT;

void setup() {
  pinMode(CHANNEL_1, OUTPUT);
  pinMode(CHANNEL_2, OUTPUT);
  pinMode(CHANNEL_3, OUTPUT);
  pinMode(CHANNEL_4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(CHANNEL_1, LOW);
  digitalWrite(CHANNEL_2, LOW);
  digitalWrite(CHANNEL_3, LOW);
  digitalWrite(CHANNEL_4, LOW);

  SerialBT.begin("RRPL Booster");
}

void loop() {
  if (SerialBT.hasClient()) {
    digitalWrite(LED_BUILTIN, HIGH);

    if (SerialBT.available()) {
      String command = SerialBT.readStringUntil('\n');
      command.trim();

      if (command == "ARM_1") {
        digitalWrite(CHANNEL_1, HIGH);
        SerialBT.println("ARMED_1");
      } else if (command == "ARM_2") {
        digitalWrite(CHANNEL_2, HIGH);
        SerialBT.println("ARMED_2");
      } else if (command == "ARM_3") {
        digitalWrite(CHANNEL_3, HIGH);
        SerialBT.println("ARMED_3");
      } else if (command == "ARM_4") {
        digitalWrite(CHANNEL_4, HIGH);
        SerialBT.println("ARMED_4");
      } else if (command == "DISARM_1") {
        digitalWrite(CHANNEL_1, LOW);
        SerialBT.println("DISARMED_1");
      } else if (command == "DISARM_2") {
        digitalWrite(CHANNEL_2, LOW);
        SerialBT.println("DISARMED_2");
      } else if (command == "DISARM_3") {
        digitalWrite(CHANNEL_3, LOW);
        SerialBT.println("DISARMED_3");
      } else if (command == "DISARM_4") {
        digitalWrite(CHANNEL_4, LOW);
        SerialBT.println("DISARMED_4");
      } else {
        SerialBT.println("UNKNOWN_COMMAND");
      }
    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}