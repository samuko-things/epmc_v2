import time
from math import sin, pi

class g():
  dirConfigTextList = ['left wheel', 'right wheel']
  durationList = [5,10, 15, 20] # in sec
  signalList = ["step", "square", "sine"]

  app = None
  epmcV2 = None
  port = "None"

  i2cAddress = None

  motorTestPwm = [0, 0, 0, 0] 
  motorTestDuration = [durationList[1], durationList[1], durationList[1], durationList[1]]
  
  motorInitialTheta = [-90, -90, -90, -90]
  motorTheta = [0.0, 0.0, 0.0, 0.0]

  motorPPR = [1000.0, 1000.0, 1000.0, 1000.0]
  motorDirConfig = [1, 1, 1, 1]
  motorDirConfigText = [dirConfigTextList[0], dirConfigTextList[0], dirConfigTextList[0], dirConfigTextList[0]]

  motorStartTime = [time.time(), time.time(), time.time(), time.time()]
  motorIsOn = [False, False, False, False]

  motorAngPos = [0.0, 0.0, 0.0, 0.0]
  motorAngVel = [0.0, 0.0, 0.0, 0.0]


  motorKp = [0.0, 0.0, 0.0, 0.0]
  motorKi = [0.0, 0.0, 0.0, 0.0]
  motorKd = [0.0, 0.0, 0.0, 0.0]
  motorCf = [0.0, 0.0, 0.0, 0.0]

  motorMaxVel = [10.0, 10.0, 10.0, 10.0]
  motorTargetMaxVel = [0.0, 0.0, 0.0, 0.0]
  motorTestSignal = [signalList[0], signalList[0], signalList[0], signalList[0]]

  motorTargetVel = [0.0, 0.0, 0.0, 0.0]
  motorActualVel = [0.0, 0.0, 0.0, 0.0]
  #######################################################






###################################################################

def stepSignal(targetMax, deltaT, duration):
  if (deltaT>(2/10*duration)):
     targetCtrl = targetMax
  else:
     targetCtrl = 0              
  return targetCtrl

def squareSignal(targetMax, deltaT, duration):
  if (deltaT>(1/10*duration)) and (deltaT < (4.5/10*duration)):
     targetCtrl = targetMax
  elif (deltaT>(5.5/10*duration)) and (deltaT < (9/10*duration)):
     targetCtrl = -1*targetMax
  else:
     targetCtrl = 0              
  return targetCtrl

def sineSignal(targetMax, deltaT, duration):
  targetCtrl = targetMax * sin(2*pi*(deltaT/duration))
  return targetCtrl

# def squareSignal(targetMax, deltaT, duration):
#     ramp_time = 0.5  # seconds for ramp up/down
#     t = deltaT % duration  # wrap within period
    
#     # Define high and low phases (excluding ramps)
#     high_start = 1/10 * duration
#     high_end   = 4.5/10 * duration
#     low_start  = 5.5/10 * duration
#     low_end    = 9/10 * duration

#     if high_start <= t <= high_end:
#         # Ramp up at start of high phase
#         if t - high_start < ramp_time:
#             targetCtrl = targetMax * ((t - high_start) / ramp_time)
#         # Ramp down at end of high phase
#         elif high_end - t < ramp_time:
#             targetCtrl = targetMax * ((high_end - t) / ramp_time)
#         # Steady high
#         else:
#             targetCtrl = targetMax

#     elif low_start <= t <= low_end:
#         # Ramp down into negative
#         if t - low_start < ramp_time:
#             targetCtrl = -targetMax * ((t - low_start) / ramp_time)
#         # Ramp up toward zero from negative
#         elif low_end - t < ramp_time:
#             targetCtrl = -targetMax * ((low_end - t) / ramp_time)
#         # Steady negative
#         else:
#             targetCtrl = -targetMax

#     else:
#         targetCtrl = 0.0

#     return targetCtrl


def selectSignal(type, targetMax, deltaT, duration):
  if type == g.signalList[0]:
    targetCtrl = stepSignal(targetMax, deltaT, duration)
  elif type == g.signalList[1]:
    targetCtrl = squareSignal(targetMax, deltaT, duration)
  elif type == g.signalList[2]:
    targetCtrl = triangleSignal(targetMax, deltaT, duration)
  else:
    targetCtrl = 0.0
  
  return targetCtrl


def triangleSignal(targetMax, deltaT, duration):
    # Normalized time in range [0,1)
    t = (deltaT / duration) % 1.0
    
    # Triangle wave goes 0 → +max → 0 → -max → 0
    if t < 0.25:  
        targetCtrl = 4 * targetMax * t
    elif t < 0.75:  
        targetCtrl = 2 * targetMax - 4 * targetMax * t
    else:  
        targetCtrl = -4 * targetMax + 4 * targetMax * t
    return targetCtrl


# def triangleSignal(targetMax, deltaT, duration):
#     # Normalized time [0,1)
#     t = (deltaT / duration) % 1.0
    
#     # Map t into a triangle wave between -1 and 1
#     tri = 4 * t if t < 0.5 else 4 * (1 - t)  # goes 0 → 2 → 0
#     tri -= 1  # shift to range [-1, 1]
    
#     targetCtrl = targetMax * tri
#     return targetCtrl

##############################################################################