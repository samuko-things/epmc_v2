import time
from epmc_v2 import EPMC_V2

port = '/dev/ttyUSB0'
epmcV2 = EPMC_V2(port)

if __name__ == '__main__':
  
  time.sleep(2.0)

  epmcV2.writeSpeed(0, 0.00)
  epmcV2.writeSpeed(1, 0.00)

  epmcV2.setCmdTimeout(4000)
  timeout = epmcV2.getCmdTimeout()
  print("command timeout in ms: ", timeout)

  lowTargetVel = -3.142 # in rad/sec
  highTargetVel = 3.142 # in rad/sec

  prevTime = None
  sampleTime = 0.05

  ctrlPrevTime = None
  ctrlSampleTime = 5.0
  sendHigh = True


  epmcV2.writeSpeed(0, lowTargetVel)
  epmcV2.writeSpeed(1, lowTargetVel)
  sendHigh = True

  prevTime = time.time()
  ctrlPrevTime = time.time()
  
  while True:
    if time.time() - ctrlPrevTime > ctrlSampleTime:
      if sendHigh:
        epmcV2.writeSpeed(0, highTargetVel)
        epmcV2.writeSpeed(1, highTargetVel)
        sendHigh = False
      else:
        epmcV2.writeSpeed(0, lowTargetVel)
        epmcV2.writeSpeed(1, lowTargetVel)
        sendHigh = True
      
      ctrlPrevTime = time.time()



    if time.time() - prevTime > sampleTime:
      try:
        angPos0 = epmcV2.readPos(0)
        angVel0 = epmcV2.readVel(0)

        angPos1 = epmcV2.readPos(1)
        angVel1 = epmcV2.readVel(1)
        
        print(f"motor0_readings: [{angPos0}, {angVel0}]")
        print(f"motor1_readings: [{angPos1}, {angVel1}]")
        print("")
      except:
        pass
      
      prevTime = time.time()