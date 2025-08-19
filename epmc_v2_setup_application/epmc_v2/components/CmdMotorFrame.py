import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *

from epmc_v2.globalParams import g


class CmdMotorFrame(tb.LabelFrame):
  def __init__(self, parentFrame, motorNo):
    super().__init__(master=parentFrame, borderwidth=5, bootstyle='secondary')

    self.motorNo = motorNo
    self.frame = tb.Frame(self)

    # create widgets
    self.text = tb.Label(self.frame, text="MANUAL MOTOR CONTROL", font=('Monospace',12, 'bold') ,bootstyle="danger")
      
    buttonStyle = tb.Style()
    buttonStyleName = 'primary.TButton'
    buttonStyle.configure(buttonStyleName, font=('Monospace',12,'bold'))
    self.leftButton = tb.Button(self.frame, text="<|", style=buttonStyleName)
    self.rightButton = tb.Button(self.frame, text="|>", style=buttonStyleName)

    # Bind the Mouse events with the Handler
    self.leftButton.bind('<ButtonPress-1>', self.startForwardCommand)
    self.leftButton.bind('<ButtonRelease-1>', self.stopForwardCommand)

    self.rightButton.bind('<ButtonPress-1>', self.startReverseCommand)
    self.rightButton.bind('<ButtonRelease-1>', self.stopReverseCommand)

    # add widgets to Frames
    self.text.pack(side='left', fill='both', padx=(80,10))
    self.leftButton.pack(side='left', padx=(10,2))
    self.rightButton.pack(side='left', padx=(2,10))


    self.frame.pack(side='left', expand=True, fill='both')

  def startForwardCommand(self, e):
    # print("start forward command")
    if int(g.motorDirConfig[self.motorNo]) == 1:
      g.epmcV2.writePWM(self.motorNo, g.motorTestPwm[self.motorNo])
    elif int(g.motorDirConfig[self.motorNo]) == -1:
      g.epmcV2.writePWM(self.motorNo, -g.motorTestPwm[self.motorNo])

  def stopForwardCommand(self, e):
    # print("stop forward command")
    g.epmcV2.writePWM(self.motorNo, 0)

  def startReverseCommand(self, e):
    # print("start reverse command")
    if int(g.motorDirConfig[self.motorNo]) == 1:
      g.epmcV2.writePWM(self.motorNo, -g.motorTestPwm[self.motorNo])
    elif int(g.motorDirConfig[self.motorNo]) == -1:
      g.epmcV2.writePWM(self.motorNo, g.motorTestPwm[self.motorNo])

  def stopReverseCommand(self, e):
    # print("stop reverse command")
    g.epmcV2.writePWM(self.motorNo, 0)
