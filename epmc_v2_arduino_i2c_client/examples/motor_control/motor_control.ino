/*
 * Basic example code on how to control via I2C your geared DC motor with quadrature
 * encoder which is already connected to the Easy PID Motor Controller module and have already
 * succesfully set up their velocity PID control using the epmc_setup_application
 *
 * The code basically sends a low target velocity (in rad/s), waits for some time and then
 * sends a high target velocity (in rad/s). it also prints out the motors' angular positions
 * (in rad) and angular velocities (in rad/s).
 *
 * you can copy the code and use it in your project as you will.
 */

// Easy PID Motor Control i2c communication library
#include <epmcV2.h>

int i2c_address = 0x55; // set this address to the same address you have during setup via the GUI app
EPMC_V2 epmcv2(i2c_address);

///////// my sepcial delay function ///////////////
void delayMs(int ms)
{
  for (int i = 0; i < ms; i += 1)
  {
    delayMicroseconds(1000);
  }
}
//////////////////////////////////////////////////

bool isSuccessful;
float angPos0, angPos1; // (in rad)
float angVel0, angVel1; // (in rad/sec)

float lowTargetVel = -1.57;  // rad/sec
float highTargetVel = 1.57; // rad/sec
bool sendHigh = true;

long prevTime;
long sampleTime = 100; // millisec

long ctrlPrevTime;
long ctrlSampleTime = 5000; // millisec

void setup()
{
  // start i2c communication
  Wire.begin();

  // setup serial communication to print result on serial minitor
  Serial.begin(115200);

  delay(2000);

  epmcv2.writeSpeed(0, 0.00);
  epmcv2.writeSpeed(1, 0.00);

  int cmd_vel_timeout = 2000; // 0 to deactivate.
  epmcv2.setCmdTimeout(cmd_vel_timeout); // set motor command velocity timeout
  epmcv2.getCmdTimeout(cmd_vel_timeout); // get the stored command velocity timeout
  Serial.print("motor command vel timeout in ms: ");
  Serial.println(cmd_vel_timeout);

  epmcv2.writeSpeed(0, lowTargetVel);
  epmcv2.writeSpeed(1, lowTargetVel);

  sendHigh = true;

  prevTime = millis();
  ctrlPrevTime = millis();
}

void loop()
{
  if ((millis() - ctrlPrevTime) >= ctrlSampleTime)
  {
    if (sendHigh)
    {
      epmcv2.writeSpeed(0, highTargetVel);
      epmcv2.writeSpeed(1, highTargetVel);
      sendHigh = false;
    }
    else
    {
      epmcv2.writeSpeed(0, lowTargetVel);
      epmcv2.writeSpeed(1, lowTargetVel);
      sendHigh = true;
    }
    ctrlPrevTime = millis();
  }

  if ((millis() - prevTime) >= sampleTime)
  {
    /* CODE SHOULD GO IN HERE*/
    epmcv2.readPos(0, angPos0);
    epmcv2.readPos(1, angPos1);

    epmcv2.readVel(0, angVel0);
    epmcv2.readVel(1, angVel1);

    // Print results
    Serial.print("Motor 0: ");
    Serial.print(angPos0); Serial.print("\t");
    Serial.print(angVel0, 4); Serial.print("\t");

    Serial.print("Motor 1: ");
    Serial.print(angPos1); Serial.print("\t");
    Serial.println(angVel1, 4);
    Serial.println();

    prevTime = millis();
  }
}