
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

  float roll, pitch, yaw;

  auto prevTime = std::chrono::system_clock::now();
  std::chrono::duration<double> duration;
  float sampleTime = 0.02;

  // std::string port = "/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.4:1.0-port0";
  std::string port = "/dev/ttyUSB0";
  epmcV2.connect(port);

  // // wait for the epmcV2 to fully setup
  // for (int i = 1; i <= 6; i += 1)
  // {
  //   delay_ms(1000);
  //   std::cout << "configuring controller: " << i << " sec" << std::endl;
  // }

  // // change the reference frame to ENU frame (0 - NWU,  1 - ENU,  2 - NED)
  // epmcV2.setRefFrame(1);
  // int ref_frame_id;

  // // check the refence frame the epmcV2 is working in (0 - NWU,  1 - ENU,  2 - NED)
  // epmcV2.getRefFrame(ref_frame_id);
  // if (ref_frame_id == 0)
  //   std::cout << "Reference Frame is North-West-Up (NWU) " << std::endl;
  // else if (ref_frame_id == 1)
  //   std::cout << "Reference Frame is East-North-Up (ENU) " << std::endl;
  // else if (ref_frame_id == 2)
  //   std::cout << "Reference Frame is North-East-Down (NED) " << std::endl;

  // wait for the epmcV2 to fully setup
  for (int i = 1; i <= 2; i += 1)
  {
    delay_ms(1000);
    std::cout << "configuring controller: " << i << " sec" << std::endl;
  }

  prevTime = std::chrono::system_clock::now();

  while (true)
  {
    duration = (std::chrono::system_clock::now() - prevTime);
    if (duration.count() > sampleTime)
    {
      try
      {
        roll = epmcV2.readRPY(0);
        pitch = epmcV2.readRPY(1);
        yaw = epmcV2.readRPY(2);
      }
      catch (...)
      {
      }

      std::cout << "ROLL: " << roll << std::fixed << std::setprecision(4) << std::endl;
      std::cout << "PITCH: " << pitch << std::fixed << std::setprecision(4) << std::endl;
      std::cout << "YAW: " << yaw << std::fixed << std::setprecision(4) << std::endl;
      std::cout << std::endl;

      prevTime = std::chrono::system_clock::now();
    }
  }
}