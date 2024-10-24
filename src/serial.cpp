#include <Arduino.h>
#include <wifi.h>
#include <constants.h>
#include "motor.h"

void setupSerial() {
  Serial.begin(115200);
}

void loopSerial() {
  if (Serial.available()) {
    String receivedString = Serial.readString();
    receivedString.trim();

    Serial.println("I received: " + receivedString);

    if (receivedString == RESET_EVENT) {
      resetWifi();
    }

    if (receivedString == SERVE_FOOD) {
      moveServo();
    }
  }
}