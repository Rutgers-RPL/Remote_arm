#include "BluetoothSerial.h"
#include "Preferences.h"

#define CHANNEL_1 14
#define CHANNEL_2 27
#define CHANNEL_3 26
#define CHANNEL_4 25
#define LED_BUILTIN 2

#define CHANNEL_KEY(ch) ((ch) == CHANNEL_1 ? "ch1" : (ch) == CHANNEL_2 ? "ch2" : (ch) == CHANNEL_3 ? "ch3" : (ch) == CHANNEL_4 ? "ch4" : "")

BluetoothSerial SerialBT;
Preferences prefs;

void set_channel(uint8_t channel, uint8_t state) {
  prefs.putUChar(CHANNEL_KEY(channel), state);
  digitalWrite(channel, state);
}

void init_channels() {
  uint8_t channel_1_state = prefs.getUChar(CHANNEL_KEY(CHANNEL_1), LOW);
  uint8_t channel_2_state = prefs.getUChar(CHANNEL_KEY(CHANNEL_2), LOW);
  uint8_t channel_3_state = prefs.getUChar(CHANNEL_KEY(CHANNEL_3), LOW);
  uint8_t channel_4_state = prefs.getUChar(CHANNEL_KEY(CHANNEL_4), LOW);

  pinMode(CHANNEL_1, OUTPUT);
  pinMode(CHANNEL_2, OUTPUT);
  pinMode(CHANNEL_3, OUTPUT);
  pinMode(CHANNEL_4, OUTPUT);

  digitalWrite(CHANNEL_1, channel_1_state);
  digitalWrite(CHANNEL_2, channel_2_state);
  digitalWrite(CHANNEL_3, channel_3_state);
  digitalWrite(CHANNEL_4, channel_4_state);
}

void setup() {
  prefs.begin("arming-states"); 

  init_channels();
  pinMode(LED_BUILTIN, OUTPUT);

  SerialBT.begin("RRPL Booster");
}

void loop() {
  if (SerialBT.hasClient()) {
    digitalWrite(LED_BUILTIN, HIGH);

    if (SerialBT.available()) {
      String command = SerialBT.readStringUntil('\n');
      command.trim();

      if (command == "ARM_1") {
        set_channel(CHANNEL_1, HIGH);
        SerialBT.println("ARMED_1");
      } else if (command == "ARM_2") {
        set_channel(CHANNEL_2, HIGH);
        SerialBT.println("ARMED_2");
      } else if (command == "ARM_3") {
        set_channel(CHANNEL_3, HIGH);
        SerialBT.println("ARMED_3");
      } else if (command == "ARM_4") {
        set_channel(CHANNEL_4, HIGH);
        SerialBT.println("ARMED_4");
      } else if (command == "DISARM_1") {
        set_channel(CHANNEL_1, LOW);
        SerialBT.println("DISARMED_1");
      } else if (command == "DISARM_2") {
        set_channel(CHANNEL_2, LOW);
        SerialBT.println("DISARMED_2");
      } else if (command == "DISARM_3") {
        set_channel(CHANNEL_3, LOW);
        SerialBT.println("DISARMED_3");
      } else if (command == "DISARM_4") {
        set_channel(CHANNEL_4, LOW);
        SerialBT.println("DISARMED_4");
      } else {
        SerialBT.println("UNKNOWN_COMMAND");
      }
    }
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}