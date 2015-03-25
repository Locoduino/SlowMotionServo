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

SlowMotionServo *SlowMotionServo::sServoList = NULL;

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
  mTargetRelativeTime = position * 1000;
  mTimeSpan = mTargetRelativeTime - mInitialRelativeTime;
  mStartTime = millis();
}

void SlowMotionServo::updatePosition()
{
  float relativeTime = (millis() - mStartTime) + mInitialRelativeTime;
  int position = time *  
}
