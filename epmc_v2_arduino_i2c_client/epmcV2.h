#ifndef EPMC_V2_H
#define EPMC_V2_H

#include <Arduino.h>
#include <Wire.h>

class EPMC_V2
{
public:
  EPMC_V2(int);

  bool writeSpeed(int, float);

  bool writePWM(int, int);

  void readPos(int, float &);

  void readVel(int, float &);

  void readVelFull(int, float &, float &);

  bool setCmdTimeout(int);

  void getCmdTimeout(int &);

  bool setPidMode(int, int);

  void getPidMode(int, int &);


private:
  int slaveAddr;
  String dataMsg = "", dataMsgBuffer = "", dataBuffer[2];
  float valA, valB;

  void get(String, int);

  bool send(String, int, float);

  void masterSendData(String);

  String masterReceiveData();
};

#endif


// #include <Arduino.h>
// #include "Wire.h"

// #define I2C_DEV_ADDR 0x55

// uint32_t i = 0;


// void masterSendData(String i2c_msg)
// {
//   char charArray[i2c_msg.length() + 1];
//   i2c_msg.toCharArray(charArray, i2c_msg.length() + 1);

//   Wire.beginTransmission(I2C_DEV_ADDR);
//   Wire.write(charArray);
//   Wire.endTransmission();
// }

// String masterReceiveData()
// {
//   String i2c_msg = "";
//   Wire.requestFrom(I2C_DEV_ADDR, 32);
//   while (Wire.available())
//   {
//     char c = Wire.read();
//     i2c_msg += c;
//   }
//   int indexPos = i2c_msg.indexOf((char)255);
//   if (indexPos != -1)
//   {
//     return i2c_msg.substring(0, indexPos);
//   }
//   return i2c_msg;
// }


// void setup() {
//   Serial.begin(115200);
//   // Serial.setDebugOutput(true);
//   Wire.begin();

//   delay(1000);
//   masterSendData("/timeout,-1,5000");
//   masterSendData("/pwm,0,60");
//   masterSendData("/pwm,1,100");
//   // String reply = masterReceiveData();
//   // Serial.println(reply);
// }

// void loop() {
//   String reply;
//   delay(20);

//   masterSendData("/vel,0");
//   reply = masterReceiveData();
//   Serial.println(reply);

//   masterSendData("/vel,1");
//   reply = masterReceiveData();
//   Serial.println(reply);

//   Serial.println();
// }