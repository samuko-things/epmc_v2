import time
from epmc_v2 import EPMC_V2

port = '/dev/ttyUSB0'
epmcV2 = EPMC_V2(port)

def main():
  for i in range(2):
    time.sleep(1.0)
    print(i+1, " sec")

  # # change the reference frame to ENU frame (0 - NWU,  1 - ENU,  2 - NED)
  # epmcV2.setRefFrame(1)

  # # check the reference frame the epmcV2 is working in (0 - NWU,  1 - ENU,  2 - NED)
  # ref_frame_id = epmcV2.getRefFrame()

  # if ref_frame_id == 0:
  #   print("Reference Frame is North-West-Up (NWU)")
  # elif ref_frame_id == 1:
  #   print("Reference Frame is East-North-Up (ENU)")
  # elif ref_frame_id == 2:
  #   print("Reference Frame is North-East-Down (NED)")

  prevTime = time.time()
  sampleTime = 0.01

  while True:
    if time.time() - prevTime > sampleTime:
      try:
        roll = epmcV2.readRPY(0)
        pitch = epmcV2.readRPY(1)
        yaw = epmcV2.readRPY(2)
        print(f"roll: {roll}\tpitch: {pitch}\tyaw: {yaw}\n")
      except:
        pass
      
      prevTime = time.time()

if __name__ == "__main__":
  main()