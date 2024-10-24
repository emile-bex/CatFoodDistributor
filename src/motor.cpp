#include <Servo.h>
#include <Arduino.h>
#include "serial.h"

#define SERVO_MIN 20
#define SERVO_MAX 170
Servo servo;

int position = 0;

void setupServo() {
  servo.attach(D5,400, 2500);
}

void moveServo() {
  for (position = SERVO_MIN; position <= SERVO_MAX; position += 1) {
    servo.write(position);
    Serial.println("Position: " + position);
    delay(15);
  }
  delay(2000);
  for (position = SERVO_MAX; position >= SERVO_MIN; position -= 1) {
    servo.write(position);
    Serial.println("Position: " + position);
    delay(15);
  }
}