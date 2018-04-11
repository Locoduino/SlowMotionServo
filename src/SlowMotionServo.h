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

#ifndef __SlowMotionServo_h__
#define __SlowMotionServo_h__

#include "Arduino.h"
#include <Servo.h>

class SlowMotionServo : public Servo
{
private:
  byte mPin;                  // connection pin
  byte mState:3;              // state of the servo
  bool mDetachAtMin:1;
  bool mDetachAtMax:1;
  bool mReverted:1;
  unsigned int mMinPulse;     // minimum position of the servo in microseconds
  unsigned int mMaxPulse;     // maximum position of the servo in microseconds
  unsigned long mStartTime;   // time when the movement begin
  float mInitialRelativeTime; // starting position-time of the servo
  float mTargetRelativeTime;  // target position-time of the servo
  float mCurrentRelativeTime; // current position-time of the servo
  float mTimeFactorUp;        // time factor of a millis for movement from min to max
  float mTimeFactorDown;      // time factor of a millis for movement from max to min
  SlowMotionServo *mNext;     // next servo in the list

  static SlowMotionServo *sServoList; // head of the servo list
  static unsigned int sDelayUntilStop;

  void updatePosition();      // update the position of the servo
  void updatePulseAccordingToMinMax();
  unsigned int normalizePos(const unsigned int inPos);

public:
  SlowMotionServo();
  SlowMotionServo(byte pin);
  void setPin(const byte pin) { mPin = pin; }
  void setMinMax(unsigned int minPulse, unsigned int maxPulse);
  void setMin(unsigned int minPulse);
  void setMax(unsigned int maxPulse);
  void setReverted(const bool inReverted) { mReverted = inReverted; }
  void setMinToMaxSpeed(const float speed) { mTimeFactorUp = speed / 10000.0; }
  void setMaxToMinSpeed(const float speed) { mTimeFactorDown = speed / 10000.0; }
  void setSpeed(const float speed) { mTimeFactorUp = mTimeFactorDown = speed / 10000.0; }
  void setInitialPosition(float position);
  void setDetachAtMin(bool detach) { mDetachAtMin = detach; }
  void setDetachAtMax(bool detach) { mDetachAtMax = detach; }
  void setDetach(bool detach) { mDetachAtMin = mDetachAtMax = detach; }
  void goTo(float position);
  void goToMin() { goTo(0.0); }
  void goToMax() { goTo(1.0); }
  bool isStopped();

  byte pin()                  { return mPin; }
  bool detachAtMin()          { return mDetachAtMin; }
  bool detachAtMax()          { return mDetachAtMax; }
  unsigned int minimumPulse() { return mMinPulse; }
  unsigned int maximumPulse() { return mMaxPulse; }
  float minToMaxSpeed();
  float maxToMinPulse();
  bool isReverted()           { return mReverted; }

  virtual float slopeUp(float time) = 0;
  virtual float slopeDown(float time) = 0;

  static void update();
  static void setDelayUntilStop(unsigned int delayUntilStop);
};

class SMSLinear : public SlowMotionServo
{
public:
  virtual float slopeUp(float time);
  virtual float slopeDown(float time);
  float slope(float time) { return time; }
};

class SMSSmooth : public SlowMotionServo
{
public:
  virtual float slopeUp(float time);
  virtual float slopeDown(float time);
  float slope(float time) { return (1.0 - cos(time * PI))/2.0; }
};

class SMSSmoothBounce : public SlowMotionServo
{
public:
  virtual float slopeUp(float time);
  virtual float slopeDown(float time);
};

#endif
