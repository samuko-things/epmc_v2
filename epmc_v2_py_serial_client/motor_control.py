import time
from epmc_v2 import EPMC_V2

port = '/dev/ttyUSB0'
epmcV2 = EPMC_V2(port)

if __name__ == '__main__':
  
  time.sleep(2.0)

  # left wheels (motor 0 and motor 2)
  epmcV2.writeSpeed(0, 0.00)
  epmcV2.writeSpeed(2, 0.00)

  # right wheels (motor 1 and motor 3)
  epmcV2.writeSpeed(1, 0.00)
  epmcV2.writeSpeed(3, 0.00)

  # epmcV2.setCmdTimeout(0)
  # timeout = epmcV2.getCmdTimeout()
  # print("command timeout in ms: ", timeout)

  lowTargetVel = 0.00 # in rad/sec
  highTargetVel = 3.142 # in rad/sec

  prevTime = None
  sampleTime = 0.05

  ctrlPrevTime = None
  ctrlSampleTime = 4.0
  sendHigh = True


  # left wheels (motor 0 and motor 2)
  epmcV2.writeSpeed(0, lowTargetVel)
  epmcV2.writeSpeed(2, lowTargetVel)

  # right wheels (motor 1 and motor 3)
  epmcV2.writeSpeed(1, lowTargetVel)
  epmcV2.writeSpeed(3, lowTargetVel)

  sendHigh = True

  prevTime = time.time()
  ctrlPrevTime = time.time()
  
  while True:
    if time.time() - ctrlPrevTime > ctrlSampleTime:
      if sendHigh:
        # left wheels (motor 0 and motor 2)
        epmcV2.writeSpeed(0, highTargetVel)
        epmcV2.writeSpeed(2, highTargetVel)

        # right wheels (motor 1 and motor 3)
        epmcV2.writeSpeed(1, highTargetVel)
        epmcV2.writeSpeed(3, highTargetVel)

        sendHigh = False
      else:
        # left wheels (motor 0 and motor 2)
        epmcV2.writeSpeed(0, lowTargetVel)
        epmcV2.writeSpeed(2, lowTargetVel)

        # right wheels (motor 1 and motor 3)
        epmcV2.writeSpeed(1, lowTargetVel)
        epmcV2.writeSpeed(3, lowTargetVel)

        sendHigh = True
      
      ctrlPrevTime = time.time()



    if time.time() - prevTime > sampleTime:
      try:
        # left wheels (motor 0 and motor 2)
        angPos0 = epmcV2.readPos(0)
        angVel0 = epmcV2.readVel(0)

        angPos2 = epmcV2.readPos(2)
        angVel2 = epmcV2.readVel(2)

        # right wheels (motor 1 and motor 3)
        angPos1 = epmcV2.readPos(1)
        angVel1 = epmcV2.readVel(1)

        angPos3 = epmcV2.readPos(3)
        angVel3 = epmcV2.readVel(3)
        
        print("-----------------------------------------")
        print("left wheels - motor 0 and motor 2")
        print(f"motor0_readings: [{angPos0}, {angVel0}]")
        print(f"motor2_readings: [{angPos2}, {angVel2}]\n")

        print("right wheels - motor 1 and motor 3")
        print(f"motor1_readings: [{angPos1}, {angVel1}]")
        print(f"motor3_readings: [{angPos3}, {angVel3}]")
        print("-----------------------------------------\n")

      except:
        pass
      
      prevTime = time.time()