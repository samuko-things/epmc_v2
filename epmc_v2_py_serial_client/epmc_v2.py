import serial
import time

class EPMC_V2:
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
        elif len(data)==3:
            return float(data[0]), float(data[1]), float(data[2])
        elif len(data)==4:
            return float(data[0]), float(data[1]), float(data[2]), float(data[3])
        
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
    
    def readRPY(self):
        r, p, y = self.get("/rpy", -1)
        return r, p, y
    
    def readAcc(self):
        ax, ay, az = self.get("/acc", -1)
        return ax, ay, az
    
    def readGyro(self):
        gx, gy, gz = self.get("/gyro", -1)
        return gx, gy, gz
    
    def readQuat(self):
        qw, qx, qy, qz = self.get("/quat", -1)
        return qw, qx, qy, qz