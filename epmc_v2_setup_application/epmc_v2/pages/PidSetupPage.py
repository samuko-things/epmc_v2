import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *

from epmc_v2.globalParams import g

from epmc_v2.components.SetValueFrame import SetValueFrame
from epmc_v2.components.SelectValueFrame import SelectValueFrame
from epmc_v2.components.GraphFrame import GraphFrame




class PidSetupFrame(tb.Frame):
  def __init__(self, parentFrame, motorNo):
    super().__init__(master=parentFrame)

    self.motorNo = motorNo

    self.label = tb.Label(self, text=f"MOTOR {self.motorNo} PID SETUP", font=('Monospace',16, 'bold') ,bootstyle="dark")

    self.frame1 = tb.Frame(self)
    self.frame2 = tb.Frame(self)

    # configure grid for frame1
    self.frame1.grid_columnconfigure((0,1,2,3), weight=1, uniform='a')
    self.frame1.grid_rowconfigure((0,1), weight=1, uniform='a')


    #create widgets to be added to frame1
    g.motorKp[self.motorNo] = g.epmcV2.getKp(self.motorNo)
    self.setKp = SetValueFrame(self.frame1, keyTextInit=f"*KP: ", valTextInit=g.motorKp[self.motorNo],
                               middleware_func=self.setKpFunc)

    g.motorKi[self.motorNo] = g.epmcV2.getKi(self.motorNo)
    self.setKi = SetValueFrame(self.frame1, keyTextInit=f"*KI: ", valTextInit=g.motorKi[self.motorNo],
                               middleware_func=self.setKiFunc)

    g.motorKd[self.motorNo] = g.epmcV2.getKd(self.motorNo)
    self.setKd = SetValueFrame(self.frame1, keyTextInit=f"*KD: ", valTextInit=g.motorKd[self.motorNo],
                               middleware_func=self.setKdFunc)

    g.motorCf[self.motorNo] = g.epmcV2.getCutOffFreq(self.motorNo)
    self.setCf = SetValueFrame(self.frame1, keyTextInit=f"*CF(Hz): ", valTextInit=g.motorCf[self.motorNo],
                               middleware_func=self.setCfFunc)
    

    g.motorMaxVel[self.motorNo] = g.epmcV2.getMaxVel(self.motorNo)
    self.setMaxVel = SetValueFrame(self.frame1, keyTextInit=f"*W_MAX(rad/s): ", valTextInit=g.motorMaxVel[self.motorNo],
                                   middleware_func=self.setMaxVelFunc)
    
    self.setTargetVel = SetValueFrame(self.frame1, keyTextInit="TARGET(rad/s): ", valTextInit=g.motorTargetMaxVel[self.motorNo],
                                    middleware_func=self.setTargetVelFunc)
    
    self.selectSignal = SelectValueFrame(self.frame1, keyTextInit="TEST_SIGNAL: ", valTextInit=g.motorTestSignal[self.motorNo],
                                           initialComboValues=g.signalList, middileware_func=self.selectSignalFunc)
    
    self.selectDuration = SelectValueFrame(self.frame1, keyTextInit="DURATION(sec): ", valTextInit=g.motorTestDuration[self.motorNo],
                                           initialComboValues=g.durationList)


    #add framed widgets to frame1
    self.setKp.grid(row=0, column=0, sticky='nsew', padx=5, pady=(0,10))
    self.setKi.grid(row=0, column=1, sticky='nsew', padx=5, pady=(0,10))
    self.setKd.grid(row=0, column=2, sticky='nsew', padx=5, pady=(0,10))
    self.setCf.grid(row=0, column=3, sticky='nsew', padx=5, pady=(0,10))
  
    self.setMaxVel.grid(row=1, column=0, sticky='nsew', padx=5)
    self.setTargetVel.grid(row=1, column=1, sticky='nsew', padx=5)
    self.selectSignal.grid(row=1, column=2, sticky='nsew', padx=5)
    self.selectDuration.grid(row=1, column=3, sticky='nsew', padx=5)


    #create widgets to be added to frame2
    self.graph = GraphFrame(self.frame2, motorNo=self.motorNo)

    #add framed widgets to frame2
    self.graph.pack(side="left", expand=True, fill="both", padx=5)


    #add frame1, frame2 and frame3 to MainFrame
    self.label.pack(side="top", fill="x", padx=(200,0), pady=(5,0))
    self.frame1.pack(side="top", expand=True, fill="x")
    self.frame2.pack(side="top", expand=True, fill="both", pady=(20, 0))

 

  def setKpFunc(self, kp_val_str):
    try:
      if kp_val_str:
        isSuccessful = g.epmcV2.setKp(self.motorNo, float(kp_val_str))
        val = g.epmcV2.getKp(self.motorNo)
        g.motorKp[self.motorNo] = val
    except:
      pass

    return g.motorKp[self.motorNo]
  

  def setKiFunc(self, ki_val_str):
    try:
      if ki_val_str:
        isSuccessful = g.epmcV2.setKi(self.motorNo, float(ki_val_str))
        val = g.epmcV2.getKi(self.motorNo)
        g.motorKi[self.motorNo] = val
    except:
      pass

    return g.motorKi[self.motorNo]
  

  def setKdFunc(self, kd_val_str):
    try:
      if kd_val_str:
        isSuccessful = g.epmcV2.setKd(self.motorNo, float(kd_val_str))
        val = g.epmcV2.getKd(self.motorNo)
        g.motorKd[self.motorNo] = val
    except:
      pass

    return g.motorKd[self.motorNo]
  

  def setCfFunc(self, cf_val_str):
    try:
      if cf_val_str:
        isSuccessful = g.epmcV2.setCutOffFreq(self.motorNo, float(cf_val_str))
        val = g.epmcV2.getCutOffFreq(self.motorNo)
        g.motorCf[self.motorNo] = val
    except:
      pass

    return g.motorCf[self.motorNo]

  
  def setMaxVelFunc(self, vel_val_str):
    try:
      if vel_val_str:
        isSuccessful = g.epmcV2.setMaxVel(self.motorNo, float(vel_val_str))
        val = g.epmcV2.getMaxVel(self.motorNo)
        g.motorMaxVel[self.motorNo] = val
    except:
      pass

    return g.motorMaxVel[self.motorNo]
  

  def setTargetVelFunc(self, vel_val_str):
    try:
      if vel_val_str:
        g.motorTargetMaxVel[self.motorNo] = float(vel_val_str)
    except:
      pass

    return g.motorTargetMaxVel[self.motorNo]
  

  def selectSignalFunc(self, signal_val_str):
    try:
      if signal_val_str:
        g.motorTestSignal[self.motorNo] = signal_val_str
    except:
      pass

    return g.motorTestSignal[self.motorNo]
  

  # def selectDurationFunc(self, duration_val_str):
  #   try:
  #     if duration_val_str:
  #       val = int(duration_val_str)
  #       g.motorTestDuration[self.motorNo] = val
  #   except:
  #     pass

  #   return g.motorTestDuration[self.motorNo]