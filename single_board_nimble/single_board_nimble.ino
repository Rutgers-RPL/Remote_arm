#include <NimBLEDevice.h>
#include <string>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WRITE_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define NOTIFY_CHARACTERISTIC_UUID "498c599b-ad01-4148-8a6a-73c332854747"

#define CHANNEL_1 14
#define CHANNEL_2 25
#define CHANNEL_3 26
#define CHANNEL_4 27

NimBLECharacteristic* write_char;
NimBLECharacteristic* notify_char;

void init_pins() {
  pinMode(CHANNEL_1, OUTPUT);
  pinMode(CHANNEL_2, OUTPUT);
  pinMode(CHANNEL_3, OUTPUT);
  pinMode(CHANNEL_4, OUTPUT);

  digitalWrite(CHANNEL_1, LOW);
  digitalWrite(CHANNEL_2, LOW);
  digitalWrite(CHANNEL_3, LOW);
  digitalWrite(CHANNEL_4, LOW);
}

void handle_pin_toggle(std::string val) {
  if (val.length() < 6) return;

  char pin = val[3];
  std::string action = val.substr(5);
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
    default:
      return;
  }

  std::string msg = "B0_";
  msg += pin;
  msg += "_";
  msg += action;
  msg += "ED";

  notify_char->setValue(msg);
  notify_char->notify();
}

class WriteCharCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo& connInfo) override {
    std::string val = characteristic->getValue();

    Serial.println(val.c_str());

    if (val.length() < 2) return;

    if (val[1] == '0') {
      handle_pin_toggle(val);
    }
  }
};

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* server, NimBLEConnInfo& connInfo) override {
    delay(500);
    notify_char->setValue("DISARMED");
    notify_char->notify();
  }

  void onDisconnect(NimBLEServer* server, NimBLEConnInfo& connInfo, int reason) override {
    NimBLEDevice::startAdvertising();
  }
};

void setup() {
  init_pins();
  Serial.begin(115200);

  NimBLEDevice::init("RRPL Rocket");

  NimBLEServer* server = NimBLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  NimBLEService* service = server->createService(SERVICE_UUID);

  write_char = service->createCharacteristic(
    WRITE_CHARACTERISTIC_UUID,
    NIMBLE_PROPERTY::WRITE
  );

  notify_char = service->createCharacteristic(
    NOTIFY_CHARACTERISTIC_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );

  write_char->setCallbacks(new WriteCharCallbacks());

  service->start();

  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);

  NimBLEDevice::startAdvertising();
}

void loop() {
  // No loop logic needed
}