#ifndef PARAMETER_STORE_H
#define PARAMETER_STORE_H

#include <Arduino.h>
#include <Preferences.h>
#include "command_functions.h"

//--------------- global variables -----------------//
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
//-------------------------------------------------//


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
  storage.begin(params_ns, false);
  //check for firstLoad
  firstLoad = storage.getBool(firstLoad_key, false);
  // if firsLoad -> reset all params and set firstLoad to false
  if(firstLoad){
    resetParamsInStorage();
    firstLoad = false;
    storage.putBool(firstLoad_key, firstLoad);
  }

  storage.end();
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

#endif