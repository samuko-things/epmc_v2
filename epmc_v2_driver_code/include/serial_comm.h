#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#include "command_functions.h"

String dataMsg = "", dataMsgBuffer = "", dataMsgBufferArray[3];
String sendMsg = "";


void recieve_and_send_data(){
  int indexPos = 0, i = 0;

  if (Serial.available() > 0)
  {
    while (Serial.available())
    {
      dataMsg = Serial.readString();
    }
    dataMsg.trim();
    if (dataMsg != "")
    {
      do
      {
        indexPos = dataMsg.indexOf(',');
        if (indexPos != -1)
        {
          dataMsgBuffer = dataMsg.substring(0, indexPos);
          dataMsg = dataMsg.substring(indexPos + 1, dataMsg.length());
          dataMsgBufferArray[i] = dataMsgBuffer;
          dataMsgBuffer = "";
        }
        else
        {
          if (dataMsg.length() > 0)
            dataMsgBufferArray[i] = dataMsg;
        }
        i += 1;
      } while (indexPos >= 0);
    }


    if (dataMsgBufferArray[0] != "")
    {
      int pos = dataMsgBufferArray[1].toInt();
      bool pos_not_found = (pos < 0) || (pos > (num_of_motors-1));

      digitalWrite(LED_BUILTIN, HIGH);

      if (dataMsgBufferArray[0] == "/pos")
      {
        if (pos_not_found)
          sendMsg = "0.000";
        else
          sendMsg = readPos(pos);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/pvel")
      {
        if (pos_not_found)
          sendMsg = "0.0,0.0";
        else
          sendMsg = readPidVel(pos);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/pwm")
      {
        if (pos_not_found)
          sendMsg = "0";
        else
          sendMsg = writePWM(pos, dataMsgBufferArray[2].toInt());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/vel")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.0,0.0";
          else
            sendMsg = readVel(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = writeSpeed(pos, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/mode")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "-1";
          else
            sendMsg = getPidModeFunc(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = String(pos);
          else
            sendMsg = setPidModeFunc(pos, dataMsgBufferArray[2].toInt());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/ppr")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getEncoderPPR(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = setEncoderPPR(pos, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kp")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getMotorKp(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = String(pos);
          else
            sendMsg = setMotorKp(pos, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/ki")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getMotorKi(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = setMotorKi(pos, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kd")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getMotorKd(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = setMotorKd(pos, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/rdir")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getRdir(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = setRdir(pos, dataMsgBufferArray[2].toInt());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/cut-freq")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getCutoffFreq(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = setCutoffFreq(pos, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/max-vel")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getMaxVel(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = setMaxVel(pos, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/timeout")
      {
        if (dataMsgBufferArray[2] == ""){
          sendMsg = getCmdTimeout();
        }
        else {
          sendMsg = setCmdTimeout(dataMsgBufferArray[2].toInt());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/i2c")
      {
        if (dataMsgBufferArray[2] == ""){
          sendMsg = "-1";
        }
        else {
          sendMsg = "0";
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/reset")
      {
        sendMsg = triggerResetParams();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/use-imu")
      {
        sendMsg = useImu();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/rpy")
      {
        if (pos_not_found)
          sendMsg = "0.00";
        else
          sendMsg = readRPY(pos);
        Serial.println(sendMsg);
      }
      else if (dataMsgBufferArray[0] == "/quat")
      {
        if (pos_not_found)
          sendMsg = "0.00";
        else
          sendMsg = readQuat(pos);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/acc")
      {
        if (pos_not_found)
          sendMsg = "0.00";
        else
          sendMsg = readAcc(pos);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/gyro")
      {
        if (pos_not_found)
          sendMsg = "0.00";
        else
          sendMsg = readGyro(pos);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/acc-raw")
      {
        if (pos_not_found)
          sendMsg = "0.00";
        else
          sendMsg = readAccRaw(pos);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/gyro-raw")
      {
        if (pos_not_found)
          sendMsg = "0.00";
        else
          sendMsg = readGyroRaw(pos);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/acc-off")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.00";
          else
            sendMsg = readAccOffset(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = writeAccOffset(pos, dataMsgBufferArray[2].toFloat());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/gyro-off")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.00";
          else
            sendMsg = readGyroOffset(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = writeGyroOffset(pos, dataMsgBufferArray[2].toFloat());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/acc-var")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.00";
          else
            sendMsg = readAccVariance(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = writeAccVariance(pos, dataMsgBufferArray[2].toFloat());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/gyro-var")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.00";
          else
            sendMsg = readGyroVariance(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = writeGyroVariance(pos, dataMsgBufferArray[2].toFloat());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/rpy-var")
      {
        if (dataMsgBufferArray[2] == ""){
          if (pos_not_found)
            sendMsg = "0.00";
          else
            sendMsg = readRPYVariance(pos);
        }
        else {
          if (pos_not_found)
            sendMsg = "0";
          else
            sendMsg = writeRPYVariance(pos, dataMsgBufferArray[2].toFloat());
        }
        Serial.println(sendMsg);
      }

      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH);

      sendMsg = "0";
      Serial.println(sendMsg);

      digitalWrite(LED_BUILTIN, LOW);
    }

    sendMsg = "";
    dataMsg = "";
    dataMsgBuffer = "";
    dataMsgBufferArray[0] = "";
    dataMsgBufferArray[1] = "";
    dataMsgBufferArray[2] = "";
  } 
}

#endif