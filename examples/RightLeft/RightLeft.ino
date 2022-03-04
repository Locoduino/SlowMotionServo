#include <SlowMotionServo.h>

SMSSmoothBounce servo[8];

float nextPos[8];
byte i = 0;
unsigned long startWaitTime[8];
bool waiting[8];

void setup() {
  /*
   * Setup all the servos with reduced min and max, speed of 2 and detach
   * Pins are from 3 ro 9 on the Arduino.
   * Note: you cannot power 8 servos with an Arduino. This sketch
   * is only to check the computation poxer is enough to drive 8 servos
   * simultaneously. Connect just one servo to pin 3.
   */
  for (byte num = 0; num < 8; num++) {
    servo[num].setMin(600);
    servo[num].setMax(2200);
    servo[num].setSpeed(2.0);
    servo[num].setPin(num + 2);
    servo[num].setDetach(true);
    servo[num].goTo(1.0);
    nextPos[num] = 0.0;
    startWaitTime[num] = 0;
    waiting[num] = false;
  }
}

void loop() {
  /* servos are driven along their trajectory */
  SlowMotionServo::update();

  /* At each loop turn, the current servo is tested */
  if (servo[i].isStopped()) {
    /* if it is stopped and ... */
    if (!waiting[i]) {
      /*
       * it is not waiting, so it has just stopped.
       * start the chronometer and wait
       */
      startWaitTime[i] = millis();
      waiting[i] = true;
    } else if (millis() - startWaitTime[i] > 3000) {
      /*
       * The servo is waiting and time elapsed
       * So it is no longer waiting and we start to move it
       */
      waiting[i] = false;
      servo[i].goTo(nextPos[i]);
      /* and we compute the next position */
      nextPos[i] = 1.0 - nextPos[i];
    }
  }
  i++; /* Go to the next servo */
  if (i == 8)
    i = 0; /* loop back to servo 0 */
}
