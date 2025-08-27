#include <Arduino.h>
#include "command_functions.h"
#include "serial_comm.h"
#include "i2c_comm.h"

//------------------------------------------------------------------------------//
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

void IRAM_ATTR readEncoder2()
{
  unsigned long currentTickTime = micros();

  if (digitalRead(encoder[2].clkPin) == digitalRead(encoder[2].dirPin))
  {
    encoder[2].tickCount -= 1;
    encoder[2].dir = -1;
  }
  else
  {
    encoder[2].tickCount += 1;
    encoder[2].dir = 1;
  }

  unsigned long period = currentTickTime - encoder[2].oldTickTime;
  if (period > 50 && period < 20000000)
  { // Ignore if > 20 sec or negative
    encoder[2].periodPerTick = period;
  }
  encoder[2].oldTickTime = currentTickTime;
}

void IRAM_ATTR readEncoder3()
{
  unsigned long currentTickTime = micros();

  if (digitalRead(encoder[3].clkPin) == digitalRead(encoder[3].dirPin))
  {
    encoder[3].tickCount -= 1;
    encoder[3].dir = -1;
  }
  else
  {
    encoder[3].tickCount += 1;
    encoder[3].dir = 1;
  }

  unsigned long period = currentTickTime - encoder[3].oldTickTime;
  if (period > 50 && period < 20000000)
  { // Ignore if > 20 sec or negative
    encoder[3].periodPerTick = period;
  }
  encoder[3].oldTickTime = currentTickTime;
}
//----------------------------------------------------------------------------------------------//

void encoderInit()
{
  for (int i = 0; i < num_of_motors; i += 1)
  {
    encoder[i].setPulsePerRev(enc_ppr[i]);
  }

  attachInterrupt(digitalPinToInterrupt(encoder[0].clkPin), readEncoder0, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder[1].clkPin), readEncoder1, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder[2].clkPin), readEncoder2, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder[3].clkPin), readEncoder3, RISING);
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
unsigned long serialLoopTime, serialLoopTimeInterval = 5;
unsigned long pidTime, pidTimeInterval = 5;
unsigned long pidStopTime[num_of_motors], pidStopTimeInterval = 500;
unsigned long readImuTime, readImuSampleTime = 20;        // ms -> (1000/sampleTime) hz
//---------------------------------------------------------------------------------------------


void setup()
{
  loadStoredParams();

  Serial.begin(115200);
  Serial.setTimeout(2);

  if (useIMU){
    Wire.begin();
  }
  else {
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    Wire.begin(i2cAddress);
  }

  pinMode(LED_BUILTIN, OUTPUT);

  analogWriteResolution(8); // 8 Bit resolution
  analogWriteFrequency(1000); // 1kHz

  encoderInit();
  velFilterInit();
  pidInit();

  if (useIMU)
    imu.begin();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize timing markers
  unsigned long now = millis();
  serialLoopTime = now;
  pidTime = now;
  for (int i = 0; i < num_of_motors; i += 1)
  {
    pidStopTime[i] = now;
    cmdVelTimeout[i] = now;
    isMotorCommanded[i] = 0;
  }
  readImuTime = now;
}

void loop()
{
  // Serial comm loop
  if ((millis() - serialLoopTime) >= serialLoopTimeInterval)
  {
    recieve_and_send_data();
    serialLoopTime = millis();
  }

  // Sensor update
  for (int i = 0; i < num_of_motors; i += 1)
  {
    encoder[i].resetAngVelToZero();
    unfilteredVel[i] = encoder[i].getAngVel();
    filteredVel[i] = velFilter[i].filter(unfilteredVel[i]);
  }

  // PID control loop
  if ((millis() - pidTime) >= pidTimeInterval)
  {
    for (int i = 0; i < num_of_motors; i += 1)
    {
      if (pidMode[i])
      {
        output[i] = pidMotor[i].compute(target[i], filteredVel[i]);
        motor[i].sendPWM((int)output[i]);
      }
    }
    pidTime = millis();
  }

  // check to see if motor has stopped
  for (int i = 0; i < num_of_motors; i += 1)
  {
    int target_int = (int)fabs(target[i]) * 1000;
    if (target_int < 10 && pidMode[i])
    {
      if ((millis() - pidStopTime[i]) >= pidStopTimeInterval)
      {
        pidMotor[i].begin();
        isMotorCommanded[i] = 0;
        pidMode[i] = 0;
        motor[i].sendPWM(0);
        pidStopTime[i] = millis();
      }
    }
    else
    {
      pidStopTime[i] = millis();
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
        cmdVelTimeout[i] = millis();
      }
      if (isMotorCommanded[i] && ((millis() - cmdVelTimeout[i]) >= cmdVelTimeoutInterval))
      {
        if(pidMode[i]){
          target[i] = 0.000;
        }
        else {
          motor[i].sendPWM(0);
        }
        isMotorCommanded[i] = 0;
      }
    }
  }

  if ((millis() - readImuTime) >= readImuSampleTime)
  {
    if(useIMU){
      //-----READ ACC DATA (m/s^2) AND CALIBRATE-------------//
      accRaw[0] = imu.readAccX_mps2(); // m/s²
      accRaw[1] = imu.readAccY_mps2(); // m/s²
      accRaw[2] = imu.readAccZ_mps2(); // m/s²

      accCal[0] = accRaw[0] - accOff[0];
      accCal[1] = accRaw[1] - accOff[1];
      accCal[2] = accRaw[2] - accOff[2];
      //------------------------------------------------------//

      //-----READ GYRO DATA (rad/s) AND CALIBRATE------------//
      gyroRaw[0] = imu.readGyroX_rps(); // rad/s
      gyroRaw[1] = imu.readGyroY_rps(); // rad/s
      gyroRaw[2] = imu.readGyroZ_rps(); // rad/s

      gyroCal[0] = gyroRaw[0] - gyroOff[0];
      gyroCal[1] = gyroRaw[1] - gyroOff[1];
      gyroCal[2] = gyroRaw[2] - gyroOff[2];
      //-----------------------------------------------------//

      readImuTime = millis(); 
    }
  }
}