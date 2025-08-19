#ifndef COMMAND_FUNCTIONS_H
#define COMMAND_FUNCTIONS_H

#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include "l298n_motor_control.h"
#include "encoder_setup.h"
#include "adaptive_low_pass_filter.h"
#include "simple_pid_control.h"
// #include "parameter_store.h"

//--------------- global variables -----------------//
const int num_of_motors = 2;

// motor 0 H-Bridge Connection
int IN1 = 26, IN2 = 27, enA = 25;
// motor 1 H-Bridge Connection
int IN3 = 14, IN4 = 12, enB = 13;

L298NMotorControl motor[num_of_motors] = {
  L298NMotorControl(IN1, IN2, enA), // motor 0
  L298NMotorControl(IN3, IN4, enB) // motor 1
};

float enc_ppr[num_of_motors]={
  1000.0, // motor 0 encoder pulse per revolution parameter
  1000.0 // motor 1 encoder pulse per revolution parameter
};

// motor 0 encoder connection
int enc1_clkPin = 18, enc1_dirPin = 19;
// motor 1 encoder connection
int enc2_clkPin = 16, enc2_dirPin = 17;

QuadEncoder encoder[num_of_motors] = {
  QuadEncoder(enc1_clkPin, enc1_dirPin, enc_ppr[0]), // motor 0 encoder connection
  QuadEncoder(enc2_clkPin, enc2_dirPin, enc_ppr[1]) // motor 1 encoder connection
};

// adaptive lowpass Filter
const int filterOrder = 1;
float cutOffFreq[num_of_motors] = {
  1.0, // motor 0 velocity filter cutoff frequency
  1.0 // motor 1 velocity filter cutoff frequency
};

AdaptiveLowPassFilter velFilter[num_of_motors] = {
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[0]), // motor 0 velocity filter
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[1]) // motor 1 velocity filter
};

float filteredVel[num_of_motors] = {
  0.0,
  0.0
};

float unfilteredVel[num_of_motors] = {
  0.0,
  0.0
};

// motor PID parameters
float outMin = -255.0, outMax = 255.0;

float kp[num_of_motors] = {
  0.0,
  0.0
};

float ki[num_of_motors] = {
  0.0,
  0.0
};

float kd[num_of_motors] = {
  0.0,
  0.0
};

float target[num_of_motors] = {
  0.0,
  0.0
};

float output[num_of_motors] = {
  0.0,
  0.0
};

SimplePID pidMotor[num_of_motors] = {
  SimplePID(kp[0], ki[0], kd[0], outMin, outMax),
  SimplePID(kp[1], ki[1], kd[1], outMin, outMax),
};


// check if in PID or PWM mode
int pidMode[num_of_motors] = {
  1,
  1
}; // 1-PID MODE, 0-SETUP/PWM MODE

int isMotorCommanded[num_of_motors] = {
  0,
  0
};

float rdir[num_of_motors] = {
  1.0,
  1.0
};

// // maximum motor velocity that can be commanded
float maxVel[num_of_motors] = {
  10.0,
  10.0
};

// for command timeout.
unsigned long cmdVelTimeoutInterval = 0; // us -> (1000000/sampleTime) hz
unsigned long cmdVelTimeout[num_of_motors];

// initial i2cAddress
uint8_t i2cAddress = 0x55;

// for stored initialization and reset
bool firstLoad = false;
//-------------------------------------------------//



//--------------- storage variables -----------------//
Preferences storage;

const char * ppr_key[4] = {
  "ppr0",
  "ppr1",
  "ppr2",
  "ppr3"
};

const char * cf_key[4] = {
  "cf0",
  "cf1",
  "cf2",
  "cf3"
};

const char * kp_key[4] = {
  "kp0",
  "kp1",
  "kp2",
  "kp3"
};

const char * ki_key[4] = {
  "ki0",
  "ki1",
  "ki2",
  "ki3"
};

const char * kd_key[4] = {
  "kd0",
  "kd1",
  "kd2",
  "kd3"
};

const char * rdir_key[4] = {
  "rdir0",
  "rdir1",
  "rdir2",
  "rdir3"
};

const char * maxVel_key[4] = {
  "maxVel0",
  "maxVel1",
  "maxVel2",
  "maxVel3"
};

const char * i2cAddress_key = "i2cAddress";

const char * firstLoad_key = "firstLoad";

const char * params_ns = "params"; // preference namespace

void resetParamsInStorage(){
  storage.begin(params_ns, false);

  for (int i=0; i<num_of_motors; i+=1){
    storage.putFloat(ppr_key[i], 1000.0);
    storage.putFloat(kp_key[i], 0.0);
    storage.putFloat(ki_key[i], 0.0);
    storage.putFloat(kd_key[i], 0.0);
    storage.putFloat(cf_key[i], 1.0);
    storage.putFloat(rdir_key[i], 1.0);
    storage.putFloat(maxVel_key[i], 10.0);
  }
  storage.putUChar(i2cAddress_key, 0x55);

  storage.end();
}

void initParams(){
  //check for firstLoad
  storage.begin(params_ns, true);
  firstLoad = storage.getBool(firstLoad_key);
  storage.end();
  // if firsLoad -> reset all params and set firstLoad to false
  if(firstLoad == true){
    resetParamsInStorage();
    firstLoad = false;
    storage.begin(params_ns, false);
    storage.putBool(firstLoad_key, firstLoad);
    storage.end();
  }

}

void loadStoredParams(){
  initParams();
  // load each parameter form the storage to the local variables
  storage.begin(params_ns, true);

  for (int i=0; i<num_of_motors; i+=1){
    enc_ppr[i] = storage.getFloat(ppr_key[i], 1000.0);
    kp[i] = storage.getFloat(kp_key[i], 0.0);
    ki[i] = storage.getFloat(ki_key[i], 0.0);
    kd[i] = storage.getFloat(kd_key[i], 0.0);
    cutOffFreq[i] = storage.getFloat(cf_key[i], 1.0);
    rdir[i] = storage.getFloat(rdir_key[i], 1.0);
    maxVel[i] = storage.getFloat(maxVel_key[i], 10.0);
  }
  i2cAddress = storage.getUChar(i2cAddress_key, 0x55);

  storage.end();
}


//-------------------------------------------------//




//--------------- global functions ----------------//

String readPos(int motor_no){
  float posData = encoder[motor_no].getAngPos();
  String data = String(rdir[motor_no] * posData, 3);
  return data;
}

String readVel(int motor_no){
  String data = String(rdir[motor_no] * filteredVel[motor_no], 4);
  data += ",";
  data += String(rdir[motor_no] * unfilteredVel[motor_no], 4);
  return data;
}


String readPidVel(int motor_no)
{
  String data = String(rdir[motor_no] * target[motor_no], 4);
  data += ",";
  data += String(rdir[motor_no] * filteredVel[motor_no], 4);
  return data;
}


String writePWM(int motor_no, int pwm)
{
  pidMode[motor_no] = 0;

  int p;
  if (pwm>255)
    p = 255;
  else if (pwm<-255)
    p = -255;
  else
    p = pwm;
  isMotorCommanded[motor_no] = 1;
  if (p == 0) isMotorCommanded[motor_no] = 0;
  motor[motor_no].sendPWM((int)rdir[motor_no] * p);
  
  return "1";
}


String writeSpeed(int motor_no, float targetVel)
{
  pidMode[motor_no] = 1;

  float vel;
  if (targetVel > maxVel[motor_no]){
    vel = maxVel[motor_no];
  }
  else if (targetVel < (-1.00 * maxVel[motor_no])){
    vel = -1.00 * maxVel[motor_no];
  }
  else {
    vel = targetVel;
  }
    
  target[motor_no] = rdir[motor_no] * vel;
  isMotorCommanded[motor_no] = 1;

  return "1";
}


String setPidModeFunc(int motor_no, int mode)
{
  pidMode[motor_no] = mode;
  motor[motor_no].sendPWM(0);
  pidMotor[motor_no].begin();

  return "1";
}
String getPidModeFunc(int motor_no)
{
  return String(pidMode[motor_no]);
}


String setEncoderPPR(int motor_no, float ppr)
{
  enc_ppr[motor_no] = ppr;
  storage.begin(params_ns, false);
  storage.putFloat(ppr_key[motor_no], enc_ppr[motor_no]);
  storage.end();
  encoder[motor_no].setPulsePerRev(enc_ppr[motor_no]);
  return "1";
}
String getEncoderPPR(int motor_no)
{
  return String(enc_ppr[motor_no]);
}


String setMotorKp(int motor_no, float Kp)
{
  kp[motor_no] = Kp;
  storage.begin(params_ns, false);
  storage.putFloat(kp_key[motor_no], kp[motor_no]);
  storage.end();
  pidMotor[motor_no].setKp(kp[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKp(int motor_no)
{
  return String(kp[motor_no], 4);
}


String setMotorKi(int motor_no, float Ki)
{
  ki[motor_no] = Ki;
  storage.begin(params_ns, false);
  storage.putFloat(ki_key[motor_no], ki[motor_no]);
  storage.end();
  pidMotor[motor_no].setKi(ki[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKi(int motor_no)
{
  return String(ki[motor_no], 4);
}


String setMotorKd(int motor_no, float Kd)
{
  kd[motor_no] = Kd;
  storage.begin(params_ns, false);
  storage.putFloat(kd_key[motor_no], kd[motor_no]);
  storage.end();
  pidMotor[motor_no].setKd(kd[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKd(int motor_no)
{
  return String(kd[motor_no], 4);
}


String setRdir(int motor_no, float dir)
{
  if (dir >= 0.0)
    rdir[motor_no] = 1.0;
  else
    rdir[motor_no] = -1.0;
  storage.begin(params_ns, false);
  storage.putFloat(rdir_key[motor_no], rdir[motor_no]);
  storage.end();
  return "1";
}
String getRdir(int motor_no)
{
  return String(rdir[motor_no], 1);
}


String setMaxVel(int motor_no, float max_vel)
{
  maxVel[motor_no] = fabs(max_vel);
  storage.begin(params_ns, false);
  storage.putFloat(maxVel_key[motor_no], maxVel[motor_no]);
  storage.end();
  return "1";
}
String getMaxVel(int motor_no)
{
  return String(maxVel[motor_no], 2);
}


String setCutoffFreq(int motor_no, float f0)
{
  cutOffFreq[motor_no] = f0;
  storage.begin(params_ns, false);
  storage.putFloat(cf_key[motor_no], cutOffFreq[motor_no]);
  storage.end();
  velFilter[motor_no].setCutOffFreq(cutOffFreq[motor_no]);
  return "1";
}
String getCutoffFreq(int motor_no)
{
  return String(cutOffFreq[motor_no]);
}


String setCmdTimeout(int timeout_ms)
{
  unsigned long cmdTimeout = timeout_ms;
  if (cmdTimeout < 1000)
  {
    cmdVelTimeoutInterval = 0;
  }
  else
  {
    cmdVelTimeoutInterval = cmdTimeout*1000;
  }
  return "1";
}
String getCmdTimeout()
{
  return String(cmdVelTimeoutInterval/1000);
}


String setI2cAddress(int address)
{
  if((address <= 0) || (address > 255)){
    return "0";
  }
  else {
    i2cAddress = (uint8_t)address;
    storage.begin(params_ns, false);
    storage.putUChar(i2cAddress_key, i2cAddress);
    storage.end();
    Wire.begin(i2cAddress);
    return "1";
  }
}
String getI2cAddress()
{
  return String(i2cAddress);
}

String triggerResetParams()
{
  storage.begin(params_ns, false);
  firstLoad = true;
  storage.putBool(firstLoad_key, firstLoad);
  storage.end();
  // reload to reset
  loadStoredParams();
  return "1";
}


#endif