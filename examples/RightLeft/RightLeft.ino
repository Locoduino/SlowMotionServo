#include <Servo.h>
#include <SlowMotionServo.h>

SMSSmoothBounce servo;

float nextPos = 0.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  servo.setMin(600);
  servo.setMax(2200);
  servo.setMinToMaxSpeed(0.0002);
  servo.setMaxToMinSpeed(0.0002);
  servo.attach(7);
  servo.goTo(1.0);
}

void loop() {
  // put your main code here, to run repeatedly:
  SlowMotionServo::update();
  
  if (servo.isStopped()) {
    delay(2000);
    servo.goTo(nextPos);
    nextPos = 1.0 - nextPos;
  }
}
