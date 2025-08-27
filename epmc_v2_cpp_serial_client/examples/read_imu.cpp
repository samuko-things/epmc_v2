
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
  float ax, ay, az;
  float gx, gy, gz;

  auto prevTime = std::chrono::system_clock::now();
  std::chrono::duration<double> duration;
  float sampleTime = 0.01;

  std::string port = "/dev/ttyUSB0";
  epmcV2.connect(port);

  // wait for the epmcV2 to fully setup
  for (int i = 1; i <= 2; i += 1)
  {
    delay_ms(1000);
    std::cout << "configuring controller: " << i << " sec" << std::endl;
  }

  int use_imu = epmcV2.getUseIMU();

  prevTime = std::chrono::system_clock::now();

  while (true)
  {
    duration = (std::chrono::system_clock::now() - prevTime);
    if (duration.count() > sampleTime)
    {
      try
      {
        if (use_imu == 1){
          ax = epmcV2.readAcc(0);
          ay = epmcV2.readAcc(1);
          az = epmcV2.readAcc(2);

          gx = epmcV2.readGyro(0);
          gy = epmcV2.readGyro(1);
          gz = epmcV2.readGyro(2);
        }
      }
      catch (...)
      {
      }
      if (use_imu == 1){
        std::cout << "ax: " << ax << std::fixed << std::setprecision(4);
        std::cout << "\tay: " << ax << std::fixed << std::setprecision(4);
        std::cout << "\taz: " << az << std::fixed << std::setprecision(4) << std::endl;
        std::cout << "gx: " << ax << std::fixed << std::setprecision(4);
        std::cout << "\tgy: " << ax << std::fixed << std::setprecision(4);
        std::cout << "\tgz: " << az << std::fixed << std::setprecision(4) << std::endl;
        std::cout << std::endl;
      }
      else{
        std::cout << "IMU Mode Not Activated" << std::endl;
      }
      

      prevTime = std::chrono::system_clock::now();
    }
  }
}