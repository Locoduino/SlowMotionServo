#include <Servo.h>
#include <SlowMotionServo.h>

SMSSmoothBounce servo[8];

float nextPos[8];

void setup() {
  // put your setup code here, to run once:
//  Serial.begin(9600);
  
  for (byte num = 0; num < 8; num++) {
    servo[num].setMin(600);
    servo[num].setMax(2200);
    servo[num].setMinToMaxSpeed(3.0);
    servo[num].setMaxToMinSpeed(3.0);
    servo[num].attach(num+2);
    servo[num].goTo(1.0);
    nextPos[num] = 0.0;
  }
}

byte i = 0;

void loop() {
  // put your main code here, to run repeatedly:
  SlowMotionServo::update();
  
  if (servo[i].isStopped()) {
    servo[i].goTo(nextPos[i]);
    nextPos[i] = 1.0 - nextPos[i];
    i++;
    if (i == 8) i = 0;
  }
}
