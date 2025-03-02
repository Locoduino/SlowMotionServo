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

static const byte NOPIN = 127;

enum {
  SERVO_INIT = 0,
  SERVO_STOPPED = 1,
  SERVO_SETUP = 2,
  SERVO_UP = 3,
  SERVO_DOWN = 4,
  SERVO_DELAYED_UP = 5,
  SERVO_DELAYED_DOWN = 6,
  SERVO_NO_STATE = 7
};

#if WITH_DEBUG == 1

static void printState(const byte inState) {
  switch (inState) {
  case SERVO_INIT:
    Serial.print(F("INIT"));
    break;
  case SERVO_STOPPED:
    Serial.print(F("STOPPED"));
    break;
  case SERVO_SETUP:
    Serial.print(F("SETUP"));
    break;
  case SERVO_UP:
    Serial.print(F("UP"));
    break;
  case SERVO_DOWN:
    Serial.print(F("DOWN"));
    break;
  case SERVO_DELAYED_UP:
    Serial.print(F("DELAYED_UP"));
    break;
  case SERVO_DELAYED_DOWN:
    Serial.print(F("DELAYED_DOWN"));
    break;
  default:
    Serial.print(F("*UNKNOWN*"));
    break;
  }
}

static void printlnState(const byte inState) {
  printState(inState);
  Serial.println();
}

#endif

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
 * Min and max pulse are set according to min and max value of the servo
 * library. Servo is at the min. Speed are set to 1 (mTimeFactorUp and
 * mTimeFactorDown). The servo is added to the list.
 */
SlowMotionServo::SlowMotionServo()
    : mPin(NOPIN), mDetachAtMin(false), mDetachAtMax(false), mState(SERVO_INIT),
      mSavedState(SERVO_NO_STATE), mReverted(false), mMinPulse(1000),
      mMaxPulse(2000), mInitialRelativeTime(0.0), mTargetRelativeTime(0.0),
      mCurrentRelativeTime(0.0), mTimeFactorUp(0.0001),
      mTimeFactorDown(0.0001) {
  /* Add the object at the beginning of the list */
  mNext = sServoList;
  sServoList = this;
}

/*
 * Constructor with the pin
 */
SlowMotionServo::SlowMotionServo(byte pin) : SlowMotionServo() { mPin = pin; }

/*
 * Destructor
 * Remove the object from the list
 */
SlowMotionServo::~SlowMotionServo() {
  SlowMotionServo *currentObject = sServoList;
  SlowMotionServo *previousObject = NULL;
  while (currentObject != NULL && currentObject != this) {
    previousObject = currentObject;
    currentObject = currentObject->mNext;
  }
  if (currentObject != NULL) {
    /*
     * Found. normal behavior. If not found there is a problem
     * in the datastructures
     */
    if (previousObject == NULL) {
      /* Found at head, remove it */
      sServoList = sServoList->mNext;
    } else {
      /* Found elsewhere, link the previous to the next */
      previousObject->mNext = currentObject->mNext;
    }
  }
}

/*
 * Function to constrain a pulse between the values allowed
 * by the servo library
 */
static unsigned int constrainPulse(unsigned int pulse) {
  if (pulse < 544)
    pulse = 544;
  else if (pulse > 2400)
    pulse = 2400;
  return pulse;
}

/*
 * Function to constrain a position between 0.0 and 1.0
 */
static float constrainPosition(float position) {
  if (position > 1.0)
    position = 1.0;
  else if (position < 0.0)
    position = 0.0;
  return position;
}

/*
 * Method to update the current pulse of the servo
 * according to the min and max
 */
void SlowMotionServo::updatePulseAccordingToMinMax() {
  unsigned int currentPosition = readMicroseconds();
  if (currentPosition < mMinPulse)
    currentPosition = mMinPulse;
  else if (currentPosition > mMaxPulse)
    currentPosition = mMaxPulse;
  writeMicroseconds(currentPosition);
}

/*
 * Revert the pulse (mirror at time = .5)
 */
unsigned int SlowMotionServo::normalizePos(const unsigned int inPos) {
  return mReverted ? map(inPos, mMinPulse, mMaxPulse, mMaxPulse, mMinPulse)
                   : inPos;
}

/*
 * Change the min and max. Constrain both. Check min <= max.
 * If not min and max are sit to the average of both.
 */
void SlowMotionServo::setMinMax(unsigned int minPulse, unsigned int maxPulse) {
  if (isSettable()) {
    minPulse = constrainPulse(minPulse);
    maxPulse = constrainPulse(maxPulse);
    if (minPulse <= maxPulse) {
      mMinPulse = minPulse;
      mMaxPulse = maxPulse;
    } else {
      mMinPulse = mMaxPulse = (minPulse + maxPulse) / 2;
    }
    updatePulseAccordingToMinMax();
  }
}

/*
 * Change the min. Constrain it. Check min <= max.
 * If not min is set to max.
 */
void SlowMotionServo::setMin(unsigned int minPulse) {
  if (isSettable()) {
    minPulse = constrainPulse(minPulse);
    if (minPulse > mMaxPulse)
      minPulse = mMaxPulse;
    mMinPulse = minPulse;
    updatePulseAccordingToMinMax();
  }
}

/*
 * Change the max. Constrain it. Check min <= max.
 * If not max is set to min.
 */
void SlowMotionServo::setMax(unsigned int maxPulse) {
  if (isSettable()) {
    maxPulse = constrainPulse(maxPulse);
    if (maxPulse < mMinPulse)
      maxPulse = mMinPulse;
    mMaxPulse = maxPulse;
    updatePulseAccordingToMinMax();
  }
}

/*
 * if inReverted is true, the movement is reverted. Otherwise it is not
 */
void SlowMotionServo::setReverted(const bool inReverted) {
  if (isSettable()) {
    mReverted = inReverted;
  }
}

/*
 * Set the speed when travelling from minimum position to maximum positions
 */
void SlowMotionServo::setMinToMaxSpeed(const float speed) {
  if (isSettable()) {
    mTimeFactorUp = speed / 10000.0;
  }
}

/*
 * Set the speed when travelling from maximum position to minimum positions
 */
void SlowMotionServo::setMaxToMinSpeed(const float speed) {
  if (isSettable()) {
    mTimeFactorDown = speed / 10000.0;
  }
}

/*
 * Set the speed when travelling from minimum position to maximum positions and
 * from the maximum to minimum position.
 */
void SlowMotionServo::setSpeed(const float speed) {
  if (isSettable()) {
    mTimeFactorUp = mTimeFactorDown = speed / 10000.0;
  }
}

/*
 * Set the initial position of the servo. The initial position
 * is supposed to be retrieved from the EEPROM or from
 * an other mean
 */
void SlowMotionServo::setInitialPosition(float position) {
  if (isSettable()) {
    position = constrainPosition(position);
    mTargetRelativeTime = mInitialRelativeTime = mCurrentRelativeTime =
        position;
  }
}

/*
 * Go to a new position. Compute the direction and change the state
 * of the servo. Attach it if the pin is set.
 */
void SlowMotionServo::goTo(float position) {
  position = constrainPosition(position);
  mTargetRelativeTime = position;
  mInitialRelativeTime = mCurrentRelativeTime;
  if (mTargetRelativeTime > mInitialRelativeTime) {
    mState = SERVO_UP;
    if (mPin != NOPIN)
      attach(mPin);
  } else if (mTargetRelativeTime < mInitialRelativeTime) {
    mState = SERVO_DOWN;
    if (mPin != NOPIN)
      attach(mPin);
  } else
    mState = SERVO_STOPPED;
  mStartTime = millis();
}

/*
 * Update the position of the servo.
 */
void SlowMotionServo::updatePosition() {
  unsigned long date = millis();

  switch (mState) {
  case SERVO_INIT:
    mPosition = slopeUp(mCurrentRelativeTime);
    writeMicroseconds(
        normalizePos(mPosition * (mMaxPulse - mMinPulse) + mMinPulse));
    mState = SERVO_STOPPED;
    break;
  case SERVO_UP:
    mCurrentRelativeTime =
        (float)(date - mStartTime) * mTimeFactorUp + mInitialRelativeTime;
    if (mCurrentRelativeTime > mTargetRelativeTime) {
      mCurrentRelativeTime = mTargetRelativeTime;
      mState = SERVO_DELAYED_UP;
    }
    mPosition = slopeUp(mCurrentRelativeTime);
    writeMicroseconds(
        normalizePos(mPosition * (mMaxPulse - mMinPulse) + mMinPulse));
    break;
  case SERVO_DOWN:
    mCurrentRelativeTime =
        mInitialRelativeTime - (float)(date - mStartTime) * mTimeFactorDown;
    if (mCurrentRelativeTime < mTargetRelativeTime) {
      mCurrentRelativeTime = mTargetRelativeTime;
      mState = SERVO_DELAYED_DOWN;
    }
    mPosition = slopeDown(mCurrentRelativeTime);
    writeMicroseconds(
        normalizePos(mPosition * (mMaxPulse - mMinPulse) + mMinPulse));
    break;
  case SERVO_DELAYED_UP:
    if ((date - mStartTime) > sDelayUntilStop) {
      mState = SERVO_STOPPED;
      if (mDetachAtMax)
        detach();
      mStartTime = date;
    }
    break;
  case SERVO_DELAYED_DOWN:
    if ((date - mStartTime) > sDelayUntilStop) {
      mState = SERVO_STOPPED;
      if (mDetachAtMin)
        detach();
      mStartTime = date;
    }
    break;
  }
}

/*
 * Returns true if the servo is isStopped
 */
bool SlowMotionServo::isStopped() const { return (mState == SERVO_STOPPED); }

/*
 * Returns true if attributes can be set. That means the servo is in the
 * SERVO_INIT state or in the SERVO_STOPPED state or in the SERVO_SETUP STATED
 */
bool SlowMotionServo::isSettable() const { return (mState <= SERVO_SETUP); }

/*
 * setup the minimum position in Live
 */
void SlowMotionServo::setupMin(uint16_t minPulse) {
  if (isSettable()) {
    if (mState != SERVO_SETUP) {
      mSavedState = mState;
      if (!attached())
        attach(mPin);
    }
    mState = SERVO_SETUP;
    minPulse = constrainPulse(minPulse);
    if (minPulse > mMaxPulse)
      minPulse = mMaxPulse;
    mMinPulse = minPulse;
    writeMicroseconds(minPulse);
  }
}

/*
 * setup the maximum position in Live
 */
void SlowMotionServo::setupMax(uint16_t maxPulse) {
  if (isSettable()) {
    if (mState != SERVO_SETUP) {
      mSavedState = mState;
      if (!attached())
        attach(mPin);
    }
    mState = SERVO_SETUP;
    maxPulse = constrainPulse(maxPulse);
    if (maxPulse < mMinPulse)
      maxPulse = mMinPulse;
    mMaxPulse = maxPulse;
    writeMicroseconds(maxPulse);
  }
}

void SlowMotionServo::endSetup() {
  if (mState == SERVO_SETUP) {
    mState = mSavedState;
    if (isStopped()) {
      if ((mCurrentRelativeTime == 0.0 && mDetachAtMin) ||
          (mCurrentRelativeTime == 1.0 && mDetachAtMax)) {
        detach();
      }
    }
  }
}

/*
 * Class method to update all the servos
 */
void SlowMotionServo::update() {
  SlowMotionServo *servo = sServoList;
  while (servo != NULL) {
    servo->updatePosition();
    servo = servo->mNext;
  }
}

float SlowMotionServo::minToMaxSpeed() const { return 10000.0 * mTimeFactorUp; }

float SlowMotionServo::maxToMinSpeed() const {
  return 10000.0 * mTimeFactorDown;
}

/*
 * Set the delay between the end of a movement and the time the
 * servo is stopped and detached if needed.
 */
void SlowMotionServo::setDelayUntilStop(unsigned int delayUntilStop) {
  sDelayUntilStop = delayUntilStop;
}

/*
 * Debugging purpose.
 */
#if WITH_DEBUG == 1

void SlowMotionServo::printList() {
  SlowMotionServo *currentObject = sServoList;
  Serial.print(F("[ "));
  while (currentObject != NULL) {
    Serial.print((unsigned long)currentObject, HEX);
    currentObject = currentObject->mNext;
    if (currentObject != NULL) {
      Serial.print(F(" > "));
    }
  }
  Serial.println(F(" ]"));
}

void SlowMotionServo::print() const {
  Serial.print('<');
  Serial.print((unsigned long)this, HEX);
  Serial.println(F("> {"));
  Serial.print(F("\tpin: "));
  if (mPin == 63)
    Serial.println(F("NONE"));
  else
    Serial.println(mPin);
  Serial.print(F("\tdetach at min: "));
  Serial.println(mDetachAtMin ? F("true") : F("false"));
  Serial.print(F("\tdetach at max: "));
  Serial.println(mDetachAtMax ? F("true") : F("false"));
  Serial.print(F("\tstate: "));
  printlnState(mState);
  Serial.print(F("\treverted: "));
  Serial.println(mReverted ? F("true") : F("false"));
  Serial.print(F("\tmin pulse: "));
  Serial.println(mMinPulse);
  Serial.print(F("\tmax pulse: "));
  Serial.println(mMaxPulse);
  Serial.print(F("\tstart time: "));
  Serial.println(mStartTime);
  Serial.print(F("\tinitial relative time: "));
  Serial.println(mInitialRelativeTime);
  Serial.print(F("\ttarget relative time: "));
  Serial.println(mTargetRelativeTime);
  Serial.print(F("\tcurrent relative time: "));
  Serial.println(mCurrentRelativeTime);
  Serial.print(F("\ttime factor up: "));
  Serial.println(mTimeFactorUp);
  Serial.print(F("\ttime factor down: "));
  Serial.println(mTimeFactorDown);
  Serial.println('}');
}

#endif

float SMSLinear::slopeUp(float time) { return slope(time); }

float SMSLinear::slopeDown(float time) { return slope(time); }

float SMSSmooth::slopeUp(float time) { return slope(time); }

float SMSSmooth::slopeDown(float time) { return slope(time); }

float SMSSmoothBounce::slopeUp(float time) {
  if (time <= 0.795) {
    return (1.0 - cos(time * PI)) / 1.8;
  } else {
    float timeOff = 10.0 * (time - 0.55);
    return (0.83 + 1.0 / (timeOff * timeOff));
  }
}

float SMSSmoothBounce::slopeDown(float time) {
  return (1.0 - cos(time * PI)) / 2.265;
}
