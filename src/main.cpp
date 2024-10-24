#include <Arduino.h>
// include all headers even though not used
#include <ArduinoJson.h>
#include <Servo.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include "wifi.h"
#include "serial.h"
#include "socket.h"
#include "motor.h"

void setup() {
  setupSerial();

  setupWifi();

  setupSocket();

  setupServo();
}

void loop() {
  loopSerial();
  loopSocket();
}