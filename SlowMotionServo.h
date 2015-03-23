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
public:
  SlowMotionServo(byte pin) : mPin(pin) {}
  void setMinToMaxSpeed(byte speed) { mMinToMaxSpeed = speed; }
  void setMaxToMinSpeed(byte speed) { mMaxToMinSpeed = speed; }
  void goTo(int position);
  void goToMin() { goTo(mMinPulse); }
  void goToMax() { goTo(mMaxPulse); }
  void updatePosition();
};