#ifndef COMMAND_FUNCTIONS_H
#define COMMAND_FUNCTIONS_H

#include <Arduino.h>
#include <Preferences.h>
#include "l298n_motor_control.h"
#include "encoder_setup.h"
#include "adaptive_low_pass_filter.h"
#include "simple_pid_control.h"
#include "mpu6050.h"
#include "madgwick_filter.h"

//--------------- global variables -----------------//
const int num_of_motors = 4;

// motor 0 H-Bridge Connection
int IN1_0 = 5, IN2_0 = 17, EN_0 = 16;
// motor 1 H-Bridge Connection
int IN1_1 = 19, IN2_1 = 18, EN_1 = 23;
// motor 2 H-Bridge Connection
int IN1_2 = 26, IN2_2 = 27, EN_2 = 12;
// motor 3 H-Bridge Connection
int IN1_3 = 33, IN2_3 = 25, EN_3 = 32;

L298NMotorControl motor[num_of_motors] = {
  L298NMotorControl(IN1_0, IN2_0, EN_0), // motor 0
  L298NMotorControl(IN1_1, IN2_1, EN_1), // motor 1
  L298NMotorControl(IN1_2, IN2_2, EN_2), // motor 2
  L298NMotorControl(IN1_3, IN2_3, EN_3) // motor 3
};

double enc_ppr[num_of_motors]={
  1000.0, // motor 0 encoder pulse per revolution parameter
  1000.0, // motor 1 encoder pulse per revolution parameter
  1000.0, // motor 2 encoder pulse per revolution parameter
  1000.0 // motor 3 encoder pulse per revolution parameter
};

// motor 0 encoder connection
int enc0_clkPin = 15, enc0_dirPin = 4;
// motor 1 encoder connection
int enc1_clkPin = 35, enc1_dirPin = 34;
// motor 2 encoder connection
int enc2_clkPin = 13, enc2_dirPin = 14;
// motor 3 encoder connection
int enc3_clkPin = 39, enc3_dirPin = 36;

QuadEncoder encoder[num_of_motors] = {
  QuadEncoder(enc0_clkPin, enc0_dirPin, enc_ppr[0]), // motor 0 encoder connection
  QuadEncoder(enc1_clkPin, enc1_dirPin, enc_ppr[1]), // motor 1 encoder connection
  QuadEncoder(enc2_clkPin, enc2_dirPin, enc_ppr[2]), // motor 2 encoder connection
  QuadEncoder(enc3_clkPin, enc3_dirPin, enc_ppr[3]) // motor 3 encoder connection
};

// adaptive lowpass Filter
const int filterOrder = 1;
double cutOffFreq[num_of_motors] = {
  1.5, // motor 0 velocity filter cutoff frequency
  1.5, // motor 1 velocity filter cutoff frequency
  1.5, // motor 2 velocity filter cutoff frequency
  1.5 // motor 3 velocity filter cutoff frequency
};

AdaptiveLowPassFilter velFilter[num_of_motors] = {
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[0]), // motor 0 velocity filter
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[1]), // motor 1 velocity filter
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[2]), // motor 0 velocity filter
  AdaptiveLowPassFilter(filterOrder, cutOffFreq[3]) // motor 1 velocity filter
};

double filteredVel[num_of_motors] = {
  0.0,
  0.0,
  0.0,
  0.0
};

double unfilteredVel[num_of_motors] = {
  0.0,
  0.0,
  0.0,
  0.0
};

// motor PID parameters
double outMin = -255.0, outMax = 255.0;

double kp[num_of_motors] = {
  0.0,
  0.0,
  0.0,
  0.0
};

double ki[num_of_motors] = {
  0.0,
  0.0,
  0.0,
  0.0
};

double kd[num_of_motors] = {
  0.0,
  0.0,
  0.0,
  0.0
};

double target[num_of_motors] = {
  0.0,
  0.0,
  0.0,
  0.0
};

double output[num_of_motors] = {
  0.0,
  0.0,
  0.0,
  0.0
};

SimplePID pidMotor[num_of_motors] = {
  SimplePID(kp[0], ki[0], kd[0], outMin, outMax),
  SimplePID(kp[1], ki[1], kd[1], outMin, outMax),
  SimplePID(kp[2], ki[2], kd[2], outMin, outMax),
  SimplePID(kp[3], ki[3], kd[3], outMin, outMax)
};


// check if in PID or PWM mode
int pidMode[num_of_motors] = {
  0,
  0,
  0,
  0
}; // 1-PID MODE, 0-SETUP/PWM MODE

int isMotorCommanded[num_of_motors] = {
  0,
  0,
  0,
  0
};

int rdir[num_of_motors] = {
  1,
  1,
  1,
  1
};

// // maximum motor velocity that can be commanded
double maxVel[num_of_motors] = {
  10.0,
  10.0,
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


//-------------- IMU MPU6050 ---------------------//
float accOff[3] = {
  0.0,
  0.0,
  0.0
};

float accVar[3] = {
  0.0,
  0.0,
  0.0
};

float accRaw[3] = {
  0.0,
  0.0,
  0.0
};

float accCal[3] = {
  0.0,
  0.0,
  0.0
};

float gyroOff[3] = {
  0.0,
  0.0,
  0.0
};

float gyroVar[3] = {
  0.0,
  0.0,
  0.0
};

float gyroRaw[3] = {
  0.0,
  0.0,
  0.0
};

float gyroCal[3] = {
  0.0,
  0.0,
  0.0
};

float rpy[3] = {
  0.0,
  0.0,
  0.0
};

float rpyVar[3] = {
  0.0,
  0.0,
  0.0
};

float quat[4] = {
  0.0,
  0.0,
  0.0,
  0.0
};

float IMU_filterGain = 0.1;
float IMU_gyroBiasGain = 0.0;

int use_imu = 1;

MPU6050 imu;
MadgwickFilter madgwickFilter;
//------------------------------------------------//



//--------------- storage variables -----------------//
Preferences storage;

const char * ppr_key[num_of_motors] = {
  "ppr0",
  "ppr1",
  "ppr2",
  "ppr3"
};

const char * cf_key[num_of_motors] = {
  "cf0",
  "cf1",
  "cf2",
  "cf3"
};

const char * kp_key[num_of_motors] = {
  "kp0",
  "kp1",
  "kp2",
  "kp3"
};

const char * ki_key[num_of_motors] = {
  "ki0",
  "ki1",
  "ki2",
  "ki3"
};

const char * kd_key[num_of_motors] = {
  "kd0",
  "kd1",
  "kd2",
  "kd3"
};

const char * rdir_key[num_of_motors] = {
  "rdir0",
  "rdir1",
  "rdir2",
  "rdir3"
};

const char * maxVel_key[num_of_motors] = {
  "maxVel0",
  "maxVel1",
  "maxVel2",
  "maxVel3"
};

const char * accOff_key[3] = {
  "accOff0",
  "accOff1",
  "accOff2",
};

const char * accVar_key[3] = {
  "accVar0",
  "accVar1",
  "accVar2",
};

const char * gyroOff_key[3] = {
  "gyroOff0",
  "gyroOff1",
  "gyroOff2",
};

const char * gyroVar_key[3] = {
  "gyroVar0",
  "gyroVar1",
  "gyroVar2",
};

const char * rpyVar_key[3] = {
  "rpyVar0",
  "rpyVar1",
  "rpyVar2",
};

const char * i2cAddress_key = "i2cAddress";

const char * firstLoad_key = "firstLoad";

const char * params_ns = "params"; // preference namespace

void resetParamsInStorage(){
  storage.begin(params_ns, false);

  for (int i=0; i<num_of_motors; i+=1){
    storage.putDouble(ppr_key[i], 1000.0);
    storage.putDouble(kp_key[i], 0.0);
    storage.putDouble(ki_key[i], 0.0);
    storage.putDouble(kd_key[i], 0.0);
    storage.putDouble(cf_key[i], 1.5);
    storage.putInt(rdir_key[i], 1);
    storage.putDouble(maxVel_key[i], 10.0);
  }
  for (int i=0; i<3; i+=1){
    storage.putFloat(accOff_key[i], 0.0);
    storage.putFloat(accVar_key[i], 0.0);
    storage.putFloat(gyroOff_key[i], 0.0);
    storage.putFloat(gyroVar_key[i], 0.0);
    storage.putFloat(rpyVar_key[i], 0.0);
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
    enc_ppr[i] = storage.getDouble(ppr_key[i], 1000.0);
    kp[i] = storage.getDouble(kp_key[i], 0.0);
    ki[i] = storage.getDouble(ki_key[i], 0.0);
    kd[i] = storage.getDouble(kd_key[i], 0.0);
    cutOffFreq[i] = storage.getDouble(cf_key[i], 1.5);
    rdir[i] = storage.getInt(rdir_key[i], 1);
    maxVel[i] = storage.getDouble(maxVel_key[i], 10.0);
  }
  for (int i=0; i<3; i+=1){
    accOff[i] = storage.getFloat(accOff_key[i], 0.0);
    accVar[i] = storage.getFloat(accVar_key[i], 0.0);
    gyroOff[i] = storage.getFloat(gyroOff_key[i], 0.0);
    gyroVar[i] = storage.getFloat(gyroVar_key[i], 0.0);
    rpyVar[i] = storage.getFloat(rpyVar_key[i], 0.0);
  }
  i2cAddress = storage.getUChar(i2cAddress_key, 0x55);

  storage.end();
}


//-------------------------------------------------//




//--------------- global functions ----------------//

String readPos(int motor_no){
  double posData = encoder[motor_no].getAngPos();
  String data = String(rdir[motor_no] * posData, 3);
  return data;
}

String readVel(int motor_no){
  String data = String((double)rdir[motor_no] * filteredVel[motor_no], 4);
  data += ",";
  data += String((double)rdir[motor_no] * unfilteredVel[motor_no], 4);
  return data;
}


String readPidVel(int motor_no)
{
  String data = String((double)rdir[motor_no] * target[motor_no], 4);
  data += ",";
  data += String((double)rdir[motor_no] * filteredVel[motor_no], 4);
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
  motor[motor_no].sendPWM(rdir[motor_no] * p);

  cmdVelTimeout[motor_no] = millis();
  
  return "1";
}


String writeSpeed(int motor_no, double targetVel)
{
  pidMode[motor_no] = 1;

  double vel;
  if (targetVel > maxVel[motor_no]){
    vel = maxVel[motor_no];
  }
  else if (targetVel < (-1.00 * maxVel[motor_no])){
    vel = -1.00 * maxVel[motor_no];
  }
  else {
    vel = targetVel;
  }
    
  target[motor_no] = (double)rdir[motor_no] * vel;
  isMotorCommanded[motor_no] = 1;

  cmdVelTimeout[motor_no] = millis();

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


String setEncoderPPR(int motor_no, double ppr)
{
  enc_ppr[motor_no] = ppr;
  storage.begin(params_ns, false);
  storage.putDouble(ppr_key[motor_no], enc_ppr[motor_no]);
  storage.end();
  encoder[motor_no].setPulsePerRev(enc_ppr[motor_no]);
  return "1";
}
String getEncoderPPR(int motor_no)
{
  return String(enc_ppr[motor_no]);
}


String setMotorKp(int motor_no, double Kp)
{
  kp[motor_no] = Kp;
  storage.begin(params_ns, false);
  storage.putDouble(kp_key[motor_no], kp[motor_no]);
  storage.end();
  pidMotor[motor_no].setKp(kp[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKp(int motor_no)
{
  return String(kp[motor_no], 4);
}


String setMotorKi(int motor_no, double Ki)
{
  ki[motor_no] = Ki;
  storage.begin(params_ns, false);
  storage.putDouble(ki_key[motor_no], ki[motor_no]);
  storage.end();
  pidMotor[motor_no].setKi(ki[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKi(int motor_no)
{
  return String(ki[motor_no], 4);
}


String setMotorKd(int motor_no, double Kd)
{
  kd[motor_no] = Kd;
  storage.begin(params_ns, false);
  storage.putDouble(kd_key[motor_no], kd[motor_no]);
  storage.end();
  pidMotor[motor_no].setKd(kd[motor_no]);
  pidMotor[motor_no].begin();
  return "1";
}
String getMotorKd(int motor_no)
{
  return String(kd[motor_no], 4);
}


String setRdir(int motor_no, double dir)
{
  if (dir >= 0)
    rdir[motor_no] = 1;
  else
    rdir[motor_no] = -1;
  storage.begin(params_ns, false);
  storage.putInt(rdir_key[motor_no], rdir[motor_no]);
  storage.end();
  return "1";
}
String getRdir(int motor_no)
{
  return String(rdir[motor_no]);
}


String setMaxVel(int motor_no, double max_vel)
{
  maxVel[motor_no] = fabs(max_vel);
  storage.begin(params_ns, false);
  storage.putDouble(maxVel_key[motor_no], maxVel[motor_no]);
  storage.end();
  return "1";
}
String getMaxVel(int motor_no)
{
  return String(maxVel[motor_no], 2);
}


String setCutoffFreq(int motor_no, double f0)
{
  cutOffFreq[motor_no] = f0;
  storage.begin(params_ns, false);
  storage.putDouble(cf_key[motor_no], cutOffFreq[motor_no]);
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
  if (cmdTimeout < 10)
  {
    cmdVelTimeoutInterval = 0;
  }
  else
  {
    cmdVelTimeoutInterval = cmdTimeout;
    for (int i = 0; i < num_of_motors; i += 1)
    {
      cmdVelTimeout[i] = millis();
    }
  }
  return "1";
}
String getCmdTimeout()
{
  return String(cmdVelTimeoutInterval);
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
//-----------------------------------------------------------------//



//------------------------------------------------------------------//
String useImu(){
  return String(use_imu);
}

String readRPY(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(rpy[no], 6);
}

String readAcc(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(accCal[no], 6);
}

String readGyro(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(gyroCal[no], 6);
}

String readQuat(int no)
{
  bool not_allowed = (no < 0) || (no > (3));

  if (not_allowed) 
    return "0.000";

  return String(quat[no], 6);
}

String readAccRaw(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(accRaw[no], 6);
}

String readGyroRaw(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(gyroRaw[no], 6);
}


String readAccOffset(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(accOff[no], 8);
}
String writeAccOffset(int no, float val) {
  bool not_allowed = (no < 0) || (no > (2));
  
  if (not_allowed) 
    return "0";

  accOff[no] = val;
  storage.begin(params_ns, false);
  storage.putFloat(accOff_key[no], accOff[no]);
  storage.end();
  return "1";
}


String readGyroOffset(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(gyroOff[no], 8);
}
String writeGyroOffset(int no, float val) {
  bool not_allowed = (no < 0) || (no > (2));
  
  if (not_allowed) 
    return "0";

  gyroOff[no] = val;
  storage.begin(params_ns, false);
  storage.putFloat(gyroOff_key[no], gyroOff[no]);
  storage.end();
  return "1";
}


String readAccVariance(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(accVar[no], 8);
}
String writeAccVariance(int no, float val) {
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0";

  accVar[no] = val;
  storage.begin(params_ns, false);
  storage.putFloat(accVar_key[no], accVar[no]);
  storage.end();
  return "1";
}


String readGyroVariance(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(gyroVar[no], 6);
}
String writeGyroVariance(int no, float val) {
  bool not_allowed = (no < 0) || (no > (2));
  
  if (not_allowed) 
    return "0";

  gyroVar[no] = val;
  storage.begin(params_ns, false);
  storage.putFloat(gyroVar_key[no], gyroVar[no]);
  storage.end();
  return "1";
}


String readRPYVariance(int no)
{
  bool not_allowed = (no < 0) || (no > (2));

  if (not_allowed) 
    return "0.000";

  return String(rpyVar[no], 6);
}
String writeRPYVariance(int no, float val) {
  bool not_allowed = (no < 0) || (no > (2));
  
  if (not_allowed) 
    return "0";

  rpyVar[no] = val;
  storage.begin(params_ns, false);
  storage.putFloat(rpyVar_key[no], rpyVar[no]);
  storage.end();
  return "1";
}

//-------------------------------------------------------------------//


#endif