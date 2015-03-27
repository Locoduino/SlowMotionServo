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
  byte mState;                // state of the servo
  int mMinPulse;              // minimum position of the servo in microseconds
  int mMaxPulse;              // maximum position of the servo in microseconds
  unsigned long mStartTime;   // time when the movement begin
  float mInitialRelativeTime; // starting position-time of the servo
  float mTargetRelativeTime;  // target position-time of the servo
  float mCurrentRelativeTime; // current position-time of the servo
  float mTimeFactorUp;        // time factor of a millis for movement from min to max
  float mTimeFactorDown;      // time factor of a millis for movement from max to min
  SlowMotionServo *mNext;     // next servo in the list

  static SlowMotionServo *sServoList; // head of the servo list

  void updatePosition();      // update the position of the servo

public:
  SlowMotionServo();
  void setMinMax(int minPulse, int maxPulse);
  void setMin(int minPulse);
  void setMax(int maxPulse);
  void setMinToMaxSpeed(float speed) { mTimeFactorUp = speed / 10000.0; }
  void setMaxToMinSpeed(float speed) { mTimeFactorDown = speed / 10000.0; }
  void setSpeed(float speed) { mTimeFactorUp = mTimeFactorDown = speed / 10000.0; }
  void setInitialPosition(float position);
  void goTo(float position);
  void goToMin() { goTo(0.0); }
  void goToMax() { goTo(1.0); }
  bool isStopped();

  virtual float slopeUp(float time) = 0;
  virtual float slopeDown(float time) = 0;

  static void update();
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
