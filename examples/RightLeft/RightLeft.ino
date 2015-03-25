#include <Servo.h>
#include <SlowMotionServo.h>

SMSSmooth servo;

float nextPos = 0.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  servo.setMin(1000);
  servo.setMax(2000);
  servo.goTo(1.0);
}

void loop() {
  // put your main code here, to run repeatedly:
  SlowMotionServo::update();
  
  if (servo.isStopped()) {
    servo.goTo(nextPos);
    nextPos = 1.0 - nextPos;
  }
}
