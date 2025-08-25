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
      int motor_no = dataMsgBufferArray[1].toInt();
      bool motor_no_not_found = (motor_no < 0) || (motor_no > (num_of_motors-1));

      digitalWrite(LED_BUILTIN, HIGH);

      if (dataMsgBufferArray[0] == "/pos")
      {
        if (motor_no_not_found)
          sendMsg = "0.000";
        else
          sendMsg = readPos(motor_no);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/pvel")
      {
        if (motor_no_not_found)
          sendMsg = "0.0,0.0";
        else
          sendMsg = readPidVel(motor_no);
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/pwm")
      {
        if (motor_no_not_found)
          sendMsg = "0";
        else
          sendMsg = writePWM(motor_no, dataMsgBufferArray[2].toInt());
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/vel")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "0.0,0.0";
          else
            sendMsg = readVel(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = "0";
          else
            sendMsg = writeSpeed(motor_no, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/mode")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "-1";
          else
            sendMsg = getPidModeFunc(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = String(motor_no);
          else
            sendMsg = setPidModeFunc(motor_no, dataMsgBufferArray[2].toInt());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/ppr")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getEncoderPPR(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = "0";
          else
            sendMsg = setEncoderPPR(motor_no, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kp")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getMotorKp(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = String(motor_no);
          else
            sendMsg = setMotorKp(motor_no, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/ki")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getMotorKi(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = "0";
          else
            sendMsg = setMotorKi(motor_no, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/kd")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getMotorKd(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = "0";
          else
            sendMsg = setMotorKd(motor_no, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/rdir")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getRdir(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = "0";
          else
            sendMsg = setRdir(motor_no, dataMsgBufferArray[2].toInt());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/cut-freq")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getCutoffFreq(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = "0";
          else
            sendMsg = setCutoffFreq(motor_no, dataMsgBufferArray[2].toDouble());
        }
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/max-vel")
      {
        if (dataMsgBufferArray[2] == ""){
          if (motor_no_not_found)
            sendMsg = "0.0";
          else
            sendMsg = getMaxVel(motor_no);
        }
        else {
          if (motor_no_not_found)
            sendMsg = "0";
          else
            sendMsg = setMaxVel(motor_no, dataMsgBufferArray[2].toDouble());
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

      else if (dataMsgBufferArray[0] == "/reset")
      {
        sendMsg = triggerResetParams();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/rpy")
      {
        sendMsg = readRPY();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/quat")
      {
        sendMsg = readQuat();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/acc")
      {
        sendMsg = readAcc();
        Serial.println(sendMsg);
      }

      else if (dataMsgBufferArray[0] == "/gyro")
      {
        sendMsg = readGyro();
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