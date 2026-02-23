#include <WiFi.h>
#include <esp_now.h>
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

#define ATTEMPT_MAX_ATTEMPTS 5
#define ATTEMPT_INTERVAL 500

const uint8_t ignition_bay_mac_addr[6] = {}; // TODO: Get mac addr
const uint8_t nose_cone_mac_addr[6] = {}; // TODO: Get mac addr

esp_now_peer_info_t ignition_bay_peer_info = {
  .channel = 0,
  .encrypt = false
};

esp_now_peer_info_t nose_cone_peer_info = {
  .channel = 0,
  .encrypt = false
};

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

    switch (val[1]) {
      case '0': 
        handle_pin_toggle(val);
        break;
      case '1':
        esp_now_send(ignition_bay_mac_addr, (uint8_t*)val.c_str(), val.length());
        break;
      case '2':
        esp_now_send(nose_cone_mac_addr, (uint8_t*)val.c_str(), val.length());
        break;
      default:
        break;
    }
  } 
};

esp_err_t attempt_esp_now_add_peer(esp_now_peer_info_t* peer_info, int max_attempts, int interval) {
  esp_err_t ret; 
  int attempts = 1;

  while (attempts <= max_attempts) {
    ret = esp_now_add_peer(peer_info);
    if (ret) {
      attempts++;
      delay(interval);
    } else {
      break;
    }
  }

  return ret;
}

void on_esp_now_data_sent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_FAIL) {
    if (memcmp(tx_info->des_addr, ignition_bay_mac_addr, 6) == 0) {
      ESP_LOGE("ESP NOW Send", "Failed to send data to ignition bay");
    } else if (memcmp(tx_info->des_addr, nose_cone_mac_addr, 6) == 0) {
      ESP_LOGE("ESP NOW Send", "Failed to send data to nose cone");
    }
  }
}

void on_esp_now_data_received(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
  // TODO
}

void setup() {
  init_pins();

  WiFi.mode(WIFI_STA);
  esp_now_init();
  BLEDevice::init("RRPL Rocket");

  BLEServer* server = BLEDevice::createServer();
  BLEService* service = server->createService(SERVICE_UUID);
  BLEAdvertising* advertising = server->getAdvertising();

  if (!server->isStarted()) {
    ESP_LOGE("Setup", "Failed to start BLE server");
  }

  write_char = service->createCharacteristic(WRITE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  notify_char = service->createCharacteristic(NOTIFY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  
  write_char->setCallbacks(new WriteCharCallbacks());
  notify_char->addDescriptor(new BLE2902());

  bool service_started = service->start();

  if (!service_started) {
    ESP_LOGE("Setup", "Failed to start service");
  }

  bool advertising_started = advertising->start();

  if (!advertising_started) {
    ESP_LOGE("Setup", "Failed to start advertising");
  }

  memcpy(ignition_bay_peer_info.peer_addr, ignition_bay_mac_addr, 6);
  memcpy(nose_cone_peer_info.peer_addr, nose_cone_mac_addr, 6);

  notify_char->setValue("B0_CONNECTED");
  notify_char->notify();

  esp_err_t ignition_bay_ret = attempt_esp_now_add_peer(&ignition_bay_peer_info, ATTEMPT_MAX_ATTEMPTS, ATTEMPT_INTERVAL);
  esp_err_t nose_cone_ret = attempt_esp_now_add_peer(&nose_cone_peer_info, ATTEMPT_MAX_ATTEMPTS, ATTEMPT_INTERVAL);
  
  esp_now_register_send_cb(on_esp_now_data_sent);
  esp_now_register_recv_cb(on_esp_now_data_received);

  if (ignition_bay_ret) {
    ESP_LOGE("Setup", "Failed to connect to ignition bay");
    notify_char->setValue("B1_DISCONNECTED");
    notify_char->notify();
  } 

  if (nose_cone_ret) {
    ESP_LOGE("Setup", "Failed to connect to nose cone");
    notify_char->setValue("B2_DISCONNECTED");
    notify_char->notify();
  }
}

void loop() {
  // Don't need code here
}
