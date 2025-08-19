#ifndef SIMPLE_PID_CONTROL_H
#define SIMPLE_PID_CONTROL_H
#include <Arduino.h>


class SimplePID {
  public:
    SimplePID(float Kp, float Ki, float Kd, float out_min, float out_max);

    void setParameters(float Kp, float Ki, float Kd, float out_min, float out_max);
    void setGains(float Kp, float Ki, float Kd);
    void setKp(float Kp);
    void setKi(float Ki);
    void setKd(float Kd);
    void setOutLimit(float out_max, float out_min);
    void begin();
    float compute(float target, float actual);
    void clearInegrator();

  private:
    float error, errorPrev, errorInt, errorDot;
    unsigned long lastTime = 0;
    float kp, ki, kd;
    float outMax, outMin, outSat, outUnsat;
    bool outputIsClamped = false;
    bool integratorIsOn = true;

    void reset();
};


SimplePID::SimplePID(float Kp, float Ki, float Kd, float out_min, float out_max)
{
  reset();

  kp = Kp;
  ki = Ki;
  kd = Kd;
  outMax = out_max;
  outMin = out_min;

  errorPrev = error;
  lastTime = micros();
}

void SimplePID::setParameters(float Kp, float Ki, float Kd, float out_min, float out_max)
{
  kp = Kp;
  ki = Ki;
  kd = Kd;
  outMax = out_max;
  outMin = out_min;
}

void SimplePID::setGains(float Kp, float Ki, float Kd)
{
  kp = Kp;
  ki = Ki;
  kd = Kd;
}

void SimplePID::setKp(float Kp)
{
  kp = Kp;
}

void SimplePID::setKi(float Ki)
{
  ki = Ki;
}

void SimplePID::setKd(float Kd)
{
  kd = Kd;
}

void SimplePID::setOutLimit(float out_max, float out_min)
{
  outMax = out_max;
  outMin = out_min;
}

void SimplePID::begin()
{
  reset();
}

float SimplePID::compute(float target, float actual)
{
  float sampleFreq = 1.0e6 / (float)(micros() - lastTime);

  error = target - actual;

  errorDot = (error - errorPrev) * sampleFreq;

  if (integratorIsOn)
  {
    errorInt += (error / sampleFreq);
  }
  else
  {
    errorInt += 0.0;
  }

  outUnsat = (kp * error) + (ki * errorInt) + (kd * errorDot);

  if (outUnsat > outMax)
  {
    outSat = outMax;
    outputIsClamped = true;
  }
  else if (outUnsat < outMin)
  {
    outSat = outMin;
    outputIsClamped = true;
  }
  else
  {
    outSat = outUnsat;
    outputIsClamped = false;
  }

  if (!outputIsClamped)
  {
    integratorIsOn = true;
  }
  else
  {
    integratorIsOn = false;
  }

  errorPrev = error;
  lastTime = micros();

  return outSat;
}

void SimplePID::reset()
{
  error = 0.0;
  errorPrev = 0.0;
  errorInt = 0.0;
  errorDot = 0.0;
  outSat = 0.0;
  outUnsat = 0.0;
}


#endif
