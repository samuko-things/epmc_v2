## Easy PID Motor Controller (EPMC) C++ Library

## How to Use the Library
- install the libserial-dev package
  > sudo apt-get update
  >
  > sudo apt install libserial-dev

- A simple way to get started is simply to try out and follow the example `motor_control.cpp` code.

- make, build and run the example code.
  > cd into the root directory
  >
  > mkdir build (i.e create a folder named build)
  >
  > enter the following command in the terminal in the root folder:
    ````
    cmake -B ./build/
    ````
    ````
    cmake --build ./build/
    ````
    ````
    ./build/motor_control
    or
    ./build/read_imu
    ````

- You can follow the pattern used in the example `motor_control.cpp` in your own code.


## Basic Library functions and usage

- connect to the **`L298N EPMC MODULE`** or a **`CUSTOM EPMC INTERFACE BOARD`**
  > .connect("port_name or port_path")

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