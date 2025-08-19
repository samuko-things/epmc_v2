import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *

from math import sin, cos, radians, pi
import time

from epmc_v2.globalParams import g




class MotorDrawFrame(tb.Frame):
  def __init__(self, parentFrame, motorNo):
    super().__init__(master=parentFrame)

    self.motorNo = motorNo

    self.displayFrame = tb.Frame(self)
    self.canvasFrame = tb.Frame(self)

    self.textFrame1 = tb.Frame(self.displayFrame)
    self.textFrame2 = tb.Frame(self.displayFrame)
    

    #create widgets to be added to the textFame
    buttonStyle = tb.Style()
    buttonStyleName = 'danger.TButton'
    buttonStyle.configure(buttonStyleName, font=('Monospace',10, 'bold'))

    g.motorAngPos[self.motorNo] = g.epmcV2.readPos(self.motorNo)
    g.motorAngVel[self.motorNo] = g.epmcV2.readVel(self.motorNo)

    self.posText = tb.Label(self.textFrame1, text="POS(rad):", font=('Monospace',10, 'bold') ,bootstyle="danger")
    self.posVal = tb.Label(self.textFrame1, text=g.motorAngPos[self.motorNo], font=('Monospace',10), bootstyle="dark")

    self.velText = tb.Label(self.textFrame2, text="VEL(rad/s):", font=('Monospace',10, 'bold') ,bootstyle="primary")
    self.velVal = tb.Label(self.textFrame2, text=g.motorAngVel[self.motorNo], font=('Monospace',10), bootstyle="dark")

    self.button1 = tb.Button(self.displayFrame, text="RESET HAND", style=buttonStyleName,
                             command=self.resetInitialTheta)
    self.button2 = tb.Button(self.displayFrame, text="START MOTOR", style=buttonStyleName,
                             command=self.sendPwmCtrl)

    #add created widgets to displayFrame
    self.posText.pack(side='left', fill='both')
    self.posVal.pack(side='left', expand=True, fill='both')

    self.velText.pack(side='left', fill='both')
    self.velVal.pack(side='left', expand=True, fill='both')

    self.textFrame1.pack(side='left', expand=True, fill='both')
    self.textFrame2.pack(side='left', expand=True, fill='both')

    self.button1.pack(side='left', fill='both', padx=(0,10))
    self.button2.pack(side='left', fill='both')

    #create widgets to be added to the canvasFame
    self.canvas = tb.Canvas(self.canvasFrame, width=300, height=500,autostyle=False ,bg="#FFFFFF", relief='solid')

    #add created widgets to canvasFame
    self.canvas.pack(side='left', expand=True, fill='both')

    # initialize canvas with motor representation shape
    x = 120
    self.r = 170 # circle radius
    self.m = (320, 180) #x, y primary "#4582EC" danger "#D9534F"
    self.circle = self.canvas.create_oval(152, 12, 488, 348, outline="#ADB5BD", width=5)
    self.line = self.canvas.create_line(self.m[0], self.m[1], 
                                  self.m[0]+self.r*cos(radians(-1*(g.motorTheta[self.motorNo]-g.motorInitialTheta[self.motorNo]))), 
                                  self.m[1]+self.r*sin(radians(-1*(g.motorTheta[self.motorNo]-g.motorInitialTheta[self.motorNo]))), 
                                  fill='#4582EC',width=10)
    self.mid_circle = self.canvas.create_oval(300, 160, 340, 200, fill="#D9534F", outline="#4582EC", width=7)


    # add displayFrame and canvasFrame to GraphFrame
    self.displayFrame.pack(side='top', expand=True, fill='x', padx=10)
    self.canvasFrame.pack(side='top', expand=True, fill='both', pady=(10,0))

    ############################################
    self.draw_motor_ang_pos()



  def sendPwmCtrl(self):
    if g.motorIsOn[self.motorNo]:
      isSuccess = g.epmcV2.writePWM(self.motorNo, 0)
      if isSuccess:
        g.motorIsOn[self.motorNo] = False
        self.button2.configure(text="START MOTOR")

    else:
      isSuccess = g.epmcV2.writePWM(self.motorNo, g.motorTestPwm[self.motorNo])
      if isSuccess:
        g.motorIsOn[self.motorNo] = True
        g.motorStartTime[self.motorNo] = time.time()
        self.button2.configure(text="STOP MOTOR")




  def draw_motor_ang_pos(self):
    if g.motorIsOn[self.motorNo] and g.motorTestDuration[self.motorNo] < time.time()-g.motorStartTime[self.motorNo]:
        isSuccess = g.epmcV2.writePWM(self.motorNo, 0)
        if isSuccess:
          g.motorIsOn[self.motorNo] = False
          self.button2.configure(text="START MOTOR")

    self.canvas.delete(self.line)
    self.canvas.delete(self.mid_circle)

    try:
      g.motorAngPos[self.motorNo] = g.epmcV2.readPos(self.motorNo)
      g.motorAngVel[self.motorNo] = g.epmcV2.readVel(self.motorNo)
      
    except:
      pass

    g.motorTheta[self.motorNo] = round(self.absAngDeg(g.motorAngPos[self.motorNo]),2)

    if int(g.motorDirConfig[self.motorNo]) == -1:
      self.line = self.canvas.create_line(self.m[0], self.m[1], 
                                    self.m[0]+self.r*cos(radians(g.motorTheta[self.motorNo]-g.motorInitialTheta[self.motorNo])), 
                                    self.m[1]+self.r*sin(radians(g.motorTheta[self.motorNo]-g.motorInitialTheta[self.motorNo])), 
                                    fill='#4582EC',width=10)
    elif int(g.motorDirConfig[self.motorNo]) == 1:
      self.line = self.canvas.create_line(self.m[0], self.m[1], 
                                    self.m[0]+self.r*cos(radians(-1*(g.motorTheta[self.motorNo]-g.motorInitialTheta[self.motorNo]))), 
                                    self.m[1]+self.r*sin(radians(-1*(g.motorTheta[self.motorNo]-g.motorInitialTheta[self.motorNo]))),   
                                    fill='#4582EC',width=10)
    self.mid_circle = self.canvas.create_oval(300, 160, 340, 200, fill="#D9534F", outline="#4582EC", width=7)

    self.posVal.configure(text=f"{g.motorAngPos[self.motorNo]}")
    self.velVal.configure(text=f"{g.motorAngVel[self.motorNo]}")

    self.canvas.after(1, self.draw_motor_ang_pos)


  def resetInitialTheta(self):
    if int(g.motorDirConfig[self.motorNo]) == 1:
      g.motorInitialTheta[self.motorNo] = g.motorTheta[self.motorNo] - 90
    elif int(g.motorDirConfig[self.motorNo]) == -1:
      g.motorInitialTheta[self.motorNo] = g.motorTheta[self.motorNo] + 90

  def absAngDeg(self, incAngRad):
    incAngDeg = incAngRad * 180.0 / pi
    return incAngDeg % 360.0