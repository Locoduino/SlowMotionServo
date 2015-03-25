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
  byte mState;
  int mMinPulse;
  int mMaxPulse;
  unsigned long mStartTime;
  float mInitialRelativeTime;
  float mTargetRelativeTime;
  float mCurrentRelativeTime;
  float mTimeFactorUp;
  float mTimeFactorDown;
  SlowMotionServo *mNext;

  static SlowMotionServo *sServoList;

  void updatePosition();
  int constrainPulse(int pulse);

public:
  SlowMotionServo();
  void setMinMax(int minPulse, int maxPulse);
  void setMin(int minPulse);
  void setMax(int maxPulse);
  void setMinToMaxSpeed(float speed) { mTimeFactorUp = speed; }
  void setMaxToMinSpeed(float speed) { mTimeFactorDown = speed; }
  void goTo(float position);
  void goToMin() { goTo(0.0); }
  void goToMax() { goTo(1.0); }
  bool isStopped();

  virtual float slope(float time) = 0;

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

#endif
