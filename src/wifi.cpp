#include <WiFiManager.h>
#include <Arduino.h>

WiFiManager wm;

String getMacAddress(){
  return WiFi.macAddress();
}

void setupWifi() {

  WiFi.mode(WIFI_STA);

  bool res = wm.autoConnect("Cat Food Hotspot", "");

  if (!res) {
    Serial.println("Failed to connect");
  }
  else {
    Serial.println("Connected");
  }
}

void resetWifi() {
  Serial.println("Resetting WiFi settings");
  wm.resetSettings();
  Serial.println("WiFi settings reset");
  EspClass::eraseConfig();
  delay(2000);
  EspClass::reset();
}