#include "pbr.h"

// Motor
void Motor::init(int pin) {
  pinMode(pin, OUTPUT);

  // double attatch due to a bug
  _servo.attach(pin, MOTOR_MICROS_MIN, MOTOR_MICROS_MAX);
  _servo.attach(pin, MOTOR_MICROS_MIN, MOTOR_MICROS_MAX);
}

void Motor::move(int speed) {
  _speed = map(speed, -100, 100, MOTOR_MICROS_MIN, MOTOR_MICROS_MAX);
  _servo.writeMicroseconds(_speed);
}

int Motor::getSpeed() {
  return _speed;
}


// ServoMotor
void ServoMotor::init(
  int pin,
  int lowerLimit,
  int upperLimit
) {
  pinMode(pin, OUTPUT);
  _lowerLimit = lowerLimit;
  _upperLimit = upperLimit;

  // double attatch due to a bug
  _servo.attach(pin, lowerLimit, upperLimit);
  _servo.attach(pin, lowerLimit, upperLimit);
}

void ServoMotor::moveAbsolute(int position) {
  _position = position;

  if (_position > _upperLimit) {
    position = _upperLimit;
  } else if (_position < _lowerLimit) {
    position = _lowerLimit;
  }

  _servo.writeMicroseconds(_position);
}

void ServoMotor::moveRelative(int step) {
  moveAbsolute(_position + step);
}

int ServoMotor::getPosition() {
  return _position;
}
  