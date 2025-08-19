## epmc_v2_arduino_i2c_client
This library helps communicate with the already setup **`Easy PID Motor Controller Module`** (i.e **`L298N EPMC Module`**) with  arduino microcontroller projects via I2C, after successful setup with the [epmc_setup_application](https://github.com/samuko-things-company/epmc_setup_application).

> you can use it in your Arduino-based robotics project (e.g Arduino UNO, Arduino NANO, Arduino MEGA, Esp32, etc.)

A simple way to get started is simply to try out and follow the example code


## How to Use the Library
- Ensure you have the **`Easy PID Motor Controller Module`** (i.e **`L298N EPMC Module`**). Calibrate it and set it up using the **`epmc_setup_application`**.

- Download download the library by clicking on the green Code button above (or clone it)
  > if you download it, extract it and change the folder name to `epmc_arduino`

- Move the downloaded library file - **`epmc_arduino`** - to your Arduino library folder
  > e.g on linux: ... home/Arduino/libraries/
  >
  > e.g on windows: ... Documents/Arduino/libraries/
  >
  > (or any where your arduino libraries are stored)

- restart your ArduinoIDE and navigate to examples and run the example code and see how to control the motors.

- you can copy this example code into your project and modify it to your taste.


## Basic Library functions and usage

- send target angular velocity command
  > .writeSpeed(motor_no, target_vel)

- send PWM commands
  > .writePWM(motor_no, pwm)

- set motor command timeout in ms
  > .setCmdTimeout(timeout_ms) // sets command timeout in ms

- set motor command timeout in ms
  > .getCmdTimeout(&timeout_ms) // sets command timeout in ms

- read motor angular position
  > .readPos(motor_no, &angPos) // copies the motor angular position into angPos

- read motor angular velocity
  > .readVel(motor_no, &angVel) // copies the motor ang vel into angVel