
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

  float lowTargetVel = 0.00; // in rad/sec
  float highTargetVel = 3.142; // in rad/sec

  float angPos0, angPos1, angPos2, angPos3;
  float angVel0, angVel1, angVel2, angVel3;

  auto prevTime = std::chrono::system_clock::now();
  std::chrono::duration<double> duration;
  float sampleTime = 0.05;

  auto ctrlPrevTime = std::chrono::system_clock::now();
  std::chrono::duration<double> ctrlDuration;
  float ctrlSampleTime = 4.0;

  // std::string port = "/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.4:1.0-port0";
  std::string port = "/dev/ttyUSB0";
  epmcV2.connect(port);

  delay_ms(2000);

  // left wheels (motor 0 and motor 2)
  epmcV2.writeSpeed(0, 0.00);
  epmcV2.writeSpeed(2, 0.00);

  // right wheels (motor 1 and motor 3)
  epmcV2.writeSpeed(1, 0.00);
  epmcV2.writeSpeed(3, 0.00);

  // int motor_cmd_timeout_ms = 0;
  // epmcV2.setCmdTimeout(motor_cmd_timeout_ms); // set motor command timeout
  // motor_cmd_timeout_ms = epmcV2.getCmdTimeout();
  // std::cout << "motor command timeout: " << motor_cmd_timeout_ms << " ms" << std::endl;

  // left wheels (motor 0 and motor 2)
  epmcV2.writeSpeed(0, lowTargetVel);
  epmcV2.writeSpeed(2, lowTargetVel);

  // right wheels (motor 1 and motor 3)
  epmcV2.writeSpeed(1, lowTargetVel);
  epmcV2.writeSpeed(3, lowTargetVel);

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
        // left wheels (motor 0 and motor 2)
        epmcV2.writeSpeed(0, highTargetVel);
        epmcV2.writeSpeed(2, highTargetVel);

        // right wheels (motor 1 and motor 3)
        epmcV2.writeSpeed(1, highTargetVel);
        epmcV2.writeSpeed(3, highTargetVel);

        sendHigh = false;
      }
      else
      {
        // left wheels (motor 0 and motor 2)
        epmcV2.writeSpeed(0, lowTargetVel);
        epmcV2.writeSpeed(2, lowTargetVel);

        // right wheels (motor 1 and motor 3)
        epmcV2.writeSpeed(1, lowTargetVel);
        epmcV2.writeSpeed(3, lowTargetVel);

        sendHigh = true;
      }

      ctrlPrevTime = std::chrono::system_clock::now();
    }

    duration = (std::chrono::system_clock::now() - prevTime);
    if (duration.count() > sampleTime)
    {
      try
      {
        // left wheels (motor 0 and motor 2)
        angPos0 = epmcV2.readPos(0);
        angVel0 = epmcV2.readVel(0);

        angPos2 = epmcV2.readPos(2);
        angVel2 = epmcV2.readVel(2);

        // left wheels (motor 1 and motor 3)
        angPos1 = epmcV2.readPos(1);
        angVel1 = epmcV2.readVel(1);

        angPos3 = epmcV2.readPos(3);
        angVel3 = epmcV2.readVel(3);
      }
      catch (...)
      {
        
      }

      std::cout << "----------------------------------" << std::endl;
      std::cout << "left wheels - motor 0 and motor 2" << std::endl;
      std::cout << "motor0_readings: [" << angPos0 << std::fixed << std::setprecision(4) << "," << angVel0 << std::fixed << std::setprecision(4) << "]" << std::endl;
      std::cout << "motor2_readings: [" << angPos2 << std::fixed << std::setprecision(4) << "," << angVel2 << std::fixed << std::setprecision(4) << "]" << std::endl;
      std::cout << std::endl;
      std::cout << "right wheels - motor 1 and motor 3" << std::endl;
      std::cout << "motor1_readings: [" << angPos1 << std::fixed << std::setprecision(4) << "," << angVel1 << std::fixed << std::setprecision(4) << "]" << std::endl;
      std::cout << "motor3_readings: [" << angPos3 << std::fixed << std::setprecision(4) << "," << angVel3 << std::fixed << std::setprecision(4) << "]" << std::endl;
      std::cout << "----------------------------------" << std::endl;
      std::cout << std::endl;

      prevTime = std::chrono::system_clock::now();
    }
  }
}