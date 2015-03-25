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
  byte mMinToMaxSpeed;
  byte mMaxToMinSpeed;
  
  static SlowMotionServo *sServoList;

  void updatePosition();
  int constrainPulse(int pulse);

public:
  SlowMotionServo();
  void setMinMax(int minPulse, int maxPulse);
  void setMin(int minPulse);
  void setMax(int maxPulse);
  void setMinToMaxSpeed(byte speed) { mMinToMaxSpeed = speed; }
  void setMaxToMinSpeed(byte speed) { mMaxToMinSpeed = speed; }
  void goTo(float position);
  void goToMin() { goTo(0.0); }
  void goToMax() { goTo(1.0); }
  
  virtual float slope(float time) = 0;
  
  static void update();
};

class SMSLinear : public SlowMotionServo
{
public:
  virtual float slope(float time);
};

#endif
