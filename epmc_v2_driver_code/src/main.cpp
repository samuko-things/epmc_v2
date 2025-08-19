#include <Arduino.h>
#include "command_functions.h"
#include "serial_comm.h"
#include "i2c_comm.h"

void IRAM_ATTR readEncoder0()
{
  unsigned long currentTickTime = micros();

  if (digitalRead(encoder[0].clkPin) == digitalRead(encoder[0].dirPin))
  {
    encoder[0].tickCount -= 1;
    encoder[0].dir = -1;
  }
  else
  {
    encoder[0].tickCount += 1;
    encoder[0].dir = 1;
  }

  unsigned long period = currentTickTime - encoder[0].oldTickTime;
  if (period > 50 && period < 20000000)
  { // Ignore if > 20 sec or negative
    encoder[0].periodPerTick = period;
  }
  encoder[0].oldTickTime = currentTickTime;
}

void IRAM_ATTR readEncoder1()
{
  unsigned long currentTickTime = micros();

  if (digitalRead(encoder[1].clkPin) == digitalRead(encoder[1].dirPin))
  {
    encoder[1].tickCount -= 1;
    encoder[1].dir = -1;
  }
  else
  {
    encoder[1].tickCount += 1;
    encoder[1].dir = 1;
  }

  unsigned long period = currentTickTime - encoder[1].oldTickTime;
  if (period > 50 && period < 20000000)
  { // Ignore if > 20 sec or negative
    encoder[1].periodPerTick = period;
  }
  encoder[1].oldTickTime = currentTickTime;
}

void encoderInit()
{
  for (int i = 0; i < num_of_motors; i += 1)
  {
    encoder[i].setPulsePerRev(enc_ppr[i]);
  }

  attachInterrupt(digitalPinToInterrupt(encoder[0].clkPin), readEncoder0, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder[1].clkPin), readEncoder1, RISING);
}

void velFilterInit()
{
  for (int i = 0; i < num_of_motors; i += 1)
  {
    velFilter[i].setCutOffFreq(cutOffFreq[i]);
  }
}

void pidInit()
{
  for (int i = 0; i < num_of_motors; i += 1)
  {
    pidMotor[i].setParameters(kp[i], ki[i], kd[i], outMin, outMax);
    pidMotor[i].begin();
  }
}

//---------------------------------------------------------------------------------------------
// Timing variables in microseconds
// please do not adjust any of the values as it can affect important operations
// unsigned long sensorUpdateTime, sensorUpdateTimeInterval = 500;
unsigned long serialLoopTime, serialLoopTimeInterval = 5000;
unsigned long pidTime, pidTimeInterval = 5000;
unsigned long pidStopTime[2], pidStopTimeInterval = 1000000;
//---------------------------------------------------------------------------------------------

void setup()
{
  loadStoredParams();

  Serial.begin(115200);
  Serial.setTimeout(2);

  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin(i2cAddress);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  encoderInit();
  velFilterInit();
  pidInit();

  // Initialize timing markers
  unsigned long now_us = micros();
  // sensorUpdateTime = now_us;
  serialLoopTime = now_us;
  pidTime = now_us;
  for (int i = 0; i < num_of_motors; i += 1)
  {
    pidStopTime[i] = now_us;
    cmdVelTimeout[i] = millis();
    isMotorCommanded[i] = 0;
  }
}

void loop()
{
  unsigned long now_us = micros();

  // Serial comm loop
  if ((now_us - serialLoopTime) >= serialLoopTimeInterval)
  {
    recieve_and_send_data();
    serialLoopTime = now_us;
  }

  // Sensor update loop
  // if ((now_us - sensorUpdateTime) >= sensorUpdateTimeInterval)
  // {
    for (int i = 0; i < num_of_motors; i += 1)
    {
      encoder[i].resetAngVelToZero();
      unfilteredVel[i] = encoder[i].getAngVel();
      filteredVel[i] = velFilter[i].filter(unfilteredVel[i]);
    }
  //   sensorUpdateTime = now_us;
  // }

  // PID control loop
  if ((now_us - pidTime) >= pidTimeInterval)
  {
    for (int i = 0; i < num_of_motors; i += 1)
    {
      if (pidMode[i])
      {
        output[i] = pidMotor[i].compute(target[i], filteredVel[i]);
        motor[i].sendPWM((int)output[i]);
      }
    }
    pidTime = now_us;
  }

  // check to see if motor has stopped
  for (int i = 0; i < num_of_motors; i += 1)
  {
    int target_int = (int)fabs(target[i]) * 1000;
    if (target_int < 10 && pidMode[i])
    {
      if ((now_us - pidStopTime[i]) >= pidStopTimeInterval)
      {
        pidMotor[i].begin();
        isMotorCommanded[i] = 0;
        pidStopTime[i] = now_us;
      }
    }
    else
    {
      pidStopTime[i] = now_us;
    }
  }

  // command timeout
  int cmdTimeout = (int)cmdVelTimeoutInterval;
  if (cmdVelTimeoutInterval > 0)
  {
    for (int i = 0; i < num_of_motors; i += 1)
    {
      if (!isMotorCommanded[i])
      {
        cmdVelTimeout[i] = now_us;
      }
      if (isMotorCommanded[i] && ((now_us - cmdVelTimeout[i]) >= cmdVelTimeoutInterval))
      {
        target[i] = 0.00;
        output[i] = 0.00;
        if (!pidMode[i])
          motor[i].sendPWM(0);
          pidMode[i] = 1;
        isMotorCommanded[i] = 0;
      }
    }
  }
}