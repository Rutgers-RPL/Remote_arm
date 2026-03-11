#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <string>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WRITE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define NOTIFY_CHARACTERISTIC_UUID "498c599b-ad01-4148-8a6a-73c332854747"

#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3
#define CHANNEL_4 4
#define CHANNEL_5 5 
#define CHANNEL_6 6

BLECharacteristic* write_char;
BLECharacteristic* notify_char;

// void init_pins() {
//   pinMode(CHANNEL_1, OUTPUT);
//   pinMode(CHANNEL_2, OUTPUT);
//   pinMode(CHANNEL_3, OUTPUT);
//   pinMode(CHANNEL_4, OUTPUT);
//   pinMode(CHANNEL_5, OUTPUT);
//   pinMode(CHANNEL_6, OUTPUT);

//   digitalWrite(CHANNEL_1, LOW);
//   digitalWrite(CHANNEL_2, LOW);
//   digitalWrite(CHANNEL_3, LOW);
//   digitalWrite(CHANNEL_4, LOW);
//   digitalWrite(CHANNEL_5, LOW);
//   digitalWrite(CHANNEL_6, LOW);
// }

void handle_pin_toggle(String val) {
  String action = val.substring(7);
  char pin = val[5];
  uint8_t pin_mode;

  if (action == "ARM") {
    pin_mode = HIGH;
  } else if (action == "DISARM") {
    pin_mode = LOW;
  } else {
    return;
  }

  switch (pin) {
    case '1':
      digitalWrite(CHANNEL_1, pin_mode);
      break;
    case '2':
      digitalWrite(CHANNEL_2, pin_mode);
      break;
    case '3':
      digitalWrite(CHANNEL_3, pin_mode);
      break;
    case '4':
      digitalWrite(CHANNEL_4, pin_mode);
      break;
    case '5':
      digitalWrite(CHANNEL_5, pin_mode);
      break;
    case '6':
      digitalWrite(CHANNEL_6, pin_mode);
      break;
    default:
      break;
  }

  notify_char->setValue("B0_" + String(pin) + "_" + action + "ED");
  notify_char->notify();
}

class WriteCharCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) {
    String val = characteristic->getValue();

    if (val.length() < 2) return;

    if (val[1] == '0') {
      handle_pin_toggle(val);
    }
  }
};

void setup() {
  // init_pins();

  BLEDevice::init("RRPL Rocket");

  BLEServer* server = BLEDevice::createServer();
  BLEService* service = server->createService(SERVICE_UUID);
  
  write_char = service->createCharacteristic(WRITE_CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_WRITE);
  notify_char = service->createCharacteristic(NOTIFY_CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
 
  write_char->setCallbacks(new WriteCharCallbacks());
  notify_char->addDescriptor(new BLE2902());

  notify_char->setValue("B0_CONNECTED");

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