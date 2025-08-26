import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *

import numpy as np
from termcolor import colored

from epmc_v2.globalParams import g


class ImuVarianceFrame(tb.Frame):
  def __init__(self, parentFrame):
    super().__init__(master=parentFrame)

    # intialize parameter
    self.start_process = False
    self.loop_count = 0
    self.no_of_samples = 1000

    self.accx_arr = []
    self.accy_arr = []
    self.accz_arr = []

    self.gyrox_arr = []
    self.gyroy_arr = []
    self.gyroz_arr = []

    self.label = tb.Label(self, text="COMPUTE IMU VARIANCE", font=('Monospace',16, 'bold') ,bootstyle="dark")
  
    #create widgets to be added to the Fame
    percent = 0.0
    self.textVal = tb.Label(self, text=f'{percent} %', font=('Monospace',20, 'bold'), bootstyle="primary")
    self.progressBar = tb.Progressbar(self, bootstyle="danger striped", mode='determinate',
                                      maximum=100, length=200, value=0.0)

    buttonStyle = tb.Style()
    buttonStyleName = 'primary.TButton'
    buttonStyle.configure(buttonStyleName, font=('Monospace',15,'bold'))
    self.pressButton = tb.Button(self, text="START", style=buttonStyleName,
                                 command=self.change_btn_state)
    
    self.canvasFrame = tb.Frame(self)
    
    #add created widgets to Frame
    self.label.pack(side='top', pady=(20,50))
    self.textVal.pack(side='top', expand=True, fill='y')
    self.progressBar.pack(side='top', expand=True, fill='x', padx=50)
    self.pressButton.pack(side='top', fill='y')
    self.canvasFrame.pack(side='top', expand=True, fill='both', pady=(10,0))

    #create widgets to be added to the canvasFame
    self.canvas = tb.Canvas(self.canvasFrame, width=300, height=10, autostyle=False ,bg="#FFFFFF", relief='solid')

    #add created widgets to canvasFame
    self.canvas.pack(side='left', expand=True, fill='both')

    # start process
    self.compute_variance()

  def reset_all_params(self):
    self.loop_count = 0
    self.no_of_samples = 1000

    self.r_arr = []
    self.p_arr = []
    self.y_arr = []

    self.accx_arr = []
    self.accy_arr = []
    self.accz_arr = []

    self.gyrox_arr = []
    self.gyroy_arr = []
    self.gyroz_arr = []

    percent = 0.0
    self.textVal.configure(text=f'{percent} %')
    self.progressBar['value'] = percent

  def read_cal_data(self):
    if self.start_process:
      self.no_of_samples = 1000

      r = g.epmcV2.readRPY(0)
      p = g.epmcV2.readRPY(1)
      y = g.epmcV2.readRPY(2)

      self.r_arr.append(r)
      self.p_arr.append(p)
      self.y_arr.append(y)

      accx_cal = g.epmcV2.readAcc(0)
      accy_cal = g.epmcV2.readAcc(1)
      accz_cal = g.epmcV2.readAcc(2)

      self.accx_arr.append(accx_cal)
      self.accy_arr.append(accy_cal)
      self.accz_arr.append(accz_cal)

      gyrox_cal = g.epmcV2.readGyro(0)
      gyroy_cal = g.epmcV2.readGyro(1)
      gyroz_cal = g.epmcV2.readGyro(2)

      self.gyrox_arr.append(gyrox_cal)
      self.gyroy_arr.append(gyroy_cal)
      self.gyroz_arr.append(gyroz_cal)

      self.loop_count += 1
      percent = (self.loop_count*100)/self.no_of_samples
      self.textVal.configure(text=f'{percent} %')
      self.progressBar['value'] = percent

      if percent >= 100.0:
        percent = 100.0
        self.textVal.configure(text=f'{percent} %')
        self.progressBar['value'] = percent
        self.print_computed_variance()
      else:
        self.canvas.after(1, self.read_cal_data)

    else:
      self.reset_all_params()
      self.canvas.after(10, self.compute_variance)

  def print_computed_variance(self):
    r_variance = np.var(self.r_arr)
    p_variance = np.var(self.p_arr)
    y_variance = np.var(self.y_arr)

    g.epmcV2.writeRPYVariance(0, r_variance)
    g.epmcV2.writeRPYVariance(1, p_variance)
    g.epmcV2.writeRPYVariance(2, y_variance)

    r_variance = g.epmcV2.readRPYVariance(0)
    p_variance = g.epmcV2.readRPYVariance(1)
    y_variance = g.epmcV2.readRPYVariance(2)

    rpy_variance = [ r_variance, p_variance, y_variance]

    print(colored("\n---------------------------------------------------------------", 'magenta'))
    print(colored("stored rpy variances", 'green'))
    print(rpy_variance)
    print(colored("---------------------------------------------------------------", 'magenta'))

    accx_variance = np.var(self.accx_arr)
    accy_variance = np.var(self.accy_arr)
    accz_variance = np.var(self.accz_arr)

    g.epmcV2.writeAccVariance(0, accx_variance)
    g.epmcV2.writeAccVariance(1, accy_variance)
    g.epmcV2.writeAccVariance(2, accz_variance)

    accx_variance = g.epmcV2.readAccVariance(0)
    accy_variance = g.epmcV2.readAccVariance(1)
    accz_variance = g.epmcV2.readAccVariance(2)

    acc_variance = [ accx_variance, accy_variance, accz_variance]

    print(colored("\n---------------------------------------------------------------", 'magenta'))
    print(colored("stored acc variances", 'green'))
    print(acc_variance)
    print(colored("---------------------------------------------------------------", 'magenta'))

    gyrox_variance = np.var(self.gyrox_arr)
    gyroy_variance = np.var(self.gyroy_arr)
    gyroz_variance = np.var(self.gyroz_arr)

    g.epmcV2.writeGyroVariance(0, gyrox_variance)
    g.epmcV2.writeGyroVariance(1, gyroy_variance)
    g.epmcV2.writeGyroVariance(2, gyroz_variance)

    gyrox_variance = g.epmcV2.readGyroVariance(0)
    gyroy_variance = g.epmcV2.readGyroVariance(1)
    gyroz_variance = g.epmcV2.readGyroVariance(2)

    gyro_variance = [ gyrox_variance, gyroy_variance, gyroz_variance]

    print(colored("\n---------------------------------------------------------------", 'magenta'))
    print(colored("stored gyro variances", 'green'))
    print(gyro_variance)
    print(colored("---------------------------------------------------------------", 'magenta'))

  def compute_variance(self):
    if self.start_process:
      self.reset_all_params()
      self.read_cal_data()
    else:
      self.reset_all_params()
      self.canvas.after(10, self.compute_variance)

  def change_btn_state(self):
    if self.start_process:
      self.start_process = False
      self.pressButton.configure(text='START')

    else:
      self.start_process = True
      self.pressButton.configure(text='STOP')