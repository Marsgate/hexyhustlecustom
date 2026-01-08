#include "pbr.h"

int M1[2] = {14, 27};
int M2[2] = {13, 12};
int M3[2] = {33, 32};
int M4[2] = {26, 25};
int M5[2] = {21, 19};
int M6[2] = {23, 22};

int S1 = 16;
int S2 = 17;
int S3 = 5;
int S4 = 18;

// Motor
void Motor::init(
  int port[2],
  bool reversed
) {
  pinMode(port[0], OUTPUT);
  pinMode(port[1], OUTPUT);
  _reversed = reversed;
  _fwdPin = port[0];
  _revPin = port[1];
}

void Motor::move(int speed) {
  // cap speed to -100,100
  if (speed > 100) {
    speed = 100;
  } else if (speed < -100) {
    speed = -100;
  }

  // apply the reversal
  if (_reversed == true) {
    speed = -speed;
  }

  // save the speed to the object
  _speed = speed;

  // convert to PWM and write to the motor
  speed = map(speed, -100, 100, -255, 255);
  analogWrite(_fwdPin, speed);
  analogWrite(_revPin, -speed);
}

int Motor::getSpeed() {
  return _speed;
}

// ServoMotor
void ServoMotor::init(
  int port,
  int initialPosition,
  int lowerLimit,
  int upperLimit
) {
  pinMode(port, OUTPUT);
  _lowerLimit = lowerLimit;
  _upperLimit = upperLimit;

  // double attatch due to a bug
  _servo.attach(port, lowerLimit, upperLimit);
  _servo.attach(port, lowerLimit, upperLimit);

  _servo.writeMicroseconds(initialPosition);
}

void ServoMotor::moveAbsolute(int position) {
  // cap the servo postion to the limits
  if (_position > _upperLimit) {
    position = _upperLimit;
  } else if (_position < _lowerLimit) {
    position = _lowerLimit;
  }

  // save the position to the object 
  _position = position;

  // write the position as microseconds
  _servo.writeMicroseconds(_position);
}

void ServoMotor::moveRelative(int step) {
  moveAbsolute(_position + step);
}

int ServoMotor::getPosition() {
  return _position;
}


Preferences preferences;

void pbrInit() {
  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(BOOT_BTN, INPUT);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // check for paired controller
  preferences.begin("robot", false);
  if (preferences.getBool("paired") == true) {
    Serial.println ("Controller already paired");
    BP32.enableNewBluetoothConnections(false); // do not allow new connections if there is a paired controller
  } else {
    Serial.println ("No paired controller");

    // forget previous controllers and connect to a new controller
    BP32.forgetBluetoothKeys();
    BP32.enableNewBluetoothConnections(true);
    preferences.putBool("paired", true); // assume a controller was paired
    digitalWrite(ONBOARD_LED, HIGH); // turn on the LED to let the user know we are in pairing mode
  }
  preferences.end();
}

void enterPairingMode() {
  Serial.println("Entering pairing mode on next reboot");
  preferences.begin("robot", false);
  preferences.putBool("paired", false); // set robot to unpair at next reboot
  preferences.end();

  digitalWrite(ONBOARD_LED, HIGH); // turn on the LED to let the user know we are in pairing mode
  
  while (digitalRead(BOOT_BTN) == LOW) vTaskDelay(10); // wait until button is release to restart
  esp_restart();
}

void onConnectedController(ControllerPtr controller) {
  if (ctl == nullptr) {
      Serial.println("Controller is connected");
      ctl = controller;
  }
  digitalWrite(ONBOARD_LED, LOW);
}

void onDisconnectedController(ControllerPtr controller) {
  if (ctl == controller) {
    Serial.println("Controller disconnected");
    ctl = nullptr;
  }
  digitalWrite(ONBOARD_LED, HIGH);
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}
