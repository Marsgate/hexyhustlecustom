#include "pbr.h"

ControllerPtr ctl;

// motors
Motor leftDrive;
Motor rightDrive;
Motor intake;
Motor hang;

// servos
ServoMotor descore;
ServoMotor flipper;

void driverControls() {
    // hang
    if (ctl->x()) {
        hang.move(100);
    } else if (ctl->y()) {
        hang.move(-100);
    } else {
        hang.move(0);
    }

    // intake
    if (ctl->r1()) {
        intake.move(100);
    } else if (ctl->r2()) {
        intake.move(-100);
    } else {
        intake.move(0);
    }

    // descore
    if (ctl->l1()) {
        descore.moveRelative(20);
    } else if (ctl->l2()) {
        descore.moveRelative(-20);
    }

    // flipper
    if (ctl->a()) {
        flipper.moveRelative(20);
    } else if (ctl->b()) {
        flipper.moveRelative(-20);
    }
  
    // get joysticks
    int x = ctl->axisRX();
    int y = ctl->axisY();

    // deadzone
    if (abs(x) < 100) {
        x = 0;
    }
    if (abs(y) < 100) {
        y = 0;
    }

    // map to +/- 100
    x = map(x, 512, -512, 100, -100);
    y = map(y, 512, -512, -100, 100);

    leftDrive.move(y-x);
    rightDrive.move(y+x);
}

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);

    pbrInit();
    BP32.enableBLEService(false); // set to true for BLE controllers (xbox)

    // motors
    leftDrive.init(ESC1A);
    rightDrive.init(ESC1B, 1); // reversed
    intake.init(ESC2A);
    hang.init(ESC2B);

    // servos
    descore.init(S1);
    flipper.init(S2);
}

void loop() {
    
    delay(1); // the loop requires a delay

    // if the boot button is pressed, send the board into pairing mode
    if (digitalRead(BOOT_BTN) == LOW) {
        enterPairingMode();
    }

    // get bluetooth data from the controller
    bool dataUpdated = BP32.update();
    if (!dataUpdated || !ctl || !ctl->isConnected() || !ctl->hasData()) {
        return; // if no controller is connected and returning data, skip the driver controls
    }

    // dumpController() // uncomment to print controller inputs to the serial monitor

    driverControls();
}
