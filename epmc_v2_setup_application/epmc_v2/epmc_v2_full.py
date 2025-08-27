import serial
import time

class EPMC_V2_FULL:
    def __init__(self, port, baud=115200, timeOut=0.1):
        self.ser = serial.Serial(port, baud, timeout=timeOut)
    
    def send_and_receive(self, msg_cmd):
        data = ""
        prev_time = time.time()
        while data=="":
            try:
                self.ser.write(msg_cmd.encode())   # send a single or multiple byte    
                data = self.ser.readline().decode().strip()
                if time.time()-prev_time > 2.0:
                    raise Exception("[Timeout] No response from ESP32")
            except:
                print("[Timeout] No response from ESP32")
        return data
    
    def send(self, cmd_route, motor_no, val):
        cmd_str = cmd_route+","+str(motor_no)+","+str(val)
        data = self.send_and_receive(cmd_str)
        if data == "1":
            return True
        else:
            return False
  
    def get(self, cmd_route, motor_no):
        cmd_str = cmd_route+","+str(motor_no)
        data = self.send_and_receive(cmd_str).split(',')
        if len(data)==1:
            return float(data[0])
        elif len(data)==2:
            return float(data[0]), float(data[1])
    
    ####################################################################

    def readPos(self, motor_no):
        pos = self.get("/pos", motor_no)
        return pos
    
    def readVel(self, motor_no):
        filteredVel, _ = self.get("/vel", motor_no)
        return filteredVel
    
    def readVelFull(self, motor_no):
        filteredVel, unfilteredVel = self.get("/vel", motor_no)
        return filteredVel, unfilteredVel
    
    def writePWM(self, motor_no, pwm):
        res = self.send("/pwm", motor_no, pwm)
        return res
    
    def writeSpeed(self, motor_no, speed):
        res = self.send("/vel", motor_no, speed)
        return res
    
    def setCmdTimeout(self, timeout):
        res = self.send("/timeout", -1, timeout)
        return res
    
    def getCmdTimeout(self):
        timeout = self.get("/timeout", -1)
        return timeout
    
    def setPidMode(self, motor_no, mode):
        res = self.send("/mode", motor_no, mode)
        return res
    
    def getPidMode(self, motor_no):
        mode = self.get("/mode", motor_no)
        return mode
    
    #####################################################

    def readPidVel(self, motor_no):
        targetVel, actualVel = self.get("/pvel", motor_no)
        return targetVel, actualVel
    
    def setKp(self, motor_no, kp):
        res = self.send("/kp", motor_no, kp)
        return res
    
    def getKp(self, motor_no):
        kp = self.get("/kp", motor_no)
        return kp
    
    def setKi(self, motor_no, ki):
        res = self.send("/ki", motor_no, ki)
        return res
    
    def getKi(self, motor_no):
        ki = self.get("/ki", motor_no)
        return ki
    
    def setKd(self, motor_no, kd):
        res = self.send("/kd", motor_no, kd)
        return res
    
    def getKd(self, motor_no):
        kd = self.get("/kd", motor_no)
        return kd
    
    def setPPR(self, motor_no, ppr):
        res = self.send("/ppr", motor_no, ppr)
        return res
    
    def getPPR(self, motor_no):
        ppr = self.get("/ppr", motor_no)
        return ppr
    
    def setRdir(self, motor_no, rdir):
        res = self.send("/rdir", motor_no, rdir)
        return res
    
    def getRdir(self, motor_no):
        rdir = self.get("/rdir", motor_no)
        return rdir
    
    def setCutOffFreq(self, motor_no, cutOffFreq):
        res = self.send("/cut-freq", motor_no, cutOffFreq)
        return res
    
    def getCutOffFreq(self, motor_no):
        cutOffFreq = self.get("/cut-freq", motor_no)
        return cutOffFreq
    
    def setMaxVel(self, motor_no, maxVel):
        res = self.send("/max-vel", motor_no, maxVel)
        return res
    
    def getMaxVel(self, motor_no):
        maxVel = self.get("/max-vel", motor_no)
        return maxVel
    
    def setI2cAddress(self, i2cAddress):
        res = self.send("/i2c", -1, i2cAddress)
        return res
    
    def getI2cAddress(self):
        i2cAddress = self.get("/i2c", -1)
        return i2cAddress
    
    def resetAllParams(self):
        res = self.send("/reset", -1, -1)
        return res

    ###################################################

    def setUseIMU(self, val):
        res = self.send("/use-imu", -1, val)
        return res
    
    def getUseIMU(self):
        val = self.get("/use-imu", -1)
        return val
    
    def readAcc(self, pos_no):
        val = self.get("/acc", pos_no)
        return val
    
    def readAccRaw(self, pos_no):
        val = self.get("/acc-raw", pos_no)
        return val
    
    def readAccOffset(self, pos_no):
        val = self.get("/acc-off", pos_no)
        return val
    
    def writeAccOffset(self, pos_no, ax_off):
        res = self.send("/acc-off", pos_no, ax_off)
        return res
    
    def readAccVariance(self, pos_no):
        val = self.get("/acc-var", pos_no)
        return val
    
    def writeAccVariance(self, pos_no, val):
        res = self.send("/acc-var", pos_no, val)
        return res
    
    def readGyro(self, pos_no):
        val = self.get("/gyro", pos_no)
        return val
    
    def readGyroRaw(self, pos_no):
        val = self.get("/gyro-raw", pos_no)
        return val
    
    def readGyroOffset(self, pos_no):
        val = self.get("/gyro-off", pos_no)
        return val
    
    def writeGyroOffset(self, pos_no, ax_off):
        res = self.send("/gyro-off", pos_no, ax_off)
        return res
    
    def readGyroVariance(self, pos_no):
        val = self.get("/gyro-var", pos_no)
        return val
    
    def writeGyroVariance(self, pos_no, val):
        res = self.send("/gyro-var", pos_no, val)
        return res 
    
    #####################################################