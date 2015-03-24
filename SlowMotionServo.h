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
  byte mPin;
  byte mState;
  int mMinPulse;
  int mMaxPulse;
  int currentPulse;
  byte mMinToMaxSpeed;
  byte mMaxToMinSpeed;
  
  static SlowMotionServo *sServoList;

  void updatePosition();

public:
  SlowMotionServo() : mPin(-1) {}
  void setup() { if (mPin != -1) attach(mPin); }
  void setup(byte pin) { mPin = pin; setup(); }
  void setMinMax(int minPulse, int maxPulse);
  void setMin(int minPulse);
  void setMax(int maxPulse);
  void setMinToMaxSpeed(byte speed) { mMinToMaxSpeed = speed; }
  void setMaxToMinSpeed(byte speed) { mMaxToMinSpeed = speed; }
  void goTo(int position);
  void goTo(float position);
  void goToMin() { goTo(mMinPulse); }
  void goToMax() { goTo(mMaxPulse); }
  
  virtual float slope(float time) = 0;
  
  static void update();
};

class SMSLinear : public SlowMotionServo
{
public:
  virtual float slope(float time);
};

#endif
