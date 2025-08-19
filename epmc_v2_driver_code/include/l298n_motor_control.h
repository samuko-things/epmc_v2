#ifndef L298N_MOTOR_CONTROL_H
#define L298N_MOTOR_CONTROL_H
#include <Arduino.h>

class L298NMotorControl {
  public:
    L298NMotorControl(int IN1_pin, int IN2_pin, int en_pin);

    void sendPWM(int pwmVal);
    int getDirection();
    void test();

  private:
    int in1Pin, in2Pin, enPin;
    int dir = 1;

    void setForwardDirection();
    void setReverseDirection();
    void setHalt();

};




L298NMotorControl::L298NMotorControl(int IN1_pin, int IN2_pin, int en_pin)
{
  in1Pin = IN1_pin;
  in2Pin = IN2_pin;
  enPin = en_pin;

  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enPin, OUTPUT);

  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
}

void L298NMotorControl::sendPWM(int pwmVal)
{
  if (pwmVal > 0)
  {
    analogWrite(enPin, abs(pwmVal));
    setForwardDirection();
  }
  else if (pwmVal < 0)
  {
    analogWrite(enPin, abs(pwmVal));
    setReverseDirection();
  }
  else
  {
    analogWrite(enPin, 0);
    setHalt();
  }
}

int L298NMotorControl::getDirection()
{
  return dir;
}

void L298NMotorControl::setForwardDirection()
{
  dir = 1;
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
}

void L298NMotorControl::setReverseDirection()
{
  dir = 0;
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
}

void L298NMotorControl::setHalt()
{
  dir = 0;
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
}


#endif