# SlowMotionServo

A Slow Motion Servo Library for Arduino

SlowMotionServo provides a way to drive one or many servos slowly. Instead of
setting the position, each servo has 2 trajectories, ie 2 functions angle(t)
where t is the time. The first function is used to compute the angle of the
servo when travelling from minimum to maximum angle and the second one is used
when travelling from the maximum to the minimum angle.

The function is normalized between 0.0 and 1.0, both for the angle and the time.
So the angle varies from the minimum angle set for the servo to the maximum
angle set for the servo so that the minimum angle corresponds to 0.0 and the
maximum angle corresponds to 1.0. time 0.0 is the start of the movement and time
1.0 is the end of the movement. By default the speed is set so that travelling
from the minimum angle to the maximum angle takes 10s. The same exists when
travelling from the maximum to the minimum angle and both travels have their
own speed setting.

3 trajectories are available and you can add more trajectories by deriving from
the class SlowMotionServo:
* linear trajectory. angle = t
* sinusoidal trajectory. angle = 1.0 - cos(t * PI))/2.0
* sinusoidal trajectory with a bounce. When t <= 0.79, angle = 1.0 - cos(t * PI))/1.8. Otherwise, timeOff = 10.0 * (t - 0.55) and angle = 0.834 + 1.0 / (timeOff * timeOff)

## Using the library

As usual, you have to include it at the beginning of your sketch but you also
have to include the Servo library:

```
#include <Servo.h>
#include <SlowMotionServo.h>
```

Then you have to instantiate as many object as servos you want to drive. For
that you choose what kind of trajectory you want:

SMSLinear is the class with a linear trajectory. SMSSmooth is the class with a
sinusoidal trajectory and SMSSmoothBounce is the class with a sinusoidal
trajectory with a bounce.

```
SMSLinear myServo; /* Servo with linear trajectory */
```

The following functions are available:

### setMin(min)

Set the minimum angle of the servo. The angle is expressed in its equivalency
in microseconds. The value can range from 544 to 2400. A value lower than 544
will be reset to 544 and a value greater than 2400 will be reset to 2400.
If the value is greater than the maximum angle it is reset to the maximum
angle.

### setMax(max)

Set the maximum angle of the servo. The angle is expressed in its equivalency
in microseconds. The value can range from 544 to 2400. A value lower than 544
will be reset to 544 and a value greater than 2400 will be reset to 2400.
If the value is lower than the minimum angle it is reset to the minimum
angle.

### setMinMax(min, max)

Set the minimum and maximum angles of the servo. The angle is expressed in
its equivalency in microseconds. The value can range from 544 to 2400.
A value lower than 544 will be reset to 544 and a value greater than 2400
will be reset to 2400.
If the minimum angle is greater than the maximum angle, both angles are set
to the average value. For instance if you set the minimum angle to 2000 and
the maximum angle to 1000, both angles will be set to 1500.

### setPin(pin)

Set the pin to which the servo is attached.

### setMinToMaxSpeed(speed)

Set the speed of the servo when travelling from the minimum to the maximum
angle. speed is a floating point number. A speed of 1.0 corresponds to a
10s travelling.

### setMaxToMinSpeed(speed)

Set the speed of the servo when travelling from the maximum to the minimum
angle. speed is a floating point number. A speed of 1.0 corresponds to a
10s travelling.

### setSpeed(speed)

Set the speed of the servo when travelling from the minimum to the maximum
angle and from the maximum to the minimum angle.
speed is a floating point number. A speed of 1.0 corresponds to a
10s travelling.

### setInitialPosition(position)

Set the initial position of the servo. The position is a floating point number
ranging from 0.0 to 1.0. If the value is greater than 1.0, ti is reset to 1.0
and if lower than 0.0, it is reset to 0.0

### setReverted(reverted)

Reverse the movement. By default reverted is false. If set to true, the trajectories are mirrored across an axis at time = 0.5.

### goTo(position)

Go to the specified position by following the trajectory.
The position is a floating point number
ranging from 0.0 to 1.0. If the value is greater than 1.0, ti is reset to 1.0
and if lower than 0.0, it is reset to 0.0

### goToMin()

Equivalent to goTo(0.0)

### goToMax()

Equivalent to goTo(1.0)

### setDetachAtMin(detach)

detach is a boolean. If true, the servo is detached when the minimum position
is reached. The servo is no longer driven. This is useful when the servo has
to push against an elastic restoring force. If false the servo continues to be
driven.

### setDetachAtMax(detach)

detach is a boolean. If true, the servo is detached when the maximum position
is reached. The servo is no longer driven. This is useful when the servo has
to push against an elastic restoring force. If false the servo continues to be
driven.

### setDetach(detach)

detach is a boolean. If true, the servo is detached when the minimum or the
maximum positions
are reached. The servo is no longer driven. This is useful when the servo has
to push against an elastic restoring force. If false the servo continues to be
driven.

### isStopped()

Returns true if the servo is stopped.

### pin()

Returns a ```byte``` which is the number of the pin. 255 is returned if the object
has not been connected to any pin.

### detachAtMin()

Returns true if the servo is detached when reaching the minimum position.

### detachAtMax()

Returns true if the servo is detached when reaching the maximum position.

### minimumPulse()

Returns an ```uint16_t``` which is the pulse width in microseconds corresponding to the minimum servo position.

### maximumPulse()

Returns an ```uint16_t``` which is the pulse width in microseconds corresponding to the maximum servo position.

### minToMaxSpeed()

Returns a float which is the speed of the servo when traveling from minimum to maximum position.

### maxToMinPulse()

Returns a float which is the speed of the servo when traveling from maximum to minimum position.

### isReverted()

Returns true if the movement is reverted.

### SlowMotionServo::setDelayUntilStop(delay)

This class function set the delay between the time the servos reach their
minimum or maximum
position and the the time they are detached. Because the mechanic is always
late
compared to the program, detaching immediately the servos would prevent them
to reach their mechanical position. This is set once for all the servos and
used only for servos and positions for which setDetach(true) is specified.

### SlowMotionServo::update()

Update the positions of all the servos. This class function has to be called
in loop() as frequently as possible.

## Example

Let's drive a single servo with a smooth movement forward and backward.

First, include the libraries:

```
#include <Servo.h>
#include <SlowMotionServo.h>
```

Second, instantiate an object and a float to hold the target position:

```
SMSSmooth myServo; /* Servo with linear trajectory */
float target = 0.0;
```

Third, initialize it in setup(). Be careful actual minimum and maximum
positions of a servo may be greater and/or lower than positions allowed by
the Servo and SlowMotionServo libraries:

```
void setup()
{
  myServo.setInitialPosition(target);
  myServo.setMinMax(700, 2000);
  myServo.setPin(3); /* the servo is connected to pin 3 */
}
```

Fourth, if the servo is stopped compute the new target position by doing
the subtraction 1.0 - target so that if target is 0.0, the new target is
1.0 and if target is 1.0 the new target is 0.0. And of course call update.

```
void loop()
{
  if (myServo.isStopped()) {
    target = 1.0 - target;
    myServo.goTo(target);
  }

  SlowMotionServo::update();
}
```

The full sketch:

```
#include <Servo.h>
#include <SlowMotionServo.h>

SMSSmooth myServo; /* Servo with linear trajectory */
float target = 0.0;

void setup() {
  myServo.setInitialPosition(target);
  myServo.setMinMax(700, 2000);
  myServo.setPin(3); /* the servo is connected to pin 3 */
}

void loop() {
  if (myServo.isStopped()) {
    target = 1.0 - target;
    myServo.goTo(target);
  }

  SlowMotionServo::update();
}
```

## How to define your own trajectory.

To do that, you have to inherit from the SlowMotionServo class and redefine
the slopeUp and slopeDown member functions. Let's take the
SMSSmooth class as example:

```
class SMSSmooth : public SlowMotionServo
{
public:
  virtual float slopeUp(float time);
  virtual float slopeDown(float time);
  float slope(float time) { return (1.0 - cos(time * PI))/2.0; }
};
```

Because trajectories are the same from min to max and max to min, we define
a new member function, slope, that defines the trajectory. This function is
called by slopeUp and slopeDown:

```
float SMSSmooth::slopeUp(float time)
{
  return slope(time);
}

float SMSSmooth::slopeDown(float time)
{
  return slope(time);
}
```
