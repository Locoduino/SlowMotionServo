/*
 * Slow Motion Servo Library for Arduino
 *
 * Copyright Jean-Luc Béchennec 2015
 *
 * This software is distributed under the GNU Public Licence v2 (GPLv2)
 *
 * Please read the LICENCE file
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <SlowMotionServo.h>

static const byte NOPIN = 255;
enum {
	SERVO_INIT, 
	SERVO_STOPPED, 
	SERVO_UP, 
	SERVO_DOWN, 
	SERVO_DELAYED_UP, 
	SERVO_DELAYED_DOWN
};

/*
 * Static member to store a list of servos. This allow to update the
 * positions of all declared servos easily.
 */
SlowMotionServo *SlowMotionServo::sServoList = NULL;

/*
 * Delay between the last pulse chage in a movement and the time the
 * servos go to the SERVO_STOPPED state.
 * 10 ms by default. Can be changed by SlowMotionServo::setDelayUntilStop
 */
unsigned int SlowMotionServo::sDelayUntilStop = 10;

/*
 * Constructor :
 * Set the connection pin at NOPIN.
 * The state is SERVO_STOPPED.
 * The servo is not detached by default when stopped.
 * Min and max pulse are set according to min and max value of the servo library.
 * Servo is at the min.
 * Speed are set to 1 (mTimeFactorUp and mTimeFactorDown).
 * The servo is added to the list.
 */
SlowMotionServo::SlowMotionServo() :
  mPin(NOPIN),
  mState(SERVO_INIT),
  mDetachAtMin(false),
  mDetachAtMax(false),
  mMinPulse(1000),
  mMaxPulse(2400),
  mInitialRelativeTime(0.0),
  mTargetRelativeTime(0.0),
  mCurrentRelativeTime(0.0),
  mTimeFactorUp(0.0001),
  mTimeFactorDown(0.0001)
{
  mNext = sServoList;
  sServoList = this;
}

/*
 * Constructor with the pin
 */
SlowMotionServo::SlowMotionServo(byte pin) :
  SlowMotionServo()
{
  mPin = pin;
}

/*
 * Function to constrain a pulse between the values allowed
 * by the servo library
 */
static unsigned int constrainPulse(unsigned int pulse)
{
  if (pulse < 544)  pulse = 544;
  else if (pulse > 2400) pulse = 2400;
  return pulse;
}

/*
 * Function to constrain a position between 0.0 and 1.0
 */
static float constrainPosition(float position)
{
  if (position > 1.0) position = 1.0;
  else if (position < 0.0) position = 0.0;
  return position;
}

/*
 * Method to update the current pulse of the servo
 * according to the min and max
 */
void SlowMotionServo::updatePulseAccordingToMinMax()
{
  unsigned int currentPosition = readMicroseconds();
  if (currentPosition < mMinPulse) currentPosition = mMinPulse;
  else if (currentPosition > mMaxPulse) currentPosition = mMaxPulse;
  writeMicroseconds(currentPosition);
}

/*
 * Change the min and max. Constrain both. Check min <= max.
 * If not min and max are sit to the average of both.
 */
void SlowMotionServo::setMinMax(unsigned int minPulse, unsigned int maxPulse)
{
  minPulse = constrainPulse(minPulse);
  maxPulse = constrainPulse(maxPulse);
  if (minPulse <= maxPulse) {
    mMinPulse = minPulse;
    mMaxPulse = maxPulse;
  }
  else {
    mMinPulse = mMaxPulse = (minPulse + maxPulse) / 2;
  }
  updatePulseAccordingToMinMax();
}

/*
 * Change the min. Constrain it. Check min <= max.
 * If not min is set to max.
 */
void SlowMotionServo::setMin(unsigned int minPulse)
{
  minPulse = constrainPulse(minPulse);
  if (minPulse > mMaxPulse) minPulse = mMaxPulse;
  mMinPulse = minPulse;
  updatePulseAccordingToMinMax();
}

/*
 * Change the max. Constrain it. Check min <= max.
 * If not max is set to min.
 */
void SlowMotionServo::setMax(unsigned int maxPulse)
{
  maxPulse = constrainPulse(maxPulse);
  if (maxPulse < mMinPulse) maxPulse = mMinPulse;
  mMaxPulse = maxPulse;
  updatePulseAccordingToMinMax();
}

/*
 * Set the initial position of the servo. The initial position
 * is supposed to be retrieved from the EEPROM or from
 * an other mean
 */
void SlowMotionServo::setInitialPosition(float position)
{
  position = constrainPosition(position);
  mTargetRelativeTime = mInitialRelativeTime = mCurrentRelativeTime = position;
}

/*
 * Go to a new position. Compute the direction and change the state
 * of the servo. Attach it if the pin is set.
 */
void SlowMotionServo::goTo(float position)
{
  position = constrainPosition(position);
  mTargetRelativeTime = position;
  mInitialRelativeTime = mCurrentRelativeTime;
  if (mTargetRelativeTime > mInitialRelativeTime) {
    mState = SERVO_UP;
    if (mPin != NOPIN) attach(mPin);
  }
  else if (mTargetRelativeTime < mInitialRelativeTime) {
    mState = SERVO_DOWN;
    if (mPin != NOPIN) attach(mPin);
  }
  else mState = SERVO_STOPPED;
  mStartTime = millis();
}

/*
 * Update the position of the servo.
 */
void SlowMotionServo::updatePosition()
{
  float position;
  float time;
  unsigned long date = millis();

  switch (mState) {
    case SERVO_INIT:
    	position = slopeUp(mCurrentRelativeTime);
      writeMicroseconds(position * (mMaxPulse - mMinPulse) + mMinPulse);
      mState = SERVO_STOPPED;
      break;
    case SERVO_UP:
      mCurrentRelativeTime = (float)(date - mStartTime) * mTimeFactorUp +
                             mInitialRelativeTime;
      if (mCurrentRelativeTime > mTargetRelativeTime) {
        mCurrentRelativeTime = mTargetRelativeTime;
        mState = SERVO_DELAYED_UP;
      }
      position = slopeUp(mCurrentRelativeTime);
      writeMicroseconds(position * (mMaxPulse - mMinPulse) + mMinPulse);
      break;
    case SERVO_DOWN:
      mCurrentRelativeTime = mInitialRelativeTime -
                             (float)(date - mStartTime) * mTimeFactorDown;
      if (mCurrentRelativeTime < mTargetRelativeTime) {
        mCurrentRelativeTime = mTargetRelativeTime;
        mState = SERVO_DELAYED_DOWN;
      }
      position = slopeDown(mCurrentRelativeTime);
      writeMicroseconds(position * (mMaxPulse - mMinPulse) + mMinPulse);
      break;
    case SERVO_DELAYED_UP:
      if ((millis() - mStartTime) > sDelayUntilStop) {
        mState = SERVO_STOPPED;
        if (mDetachAtMax) detach();
        mStartTime = date;
      }
      break;
    case SERVO_DELAYED_DOWN:
      if ((millis() - mStartTime) > sDelayUntilStop) {
        mState = SERVO_STOPPED;
        if (mDetachAtMin) detach();
        mStartTime = date;
      }
      break;
  }
}

/*
 * Returns true is the servo is stopped
 */
bool SlowMotionServo::isStopped()
{
  return mState == SERVO_STOPPED;
}

/*
 * Class method to update all the servos
 */
void SlowMotionServo::update()
{
  SlowMotionServo *servo = sServoList;
  while (servo != NULL) {
    servo->updatePosition();
    servo = servo->mNext;
  }
}

/*
 * Set the delay between the end of a movement and the time the
 * servo is stopped and detached if needed.
 */
void SlowMotionServo::setDelayUntilStop(unsigned int delayUntilStop)
{
  sDelayUntilStop = delayUntilStop;
}

float SMSLinear::slopeUp(float time)
{
  return slope(time);
}

float SMSLinear::slopeDown(float time)
{
  return slope(time);
}

float SMSSmooth::slopeUp(float time)
{
  return slope(time);
}

float SMSSmooth::slopeDown(float time)
{
  return slope(time);
}

float SMSSmoothBounce::slopeUp(float time)
{
  if (time <= 0.795) {
    return (1.0 - cos(time * PI))/1.8;
  }
  else {
    float timeOff = 10.0 * (time - 0.55);
    return (0.83 + 1.0 / (timeOff * timeOff));
  }
}

float SMSSmoothBounce::slopeDown(float time)
{
  return (1.0 - cos(time * PI))/2.265;
}
