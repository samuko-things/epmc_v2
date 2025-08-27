#ifndef EPMC_V2_HPP
#define EPMC_V2_HPP

#include <sstream>
#include <libserial/SerialPort.h>
#include <iostream>

#include <chrono>

LibSerial::BaudRate convert_baud_rate(int baud_rate)
{
  // Just handle some common baud rates
  switch (baud_rate)
  {
  case 1200:
    return LibSerial::BaudRate::BAUD_1200;
  case 1800:
    return LibSerial::BaudRate::BAUD_1800;
  case 2400:
    return LibSerial::BaudRate::BAUD_2400;
  case 4800:
    return LibSerial::BaudRate::BAUD_4800;
  case 9600:
    return LibSerial::BaudRate::BAUD_9600;
  case 19200:
    return LibSerial::BaudRate::BAUD_19200;
  case 38400:
    return LibSerial::BaudRate::BAUD_38400;
  case 57600:
    return LibSerial::BaudRate::BAUD_57600;
  case 115200:
    return LibSerial::BaudRate::BAUD_115200;
  case 230400:
    return LibSerial::BaudRate::BAUD_230400;
  default:
    std::cout << "Error! Baud rate " << baud_rate << " not supported! Default to 57600" << std::endl;
    return LibSerial::BaudRate::BAUD_57600;
  }
}

class EPMC_V2
{

public:
  EPMC_V2() = default;

  void connect(const std::string &serial_device, int32_t baud_rate = 115200, int32_t timeout_ms = 100)
  {
    timeout_ms_ = timeout_ms;
    serial_conn_.Open(serial_device);
    serial_conn_.SetBaudRate(convert_baud_rate(baud_rate));
  }

  void disconnect()
  {
    serial_conn_.Close();
  }

  bool connected() const
  {
    return serial_conn_.IsOpen();
  }

  float readPos(int motor_no)
  {
    std::stringstream cmd_str;
    cmd_str << "/pos" << "," << motor_no;
    get(cmd_str.str());

    float angPos = val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return angPos;
  }

  float readVel(int motor_no)
  {
    std::stringstream cmd_str;
    cmd_str << "/vel" << "," << motor_no;
    get(cmd_str.str());

    float filteredAngVel = val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return filteredAngVel;
  }

  void readVelFull(int motor_no, float &filteredAngVel, float &unfilteredAngVel)
  {
    std::stringstream cmd_str;
    cmd_str << "/vel" << "," << motor_no;
    get(cmd_str.str());

    filteredAngVel = val[0];
    unfilteredAngVel = val[1];

    val[0] = 0.0;
    val[1] = 0.0;
  }

  bool writePWM(int motor_no, int pwm_val)
  {
    return send("/pwm", motor_no, (float)pwm_val);
  }

  bool writeSpeed(int motor_no, float speed_rps)
  {
    return send("/vel", motor_no, speed_rps);
  }

  bool setCmdTimeout(int timeout_ms)
  {
    return send("/timeout", -1, (float)timeout_ms);
  }

  int getCmdTimeout()
  {
    std::stringstream cmd_str;
    cmd_str << "/timeout" << "," << -1;
    get(cmd_str.str());

    int timeout_ms = (int)val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return timeout_ms;
  }

  bool setPidMode(int motor_no, int mode)
  {
    return send("/mode", motor_no, (float)mode);
  }

  int getPidMode(int motor_no)
  {
    std::stringstream cmd_str;
    cmd_str << "/mode" << "," << motor_no;
    get(cmd_str.str());

    int mode = (int)val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return mode;
  }

  int getUseIMU()
  {
    std::stringstream cmd_str;
    cmd_str << "/use-imu" << "," << -1;
    get(cmd_str.str());

    int mode = (int)val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return mode;
  }

  float readAcc(int pos_no)
  {
    std::stringstream cmd_str;
    cmd_str << "/acc" << "," << pos_no;
    get(cmd_str.str());

    float acc_val = val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return acc_val;
  }

  float readAccVarinace(int pos_no)
  {
    std::stringstream cmd_str;
    cmd_str << "/acc-var" << "," << pos_no;
    get(cmd_str.str());

    float acc_val = val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return acc_val;
  }

  float readGyro(int pos_no)
  {
    std::stringstream cmd_str;
    cmd_str << "/gyro" << "," << pos_no;
    get(cmd_str.str());

    float gyro_val = val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return gyro_val;
  }

  float readGyroVariance(int pos_no)
  {
    std::stringstream cmd_str;
    cmd_str << "/gyro-var" << "," << pos_no;
    get(cmd_str.str());

    float gyro_val = val[0];

    val[0] = 0.0;
    val[1] = 0.0;

    return gyro_val;
  }

private:
  LibSerial::SerialPort serial_conn_;
  int timeout_ms_;
  float val[2];

  std::string send_and_receive(const std::string &msg_cmd)
  {
    auto prev_time = std::chrono::system_clock::now();
    std::chrono::duration<double> duration;

    std::string response = "";

    serial_conn_.FlushIOBuffers(); // Just in case

    while (response == "")
    {
      try
      {

        try
        {
          serial_conn_.Write(msg_cmd);
          serial_conn_.ReadLine(response, '\n', timeout_ms_);
          duration = (std::chrono::system_clock::now() - prev_time);
        }
        catch (const LibSerial::ReadTimeout &)
        {
          continue;
        }

        duration = (std::chrono::system_clock::now() - prev_time);
        if (duration.count() > 2.0)
        {
          throw duration.count();
        }
      }
      catch (double x)
      {
        std::cerr << "Error getting response from ESP32, wasted much time \n";
      }
    }

    return response;
  }

  bool send(std::string cmd_route, int motor_no, float val)
  {
    std::stringstream msg_stream;
    msg_stream << cmd_route << "," << motor_no << "," << val;

    std::string res = send_and_receive(msg_stream.str());

    int data = std::stoi(res);
    if (data)
      return true;
    else
      return false;
  }

  void get(std::string cmd_route)
  {
    std::string res = send_and_receive(cmd_route);

    std::stringstream ss(res);
    std::vector<std::string> v;

    while (ss.good())
    {
      std::string substr;
      getline(ss, substr, ',');
      v.push_back(substr);
    }

    for (size_t i = 0; i < v.size(); i++)
    {
      val[i] = std::atof(v[i].c_str());
    }
  }
};

#endif