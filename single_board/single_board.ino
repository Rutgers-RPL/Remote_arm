#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <string>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WRITE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define NOTIFY_CHARACTERISTIC_UUID "498c599b-ad01-4148-8a6a-73c332854747"

#define CHANNEL_1 1
#define CHANNEL_2 0
#define CHANNEL_3 10
#define CHANNEL_4 3
#define CHANNEL_5 7
#define CHANNEL_6 5
#define NUM_CHANNELS 6

#define VIN_PIN 4

const uint8_t channel_pins[] = {CHANNEL_1, CHANNEL_2, CHANNEL_3, CHANNEL_4, CHANNEL_5, CHANNEL_6};
uint8_t channel_states[] = {LOW, LOW, LOW, LOW, LOW, LOW};

BLECharacteristic* write_char;
BLECharacteristic* notify_char;

void init_pins() {
  pinMode(CHANNEL_1, OUTPUT);
  pinMode(CHANNEL_2, OUTPUT);
  pinMode(CHANNEL_3, OUTPUT);
  pinMode(CHANNEL_4, OUTPUT);
  pinMode(CHANNEL_5, OUTPUT);
  pinMode(CHANNEL_6, OUTPUT);

  digitalWrite(CHANNEL_1, LOW);
  digitalWrite(CHANNEL_2, LOW);
  digitalWrite(CHANNEL_3, LOW);
  digitalWrite(CHANNEL_4, LOW);
  digitalWrite(CHANNEL_5, LOW);
  digitalWrite(CHANNEL_6, LOW);
}

void send_all_states() {
  for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
    uint8_t channel_state = channel_states[i];
    String state = channel_state == LOW ? "DISARMED" : "ARMED";
    char channel = i + '1';
    String msg = "B0_" + String(channel) + "_" + state;
    notify_char->setValue(msg.c_str());
    delay(10);
    notify_char->notify();
  }
}

void send_voltage() {
  uint16_t value = analogRead(VIN_PIN);
  float actual = 0.0027473f * (float)value + 0.229834f; // https://www.desmos.com/calculator/lmwl7iq6hs
  String msg = "V_" + String(actual);
  notify_char->setValue(msg.c_str());
  delay(10);
  notify_char->notify();
}

void handle_pin_toggle(String val) {
  char channel = val[3];
  uint8_t channel_index = channel - '1';
  String action = val.substring(5);
  uint8_t pin_mode;

  if (action == "ARM") {
    pin_mode = HIGH;
  } else if (action == "DISARM") {
    pin_mode = LOW;
  } else {
    return;
  }

  if (channel > '0' && channel < ((NUM_CHANNELS + 1) + '0')) {
    digitalWrite(channel_pins[channel_index], pin_mode);
    channel_states[channel_index] = pin_mode;
  }

  String msg = "B0_" + String(channel) + "_" + action + "ED";
  notify_char->setValue(msg.c_str());
  delay(10);
  notify_char->notify();
}

class WriteCharCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) {
    String val = characteristic->getValue();

    if (val.length() < 2) return;

    if (val[1] == '0') {
      handle_pin_toggle(val);
    } else if (val == "SYNC") {
      send_all_states();
    } else if (val == "VOLTAGE") {
      send_voltage();
    }
  } 
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* server) {
    // No logic needed
  }

  void onDisconnect(BLEServer* server) {
    BLEDevice::startAdvertising();
  }
};

void setup() {
  init_pins();

  Serial.begin(115200);

  BLEDevice::init("RRPL ___"); // Set the blank to wherever the board is being placed

  BLEServer* server = BLEDevice::createServer();
  BLEService* service = server->createService(SERVICE_UUID);
  server->setCallbacks(new ServerCallbacks());
  
  write_char = service->createCharacteristic(WRITE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  notify_char = service->createCharacteristic(NOTIFY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
 
  write_char->setCallbacks(new WriteCharCallbacks());
  notify_char->addDescriptor(new BLE2902());

  service->start();

  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(0x06);
  advertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();
}

void loop() {
  // No loop logic needed
}