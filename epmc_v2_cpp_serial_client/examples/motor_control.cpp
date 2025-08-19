
#include <sstream>
#include <iostream>
#include <unistd.h>

#include <chrono>

#include <iomanip>

#include "epmc_v2.hpp"

EPMC_V2 epmcV2;

void delay_ms(unsigned long milliseconds)
{
  usleep(milliseconds * 1000);
}

int main(int argc, char **argv)
{

  bool sendHigh = false;

  float lowTargetVel = -1.57; // in rad/sec
  float highTargetVel = 1.57; // in rad/sec

  float angPos0, angPos1;
  float angVel0, angVel1;

  auto prevTime = std::chrono::system_clock::now();
  std::chrono::duration<double> duration;
  float sampleTime = 0.02;

  auto ctrlPrevTime = std::chrono::system_clock::now();
  std::chrono::duration<double> ctrlDuration;
  float ctrlSampleTime = 5.0;

  // std::string port = "/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.4:1.0-port0";
  std::string port = "/dev/ttyUSB0";
  epmcV2.connect(port);

  delay_ms(2000);

  epmcV2.writeSpeed(0, 0.00);
  epmcV2.writeSpeed(1, 0.00);

  int motor_cmd_timeout_ms = 4000;
  epmcV2.setCmdTimeout(motor_cmd_timeout_ms); // set motor command timeout
  epmcV2.getCmdTimeout(motor_cmd_timeout_ms);
  std::cout << "motor command timeout: " << motor_cmd_timeout_ms << " ms" << std::endl;

  epmcV2.writeSpeed(0, lowTargetVel);
  epmcV2.writeSpeed(1, lowTargetVel);
  sendHigh = true;

  prevTime = std::chrono::system_clock::now();
  ctrlPrevTime = std::chrono::system_clock::now();

  while (true)
  {

    ctrlDuration = (std::chrono::system_clock::now() - ctrlPrevTime);
    if (ctrlDuration.count() > ctrlSampleTime)
    {
      if (sendHigh)
      {
        epmcV2.writeSpeed(0, highTargetVel);
        epmcV2.writeSpeed(1, highTargetVel);
        sendHigh = false;
      }
      else
      {
        epmcV2.writeSpeed(0, lowTargetVel);
        epmcV2.writeSpeed(1, lowTargetVel);
        sendHigh = true;
      }

      ctrlPrevTime = std::chrono::system_clock::now();
    }

    duration = (std::chrono::system_clock::now() - prevTime);
    if (duration.count() > sampleTime)
    {
      try
      {
        epmcV2.readPos(0, angPos0);
        epmcV2.readVel(0, angVel0);

        epmcV2.readPos(1, angPos1);
        epmcV2.readVel(1, angVel1);
      }
      catch (...)
      {
        
      }

      std::cout << "motor0_readings: [" << angPos0 << std::fixed << std::setprecision(4) << "," << angVel0 << std::fixed << std::setprecision(4) << "]" << std::endl;
      std::cout << "motor1_readings: [" << angPos1 << std::fixed << std::setprecision(4) << "," << angVel1 << std::fixed << std::setprecision(4) << "]" << '\n'
                << std::endl;

      prevTime = std::chrono::system_clock::now();
    }
  }
}