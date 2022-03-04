/*
 * Drive a servo by using a push button
 * Uses the Bounce2 library. This library can be installed using the library
 * manager
 */

#include <Bounce2.h>
#include <SlowMotionServo.h>

SMSSmoothBounce myServoRight;
SMSSmoothBounce myServoLeft;
Bounce myButton;

const byte servoRightPin = 4;
const byte servoLeftPin = 3;
const byte buttonPin = 5;
const byte ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);
  /* when the button is pressed, the input is LOW */
  pinMode(buttonPin, INPUT_PULLUP);

  myButton.attach(buttonPin);
  /* scan interval for debounce */
  myButton.interval(5);

  myServoRight.setMin(750);
  myServoRight.setMax(1800);
  myServoLeft.setMin(1100);
  myServoLeft.setMax(2200);

  myServoRight.setSpeed(1.5);
  myServoLeft.setSpeed(1.5);

  myServoLeft.setReverted(true);

  myServoRight.setInitialPosition(0.0);
  myServoLeft.setInitialPosition(0.0);
  myServoRight.setPin(servoRightPin);
  myServoLeft.setPin(servoLeftPin);
  digitalWrite(ledPin, HIGH);
}

void loop() {
  static float servoTarget = 0.0;

  /* update the state of the button */
  myButton.update();
  /* update the position of the servo */
  SlowMotionServo::update();

  if (myServoRight.isStopped() && myServoLeft.isStopped()) {
    digitalWrite(ledPin, LOW);
    if (myButton.fell()) {
      /* look at the button only when the servo is stopped */
      /* change the target */
      servoTarget = 1.0 - servoTarget;
      /* set the new target for the servo */
      myServoRight.goTo(servoTarget);
      myServoLeft.goTo(servoTarget);
      digitalWrite(ledPin, HIGH);
    }
  }
}
