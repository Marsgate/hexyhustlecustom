#include <Bluepad32.h>
#include <Preferences.h>
#include "pbr.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
Preferences preferences;

// motors
Motor leftDrive;
Motor rightDrive;
Motor intake;
Motor hang;

// servos
ServoMotor descore;
ServoMotor flipper;

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
    digitalWrite(ONBOARD_LED, LOW);
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
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

void processGamepad(ControllerPtr ctl) {
    if (ctl->a()) {
        static int colorIdx = 0;
        // Some gamepads like DS4 and DualSense support changing the color LED.
        // It is possible to change it by calling:
        switch (colorIdx % 3) {
            case 0:
                // Red
                ctl->setColorLED(255, 0, 0);
                break;
            case 1:
                // Green
                ctl->setColorLED(0, 255, 0);
                break;
            case 2:
                // Blue
                ctl->setColorLED(0, 0, 255);
                break;
        }
        colorIdx++;
    }

    // // winch
    // if (ctl->x()) {
    //     winch.write(180);
    // } else if (ctl->y()) {
    //     winch.write(0);
    // } else {
    //     winch.write(90);
    // }

    // intake
    if (ctl->r1()) {
        intake.move(100);
    } else if (ctl->r2()) {
        intake.move(-100);
    } else {
        intake.move(0);
    }

    // lift
    // if (ctl->l1()) {
    //     liftPosition += 20;
    // } else if (ctl->l2()) {
    //     liftPosition -= 20;
    // }

    // if (liftPosition > liftUpperBound) {
    //     liftPosition = liftUpperBound;
    // } else if (liftPosition < liftLowerBound) {
    //     liftPosition = liftLowerBound;
    // }

    // lift.writeMicroseconds(liftPosition);
  
    // drive
    // int x = ctl->axisRX();
    // int y = ctl->axisY();

    // if (abs(x) < 100) {
    //     x = 0;
    // }
    // if (abs(y) < 100) {
    //     y = 0;
    // }

    // x = map(x, 512, -512, -90, 90);
    // y = map(y, 512, -512, 0, 180);

    // Serial.print("X:");
    // Serial.print(x);
    // Serial.print("    Y:");
    // Serial.println(y);
    
    // Serial.println(liftPosition);

    // leftDrive.write(y-x);
    // rightDrive.write(map((y+x), 180, 0, 0, 180));
    

    // Another way to query controller data is by getting the buttons() function.
    // See how the different "dump*" functions dump the Controller info.
    // dumpGamepad(ctl);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

// Arduino setup function. Runs in CPU 1
void setup() {
    pinMode(ONBOARD_LED, OUTPUT);

    // motors
    leftDrive.init(ESC1A);
    rightDrive.init(ESC1B);
    intake.init(ESC2A);
    hang.init(ESC2B);

    // servos
    descore.init(S1);
    flipper.init(S2);


    Serial.begin(115200);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    BP32.enableBLEService(false); // set to true if using a BLE controller (xbox)

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
// Arduino loop function. Runs in CPU 1.
void loop() {

    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();

    // if the boot button is pressed, send the board into pairing mode
    if (digitalRead(0) == LOW) {
        Serial.println("Entering pairing mode on next reboot");
        preferences.begin("robot", false);
        preferences.putBool("paired", false); // set robot to unpair at next reboot
        preferences.end();

        digitalWrite(2, HIGH); // turn on the LED to let the user know we are in pairing mode
        
        while (digitalRead(0) == LOW) vTaskDelay(10);
        esp_restart();
    }

    //     vTaskDelay(1);
    vTaskDelay(10);
}
