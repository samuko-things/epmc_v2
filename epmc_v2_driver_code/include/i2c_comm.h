#ifndef I2C_COMM_H
#define I2C_COMM_H

#include <Wire.h>
#include "command_functions.h"

String i2cDataMsg = "", i2cDataMsgBuffer = "", i2cDataMsgBufferArray[3];
String i2cSendMsg = "";


void onRequest() {
  char charArray[i2cSendMsg.length() + 1];
  i2cSendMsg.toCharArray(charArray, i2cSendMsg.length() + 1);
  Wire.slaveWrite((u_int8_t *)charArray, i2cSendMsg.length());

  i2cSendMsg = "";
}


void onReceive(int dataSizeInBytes) {
  int indexPos = 0, i = 0;

  for (int i = 0; i < dataSizeInBytes; i += 1)
  {
    char c = Wire.read();
    i2cDataMsg += c;
  }

  i2cDataMsg.trim();

  if (i2cDataMsg != "")
  {
    do
    {
      indexPos = i2cDataMsg.indexOf(',');
      if (indexPos != -1)
      {
        i2cDataMsgBuffer = i2cDataMsg.substring(0, indexPos);
        i2cDataMsg = i2cDataMsg.substring(indexPos + 1, i2cDataMsg.length());
        i2cDataMsgBufferArray[i] = i2cDataMsgBuffer;
        i2cDataMsgBuffer = "";
      }
      else
      {
        if (i2cDataMsg.length() > 0)
          i2cDataMsgBufferArray[i] = i2cDataMsg;
      }
      i += 1;
    } while (indexPos >= 0);
  }

  if (i2cDataMsgBufferArray[0] != "")
  {
    int motor_no = i2cDataMsgBufferArray[1].toInt();
    bool motor_no_not_found = (motor_no < 0) || (motor_no > (num_of_motors-1));

    digitalWrite(LED_BUILTIN, HIGH);

    if (i2cDataMsgBufferArray[0] == "/pos")
    {
      if (motor_no_not_found)
        i2cSendMsg = "0.000";
      else
        i2cSendMsg = readPos(motor_no);
    }

    else if (i2cDataMsgBufferArray[0] == "/pwm")
    {
      if (motor_no_not_found)
        i2cSendMsg = "0";
      else
        i2cSendMsg = writePWM(motor_no, i2cDataMsgBufferArray[2].toInt());
    }

    else if (i2cDataMsgBufferArray[0] == "/vel")
    {
      if (i2cDataMsgBufferArray[2] == ""){
        if (motor_no_not_found)
          i2cSendMsg = "0.0,0.0";
        else
          i2cSendMsg = readVel(motor_no);
      }
      else {
        if (motor_no_not_found)
          i2cSendMsg = "0";
        else
          i2cSendMsg = writeSpeed(motor_no, i2cDataMsgBufferArray[2].toDouble());
      }
    }

    else if (i2cDataMsgBufferArray[0] == "/mode")
      {
        if (i2cDataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            i2cSendMsg = "-1";
          else
            i2cSendMsg = getPidModeFunc(motor_no);
        }
        else {
          if (motor_no_not_found)
            i2cSendMsg = String(motor_no);
          else
            i2cSendMsg = setPidModeFunc(motor_no, i2cDataMsgBufferArray[2].toInt());
        }
      }

    else if (i2cDataMsgBufferArray[0] == "/timeout")
    {
      if (i2cDataMsgBufferArray[2] == ""){
        i2cSendMsg = getCmdTimeout();
      }
      else {
        i2cSendMsg = setCmdTimeout(i2cDataMsgBufferArray[2].toInt());
      }
    }

    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);

    i2cSendMsg = "0";

    digitalWrite(LED_BUILTIN, LOW);
  }

  i2cDataMsg = "";
  i2cDataMsgBuffer = "";
  i2cDataMsgBufferArray[0] = "";
  i2cDataMsgBufferArray[1] = "";
  i2cDataMsgBufferArray[2] = "";
}

#endif