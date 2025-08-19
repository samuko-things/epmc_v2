#ifndef ENCODER_SETUP_H
#define ENCODER_SETUP_H
#include <Arduino.h>
#include "driver/periph_ctrl.h"

// For critical sections on ESP32
static portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;


class QuadEncoder {
public:
  volatile long tickCount;
  volatile int8_t dir;
  int clkPin, dirPin;
  float pulsePerRev;
  unsigned long periodPerTick;
  unsigned long stopPeriodPerTick;
  volatile unsigned long oldTickTime;
  volatile float freqPerTick;

  QuadEncoder(int clk_pin, int dir_pin, float ppr);

  void setPulsePerRev(float ppr);
  void setZeroPeriodPerTick(float zeroVel);
  float getAngPos();
  float getAngVel();
  void resetAngVelToZero();

private:
  float prevTickCount;
  
};





QuadEncoder::QuadEncoder(int clk_pin, int dir_pin, float ppr)
{
  clkPin = clk_pin;
  dirPin = dir_pin;
  pulsePerRev = ppr;

  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dirPin, INPUT_PULLUP);

  tickCount = 0;
  prevTickCount = 0;
  dir = 1;
  setZeroPeriodPerTick(0.1);
  oldTickTime = micros();
}

void QuadEncoder::setPulsePerRev(float ppr)
{
  pulsePerRev = ppr;
}

float QuadEncoder::getAngPos()
{
  portENTER_CRITICAL(&encoderMux);
  long ticks = tickCount;
  portEXIT_CRITICAL(&encoderMux);
  return (2.00 * PI * (float)ticks) / pulsePerRev;
}

float QuadEncoder::getAngVel()
{
  float ang_vel;

  portENTER_CRITICAL(&encoderMux);
  float direction = (float)dir;
  unsigned long dt = periodPerTick;
  portEXIT_CRITICAL(&encoderMux);

  if (dt == 0) {
    return 0.0;
  }

  float frequency = 1e6 / ((float)dt * pulsePerRev);
  ang_vel = direction * 2.00 * PI * frequency;
  return ang_vel;
}

void QuadEncoder::resetAngVelToZero()
{
  unsigned long t = micros();
  
  portENTER_CRITICAL(&encoderMux);
  if ((t - oldTickTime) >= stopPeriodPerTick)
  {
    periodPerTick = 0;
  }
  portEXIT_CRITICAL(&encoderMux);
}

void QuadEncoder::setZeroPeriodPerTick(float zeroVel)
{
  stopPeriodPerTick = (unsigned long)((4.0 * PI * 1e6)/(pulsePerRev*zeroVel));
}


#endif