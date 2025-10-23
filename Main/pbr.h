#pragma once
#include <ESP32Servo.h>
#include <Preferences.h>
#include <Bluepad32.h>

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
#define ESC4B 23

#define S1 22
#define S2 21
#define S3 19
#define S4 18
#define S5 5
#define S6 4

#define ONBOARD_LED 2
#define BOOT_BTN 0

extern Preferences preferences;
extern ControllerPtr ctl;

class Motor {
  public:
    void init(int pin, bool reversed = false, int lowerLimit = MOTOR_MICROS_MIN, int upperLimit = MOTOR_MICROS_MAX);
    void move(int speed);
    int getSpeed();
  private:
    Servo _servo;
    int _speed;
    int _lowerLimit;
    int _upperLimit;
    bool reversed;
};

class ServoMotor {
  public:
    void init(int pin, bool reversed = false, int lowerLimit = SERVO_MICROS_MIN, int upperLimit = SERVO_MICROS_MAX);
    void moveAbsolute(int position);
    void moveRelative(int step);
    int getPosition();
  private:
    Servo _servo;
    int _position;
    int _lowerLimit;
    int _upperLimit;
    bool reversed;
};

void pbrInit();
void enterPairingMode();
void onConnectedController(ControllerPtr controller);
void onDisconnectedController(ControllerPtr controller);
void dumpGamepad(ControllerPtr ctl);
