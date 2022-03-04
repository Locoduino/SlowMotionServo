/*
 * Drive a servo by using a push button and proceed to live adjustement using
 * the serial communication. Each command is a unique key: 'u' : enter the
 * adjustement process for the maximum position. 'd' : enter the adjustement
 * process for the minimum position.
 * '+' : increment the pulse for the selected position.
 * '-' : decrement the pulse for the selected position.
 * 'e' : exit the adjustement process.
 *
 * Uses the Bounce2 library. This library can be installed using the library
 * manager
 */

#include <Bounce2.h>
#include <SlowMotionServo.h>

SMSSmoothBounce myServo;
Bounce myButton;

const byte servoPin = 8;
const byte buttonPin = 4;
const byte ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);
  /* when the button is pressed, the input is LOW */
  pinMode(buttonPin, INPUT_PULLUP);

  myButton.attach(buttonPin);
  /* scan interval for debounce */
  myButton.interval(5);

  myServo.setMin(800);
  myServo.setMax(2200);
  myServo.setSpeed(1.5);
  myServo.setReverted(true);
  myServo.setInitialPosition(0.0);
  myServo.setPin(servoPin);
  digitalWrite(ledPin, HIGH);

  Serial.begin(115200);
}

enum { UPDATE_MIN, UPDATE_MAX, NO_UPDATE };
uint8_t adjust = NO_UPDATE;

void loop() {
  static float servoTarget = 0.0;

  /* update the state of the button */
  myButton.update();
  /* update the position of the servo */
  SlowMotionServo::update();

  if (myServo.isStopped()) {
    digitalWrite(ledPin, LOW);
    if (myButton.fell()) {
      /* look at the button only when the servo is stopped */
      /* change the target */
      servoTarget = 1.0 - servoTarget;
      /* set the new target for the servo */
      myServo.goTo(servoTarget);
      digitalWrite(ledPin, HIGH);
    }
  }

  if (Serial.available()) {
    char command = Serial.read();
    switch (adjust) {
    case NO_UPDATE:
      switch (command) {
      case 'u':
        adjust = UPDATE_MAX;
        break;
      case 'd':
        adjust = UPDATE_MIN;
        break;
      }
      break;
    case UPDATE_MAX:
      switch (command) {
      case '+':
        myServo.adjustMax(4);
        break;
      case '-':
        myServo.adjustMax(-4);
        break;
      case 'e':
        myServo.endSetup();
        adjust = NO_UPDATE;
        break;
      }
      break;
    case UPDATE_MIN:
      switch (command) {
      case '+':
        myServo.adjustMin(4);
        break;
      case '-':
        myServo.adjustMin(-4);
        break;
      case 'e':
        myServo.endSetup();
        adjust = NO_UPDATE;
        break;
      }
      break;
    }
  }
}
