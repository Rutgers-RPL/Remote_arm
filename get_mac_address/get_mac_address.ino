#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  uint8_t mac[6];
  WiFi.macAddress(mac);

  Serial.println("\n--- MAC Address Utility ---");

  Serial.print("Standard: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Hex Array: {");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    if (mac[i] < 0x10) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(", ");
  }
  Serial.println("}");
  
  Serial.println("---------------------------");
}

void loop() {
  // Don't need code here
}