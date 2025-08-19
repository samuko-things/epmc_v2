#include "epmcV2.h"

EPMC_V2::EPMC_V2(int slave_addr)
{
  slaveAddr = slave_addr;
}

bool EPMC_V2::writeSpeed(int motor_no, float speed)
{
  return send("/vel", motor_no, speed);
}

bool EPMC_V2::writePWM(int motor_no, int pwm)
{
  return send("/pwm", motor_no, pwm);
}

void EPMC_V2::readPos(int motor_no, float &angPos)
{
  get("/pos", motor_no);

  angPos = valA;

  valA = 0.0;
  valB = 0.0;
}

void EPMC_V2::readVel(int motor_no, float &filteredAngVel)
{
  get("/vel", motor_no);

  filteredAngVel = valA;

  valA = 0.0;
  valB = 0.0;
}

void EPMC_V2::readVelFull(int motor_no, float &filteredAngVel, float &unfilteredAngVel)
{
  get("/vel", motor_no);

  filteredAngVel = valA;
  unfilteredAngVel = valB;

  valA = 0.0;
  valB = 0.0;
}

bool EPMC_V2::setCmdTimeout(int timeout_ms = 0)
{
  return send("/timeout", -1, timeout_ms);
}

void EPMC_V2::getCmdTimeout(int &timeout_ms)
{
  get("/timeout", -1);

  timeout_ms = (int)valA;

  valA = 0.0;
  valB = 0.0;
}

bool EPMC_V2::setPidMode(int motor_no, int mode)
{
  return send("/mode", motor_no, mode);
}

void EPMC_V2::getPidMode(int motor_no, int &mode)
{
  get("/mode", motor_no);

  mode = (int)valA;

  valA = 0.0;
  valB = 0.0;
}

void EPMC_V2::get(String cmd_route, int motor_no)
{
  String msg_buffer = cmd_route;
  msg_buffer += ",";
  msg_buffer += String(motor_no);

  masterSendData(msg_buffer);
  dataMsg = masterReceiveData();

  int indexPos = 0, i = 0;
  do
  {
    indexPos = dataMsg.indexOf(',');
    if (indexPos != -1)
    {
      dataMsgBuffer = dataMsg.substring(0, indexPos);
      dataMsg = dataMsg.substring(indexPos + 1, dataMsg.length());
      dataBuffer[i] = dataMsgBuffer;
      dataMsgBuffer = "";
    }
    else
    {
      if (dataMsg.length() > 0)
        dataBuffer[i] = dataMsg;
    }
    i += 1;
  } while (indexPos >= 0);

  valA = dataBuffer[0].toFloat();
  valB = dataBuffer[1].toFloat();

  dataMsg = "";
  dataMsgBuffer = "";
  dataBuffer[0] = "";
  dataBuffer[1] = "";
}

bool EPMC_V2::send(String cmd_route, int motor_no, float val)
{
  String msg_buffer = cmd_route;
  msg_buffer += ",";
  msg_buffer += String(motor_no);
  msg_buffer += ",";
  msg_buffer += String(val, 3);

  masterSendData(msg_buffer);
  String data = masterReceiveData();
  if (data == "1")
    return true;
  else
    return false;
}

void EPMC_V2::masterSendData(String i2c_msg)
{
  char charArray[i2c_msg.length() + 1];
  i2c_msg.toCharArray(charArray, i2c_msg.length() + 1);

  Wire.beginTransmission(slaveAddr);
  Wire.write(charArray);
  Wire.endTransmission();
}

String EPMC_V2::masterReceiveData()
{
  String i2c_msg = "";
  Wire.requestFrom(slaveAddr, 32);
  while (Wire.available())
  {
    char c = Wire.read();
    i2c_msg += c;
  }
  int indexPos = i2c_msg.indexOf((char)255);
  if (indexPos != -1)
  {
    return i2c_msg.substring(0, indexPos);
  }
  return i2c_msg;
}