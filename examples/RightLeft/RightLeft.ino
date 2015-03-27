#include <Servo.h>
#include <SlowMotionServo.h>

SMSSmoothBounce servo[8];

float nextPos[8];
byte i = 0;
unsigned long startWaitTime[8];
bool waiting[8];


void setup() {
  // put your setup code here, to run once:
//  Serial.begin(9600);
  
  for (byte num = 0; num < 8; num++) {
    servo[num].setMin(600);
    servo[num].setMax(2200);
    servo[num].setSpeed(2.0);
    servo[num].setPin(num+2);
    servo[num].setDetach(true);
    servo[num].goTo(1.0);
    nextPos[num] = 0.0;
    startWaitTime[num] = 0;
    waiting[num] = false;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  SlowMotionServo::update();
  
  if (servo[i].isStopped()) {
    if (! waiting[i]) {
      startWaitTime[i] = millis();
      waiting[i] = true;
    }
    else if (millis() - startWaitTime[i] > 3000) {
      waiting[i] = false;
      servo[i].goTo(nextPos[i]);
      nextPos[i] = 1.0 - nextPos[i];
    }
  }
  i++;
  if (i == 8) i = 0;
}
