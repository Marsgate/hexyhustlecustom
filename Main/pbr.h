#pragma once
#include <ESP32Servo.h>

#define MOTOR_MICROS_MIN 1000
#define MOTOR_MICROS_MAX 2000

#define SERVO_MICROS_MIN 500
#define SERVO_MICROS_MAX 2500

#define ESC1A 13
#define ESC1B 14
#define ESC2A 27
#define ESC2B 26
#define ESC3A 25
#define ESC3B 33
#define ESC4A 32
#define ESC4B 35

#define S1 34
#define S2 15
#define S3 2
#define S4 4
#define S5 5
#define S6 18

#define ONBOARD_LED 2

class Motor {
  public:
    void init(int pin);
    void move(int speed);
    int getSpeed();
  private:
    Servo _servo;
    int _speed;
};

class ServoMotor {
  public:
    void init(int pin, int lowerLimit = SERVO_MICROS_MIN, int upperLimit = SERVO_MICROS_MAX);
    void moveAbsolute(int position);
    void moveRelative(int step);
    int getPosition();
  private:
    Servo _servo;
    int _position;
    int _lowerLimit;
    int _upperLimit;
};