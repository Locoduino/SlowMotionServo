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

#define WITH_DEBUG 1

class SlowMotionServo : public Servo
{
private:
  byte mPin:6;                // connection pin 0..63
  bool mDetachAtMin:1;
  bool mDetachAtMax:1;
  byte mState:3;              // state of the servo
  byte mSavedState:3;         // saved state of the servo for setup
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
  bool isSettable() const;

public:
  SlowMotionServo();
  SlowMotionServo(byte pin);
  virtual ~SlowMotionServo();
  void setPin(const byte pin) { mPin = pin; }
  void setMinMax(unsigned int minPulse, unsigned int maxPulse);
  void setMin(unsigned int minPulse);
  void setMax(unsigned int maxPulse);
  unsigned int minPosition() const { return mMinPulse; }
  unsigned int maxPosition() const { return mMaxPulse; }
  void setReverted(const bool inReverted);
  void setMinToMaxSpeed(const float speed);
  void setMaxToMinSpeed(const float speed);
  void setSpeed(const float speed);
  void setInitialPosition(float position);
  void setDetachAtMin(bool detach) { mDetachAtMin = detach; }
  void setDetachAtMax(bool detach) { mDetachAtMax = detach; }
  void setDetach(bool detach) { mDetachAtMin = mDetachAtMax = detach; }
  void goTo(float position);
  void goToMin() { goTo(0.0); }
  void goToMax() { goTo(1.0); }
  bool isStopped() const;

  /*
   * Live configuration functions.
   */
  void setupMin(uint16_t minPulse);
  void setupMax(uint16_t maxPulse);
  void adjustMin(int16_t increment) { setupMin(mMinPulse + increment); }
  void adjustMax(int16_t increment) { setupMax(mMaxPulse + increment); }
  void endSetup();

  byte pin() const                  { return mPin; }
  bool detachAtMin() const          { return mDetachAtMin; }
  bool detachAtMax() const          { return mDetachAtMax; }
  unsigned int minimumPulse() const { return mMinPulse; }
  unsigned int maximumPulse() const { return mMaxPulse; }
  float minToMaxSpeed() const;
  float maxToMinSpeed() const;
  bool isReverted() const           { return mReverted; }

  virtual float slopeUp(float time) = 0;
  virtual float slopeDown(float time) = 0;

  static void update();
  static void setDelayUntilStop(unsigned int delayUntilStop);

#if WITH_DEBUG == 1
  static void printList();
  void print() const;
#endif
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
