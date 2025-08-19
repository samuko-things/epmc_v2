import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *

from epmc_v2.globalParams import g

from epmc_v2.components.SetValueFrame import SetValueFrame
from epmc_v2.components.SelectValueFrame import SelectValueFrame
from epmc_v2.components.MotorDrawFrame import MotorDrawFrame
from epmc_v2.components.CmdMotorFrame import CmdMotorFrame





class EncSetupFrame(tb.Frame):
  def __init__(self, parentFrame, motorNo):
    super().__init__(master=parentFrame)

    self.motorNo = motorNo

    self.label = tb.Label(self, text=f"MOTOR {self.motorNo} ENCODER SETUP", font=('Monospace',16, 'bold') ,bootstyle="dark")

    self.frame1 = tb.Frame(self)
    self.frame2 = tb.Frame(self)

    # configure grid for frame1
    self.frame1.grid_columnconfigure((0,1,2,3), weight=1, uniform='a')

    #create widgets to be added to frame1
    g.motorPPR[self.motorNo] = g.epmcV2.getPPR(self.motorNo)
    self.setPulsePerRev = SetValueFrame(self.frame1, keyTextInit=f"*PPR: ", valTextInit=g.motorPPR[self.motorNo],
                                        middleware_func=self.setPulsePerRevFunc)
    
    self.initDirConfigA()
    self.selectDirConfig = SelectValueFrame(self.frame1, keyTextInit=f"*DIR: ", valTextInit=g.motorDirConfigText[self.motorNo],
                                            initialComboValues=g.dirConfigTextList, middileware_func=self.selectDirConfigFunc)
    
    self.setTestPwm = SetValueFrame(self.frame1, keyTextInit="TEST_PWM: ", valTextInit=g.motorTestPwm[self.motorNo],
                                    middleware_func=self.setTestPwmFunc)
    
    self.selectDuration = SelectValueFrame(self.frame1, keyTextInit="DURATION(sec): ", valTextInit=g.motorTestDuration[self.motorNo],
                                           initialComboValues=g.durationList, middileware_func=self.selectDurationFunc)

    #add framed widgets to frame1
    self.setPulsePerRev.grid(row=0, column=0, sticky='nsew', padx=5)
    self.selectDirConfig.grid(row=0, column=1, sticky='nsew', padx=5)
    self.setTestPwm.grid(row=0, column=2, sticky='nsew', padx=5)
    self.selectDuration.grid(row=0, column=3, sticky='nsew', padx=5)

    #create widgets to be added to frame2
    self.cmdMotor = CmdMotorFrame(self.frame2, motorNo=self.motorNo)
    self.drawMotor = MotorDrawFrame(self.frame2, motorNo=self.motorNo)

    #add framed widgets to frame2
    self.cmdMotor.pack(side="top", fill="x", padx=(100,100), pady=(0,20))
    self.drawMotor.pack(side="left", expand=True, fill="both", padx=5)


    #add frame1, frame2 and frame3 to MainFrame
    self.label.pack(side="top", fill="x", padx=(200,0), pady=(5,0))
    self.frame1.pack(side="top", expand=True, fill="x")
    self.frame2.pack(side="top", expand=True, fill="both", pady=(10, 0))




  def setTestPwmFunc(self, pwm_val_str):
    try:
      if pwm_val_str:
        val = int(pwm_val_str)
        if val > 255:
          g.motorTestPwm[self.motorNo] = 255
        elif val < -255:
          g.motorTestPwm[self.motorNo] = -255
        else:
          g.motorTestPwm[self.motorNo] = val
    except:
      pass

    return g.motorTestPwm[self.motorNo]
    


  def setPulsePerRevFunc(self, ppr_val_str):
    try:
      if ppr_val_str:
        val = float(ppr_val_str)
        isSuccessful = g.epmcV2.setPPR(self.motorNo, val)
        val = g.epmcV2.getPPR(self.motorNo)
        g.motorPPR[self.motorNo] = val
    except:
      pass

    return g.motorPPR[self.motorNo]

  

  def selectDurationFunc(self, duration_val_str):
    try:
      if duration_val_str:
        val = int(duration_val_str)
        g.motorTestDuration[self.motorNo] = val
    except:
      pass

    return g.motorTestDuration[self.motorNo]
  


  def initDirConfigA(self):
    try:
      g.motorDirConfig[self.motorNo] = g.epmcV2.getRdir(self.motorNo)
      if int(g.motorDirConfig[self.motorNo]) == 1:
        g.motorDirConfigText[self.motorNo] = g.dirConfigTextList[0]
      elif int(g.motorDirConfig[self.motorNo]) == -1:
        g.motorDirConfigText[self.motorNo] = g.dirConfigTextList[1]
      self.resetInitialTheta()
    except:
      pass



  def selectDirConfigFunc(self, dir_val_str):
    try:
      if dir_val_str:
        g.motorDirConfigText[self.motorNo] = dir_val_str

        if g.motorDirConfigText[self.motorNo] == g.dirConfigTextList[0]:
          isSuccessful = g.epmcV2.setRdir(self.motorNo, 1.00)
          g.motorDirConfig[self.motorNo] = g.epmcV2.getRdir(self.motorNo)
          g.motorInitialTheta[self.motorNo] = -1*g.motorTheta[self.motorNo] - 90
          
        elif g.motorDirConfigText[self.motorNo] == g.dirConfigTextList[1]:
          isSuccessful = g.epmcV2.setRdir(self.motorNo, -1.00)
          g.motorDirConfig[self.motorNo] = g.epmcV2.getRdir(self.motorNo)
          g.motorInitialTheta[self.motorNo] = -1*g.motorTheta[self.motorNo] + 90
        
        

    except:
      pass

    return g.motorDirConfigText[self.motorNo]



  def resetInitialTheta(self):
    if int(g.motorDirConfig[self.motorNo]) == 1:
      g.motorInitialTheta[self.motorNo] = g.motorTheta[self.motorNo] - 90
    elif int(g.motorDirConfig[self.motorNo]) == -1:
      g.motorInitialTheta[self.motorNo] = g.motorTheta[self.motorNo] + 90
