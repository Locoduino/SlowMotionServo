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

enum { SERVO_STOPPED, SERVO_UP, SERVO_DOWN, SERVO_DELAYED };

SlowMotionServo *SlowMotionServo::sServoList = NULL;

SlowMotionServo::SlowMotionServo() :
  mState(SERVO_STOPPED),
  mMinPulse(544),
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

int SlowMotionServo::constrainPulse(int pulse)
{
  if (pulse < 544)  pulse = 544;
  else if (pulse > 2400) pulse = 2400;
  return pulse;
}

void SlowMotionServo::setMinMax(int minPulse, int maxPulse)
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
}

void SlowMotionServo::setMin(int minPulse)
{
  minPulse = constrainPulse(minPulse);
  if (minPulse > mMaxPulse) minPulse = mMaxPulse;
  mMinPulse = minPulse;
}

void SlowMotionServo::setMax(int maxPulse)
{
  maxPulse = constrainPulse(maxPulse);
  if (maxPulse < mMinPulse) maxPulse = mMinPulse;
  mMaxPulse = maxPulse;
}

void SlowMotionServo::goTo(float position)
{
  if (position > 1.0) position = 1.0;
  else if (position < 0.0) position = 0.0;
  mTargetRelativeTime = position;
  mInitialRelativeTime = mCurrentRelativeTime;
  if (mTargetRelativeTime > mInitialRelativeTime) mState = SERVO_UP;
  else if (mTargetRelativeTime < mInitialRelativeTime) mState = SERVO_DOWN;
  else mState = SERVO_STOPPED;
  mStartTime = millis();
}

void SlowMotionServo::updatePosition()
{
  float position;
  float time;
  unsigned long date = millis();

  switch (mState) {
    case SERVO_UP:
      mCurrentRelativeTime = (float)(date - mStartTime) * mTimeFactorUp +
                             mInitialRelativeTime;
      if (mCurrentRelativeTime > mTargetRelativeTime) {
        mCurrentRelativeTime = mTargetRelativeTime;
        mState = SERVO_DELAYED;
      }
      position = slope(mCurrentRelativeTime);
      writeMicroseconds(position * (mMaxPulse - mMinPulse) + mMinPulse);
      break;
    case SERVO_DOWN:
      mCurrentRelativeTime = mInitialRelativeTime -
                             (float)(date - mStartTime) * mTimeFactorDown;
      if (mCurrentRelativeTime < mTargetRelativeTime) {
        mCurrentRelativeTime = mTargetRelativeTime;
        mState = SERVO_DELAYED;
      }
      position = slope(mCurrentRelativeTime);
      writeMicroseconds(position * (mMaxPulse - mMinPulse) + mMinPulse);
      break;
    case SERVO_DELAYED:
      if ((millis() - mStartTime) > 10) {
        mState = SERVO_STOPPED;
        mStartTime = date;
      }
      break;
  }
}

bool SlowMotionServo::isStopped()
{
  return mState == SERVO_STOPPED;
}

void SlowMotionServo::update()
{
  SlowMotionServo *servo = sServoList;
  while (servo != NULL) {
    servo->updatePosition();
    servo = servo->mNext;
  }
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
