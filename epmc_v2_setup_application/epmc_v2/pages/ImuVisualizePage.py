import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

from epmc_v2.globalParams import g
# from epmc_v2.components.SetValueFrame import SetValueFrame
# from epmc_v2.components.SelectValueFrame import SelectValueFrame



class ImuVisualizeFrame(tb.Frame):
  def __init__(self, parentFrame):
    super().__init__(master=parentFrame)

    self.fig, self.ax = None, None

    self.world_axis_x_color = '#a00000'
    self.world_axis_y_color = 'green'
    self.world_axis_z_color = '#0000a0'

    self.world_axis_line_width = str(4.0)

    self.sensor_axis_x_color = 'red'
    self.sensor_axis_y_color = '#00a000'
    self.sensor_axis_z_color = 'blue'

    self.sensor_axis_line_width = str(4.0)

    # self.plot_elevation_angle = 60 
    # self.plot_horizontal_angle = 60
    

    self.label = tb.Label(self, text="VIZUALIZE IMU", font=('Monospace',16, 'bold') ,bootstyle="dark")
  
    #create widgets to be added to the Fame
    # g.frameId = int(g.epmcV2.get("/frame-id"))
    # self.selectFrameId = SelectValueFrame(self, keyTextInit=f"REFERENCE_FRAME: ", valTextInit=g.frameList[g.frameId],
    #                                       initialComboValues=g.frameList, middileware_func=self.selectFrameIdFunc)
    
    # g.filterGain = g.epmcV2.get("/gain")
    # self.setFilterGain = SetValueFrame(self, keyTextInit="FILTER_GAIN: ", valTextInit=g.filterGain,
    #                             middleware_func=self.setFilterGainFunc)
    
    buttonStyle = tb.Style()
    buttonStyleName = 'primary.TButton'
    buttonStyle.configure(buttonStyleName, font=('Monospace',12,'bold'))

    self.button = tb.Button(self, text="VISUALIZE IMU", style=buttonStyleName,
                             padding=20, command=self.runVisualization)
    

    self.rollValFrame = tb.Frame(self)
    self.pitchValFrame = tb.Frame(self)
    self.yawValFrame = tb.Frame(self)

    roll = g.epmcV2.readRPY(0)
    pitch = g.epmcV2.readRPY(1)
    yaw = g.epmcV2.readRPY(2)

    self.rollText = tb.Label(self.rollValFrame, text="ROLL:", font=('Monospace',10, 'bold') ,bootstyle="danger")
    self.rollVal = tb.Label(self.rollValFrame, text=f'{roll}', font=('Monospace',10), bootstyle="dark")

    self.pitchText = tb.Label(self.pitchValFrame, text="PITCH:", font=('Monospace',10, 'bold') ,bootstyle="success")
    self.pitchVal = tb.Label(self.pitchValFrame, text=f'{pitch}', font=('Monospace',10), bootstyle="dark")

    self.yawText = tb.Label(self.yawValFrame, text="YAW:", font=('Monospace',10, 'bold') ,bootstyle="primary")
    self.yawVal = tb.Label(self.yawValFrame, text=f'{yaw}', font=('Monospace',10), bootstyle="dark")
    

    #add created widgets to displayFrame
    self.rollText.pack(side='left', fill='both')
    self.rollVal.pack(side='left', expand=True, fill='both')

    self.pitchText.pack(side='left', fill='both')
    self.pitchVal.pack(side='left', expand=True, fill='both')

    self.yawText.pack(side='left', fill='both')
    self.yawVal.pack(side='left', expand=True, fill='both')

    #add created widgets to Frame
    self.label.pack(side='top', pady=(20,20))
    # self.selectFrameId.pack(side='top', fill='y', pady=(30,0))
    # self.setFilterGain.pack(side='top', fill='y', pady=(30,0))
    self.button.pack(side='top', fill='y', pady=(50,0))

    self.rollValFrame.pack(side='top', fill='x')
    self.pitchValFrame.pack(side='top', fill='x')
    self.yawValFrame.pack(side='top', fill='x')
  
    ############################################


  # def setFilterGainFunc(self, text):
  #   try:
  #     if text:
  #       isSuccessful = g.epmcV2.send("/gain", float(text))
  #       val = g.epmcV2.get("/gain")
  #       g.filterGain = val
  #   except:
  #     pass
  
  #   return g.filterGain
  

  # def selectFrameIdFunc(self, frame_val_str):
  #   try:
  #     if frame_val_str:
        
  #       if frame_val_str == g.frameList[0]:
  #         isSuccessful = g.epmcV2.send("/frame-id", 0)
          
  #       elif frame_val_str == g.frameList[1]:
  #         isSuccessful = g.epmcV2.send("/frame-id", 1)
        
  #       elif frame_val_str == g.frameList[2]:
  #         isSuccessful = g.epmcV2.send("/frame-id", 2)

  #   except:
  #     pass

  #   g.frameId = int(g.epmcV2.get("/frame-id"))
  #   return g.frameList[g.frameId]


  def onClose(self,event): 
    plt.close()
    self.fig, self.ax = None, None 


  def animate(self,i):
    try:
      roll = g.epmcV2.readRPY(0)
      pitch = g.epmcV2.readRPY(1)
      yaw = g.epmcV2.readRPY(2)

      self.rollVal.configure(text=f"{roll}")
      self.pitchVal.configure(text=f"{pitch}")
      self.yawVal.configure(text=f"{yaw}")

      #-----------------------------------------------------------------------
      ##### convert rpy to DCM #####################
      DCM = [[np.cos(pitch)*np.cos(yaw), np.cos(pitch)*np.sin(yaw), -1.0*np.sin(pitch)], # cθcψ, cθsψ, −sθ
             [(np.sin(roll)*np.sin(pitch)*np.cos(yaw)) - (np.cos(roll)*np.sin(yaw)), (np.sin(roll)*np.sin(pitch)*np.sin(yaw)) + (np.cos(roll)*np.cos(yaw)), np.sin(roll)*np.cos(pitch)], # sϕsθcψ - cϕsψ, sϕsθsψ + cϕcψ, sϕcθ
             [(np.cos(roll)*np.sin(pitch)*np.cos(yaw)) + (np.sin(roll)*np.sin(yaw)), (np.cos(roll)*np.sin(pitch)*np.sin(yaw)) - (np.sin(roll)*np.cos(yaw)), np.cos(roll)*np.cos(pitch)]] # cϕsθcψ + sϕsψ, cϕsθsψ - sϕcψ, cϕcθ
      
      ##### get the IMU sensor coordinate vector from the DCM #####################
      x_vect = DCM[0]
      y_vect = DCM[1]
      z_vect = DCM[2]
      #----------------------------------------------------------------------



      # Clear all axis
      self.ax.cla()
      
      self.ax.set_xlim(-1.0, 1.0)
      self.ax.set_ylim(-1.0, 1.0)
      self.ax.set_zlim(-1.0, 1.0)
      self.ax.grid(False)
      # self.ax.view_init(self.plot_elevation_angle, self.plot_horizontal_angle)
      
      # defining world axes
      x0 = [0, 1]
      x1 = [0, 0]
      x2 = [0, 0]  
      self.ax.plot(x0, x1, x2, c=self.world_axis_x_color, lw=self.world_axis_line_width)

      y0 = [0, 0]
      y1 = [0, 1]
      y2 = [0, 0]  
      self.ax.plot(y0, y1, y2, c=self.world_axis_y_color, lw=self.world_axis_line_width)

      z0 = [0, 0]
      z1 = [0, 0]
      z2 = [0, 1]  
      self.ax.plot(z0, z1, z2, c=self.world_axis_z_color, lw=self.world_axis_line_width)


      # defining sensor axes
      x0 = [0, x_vect[0]]
      x1 = [0, x_vect[1]]
      x2 = [0, x_vect[2]]  
      self.ax.plot(x0, x1, x2, c=self.sensor_axis_x_color, lw=self.sensor_axis_line_width)

      y0 = [0, y_vect[0]]
      y1 = [0, y_vect[1]]
      y2 = [0, y_vect[2]]  
      self.ax.plot(y0, y1, y2, c=self.sensor_axis_y_color, lw=self.sensor_axis_line_width)

      z0 = [0, z_vect[0]]
      z1 = [0, z_vect[1]]
      z2 = [0, z_vect[2]]  
      self.ax.plot(z0, z1, z2, c=self.sensor_axis_z_color, lw=self.sensor_axis_line_width)
        
        
    ##    # Pause the plot for INTERVAL seconds 
    ##    plt.pause(INTERVAL)
    except:
      pass


  def runVisualization(self):
    self.fig = plt.figure()
    self.ax = self.fig.add_subplot(111, projection='3d')

    self.ax.set_xlim(-1.0, 1.0)
    self.ax.set_ylim(-1.0, 1.0)
    self.ax.set_zlim(-1.0, 1.0)
    self.ax.grid(False)
    # self.ax.view_init(self.plot_elevation_angle, self.plot_horizontal_angle)
    
    # defining axes
    x0 = [0, 1]
    x1 = [0, 0]
    x2 = [0, 0]  
    self.ax.plot(x0, x1, x2, c=self.world_axis_x_color, lw=self.world_axis_line_width)

    y0 = [0, 0]
    y1 = [0, 1]
    y2 = [0, 0]  
    self.ax.plot(y0, y1, y2, c=self.world_axis_y_color, lw=self.world_axis_line_width)

    z0 = [0, 0]
    z1 = [0, 0]
    z2 = [0, 1]  
    self.ax.plot(z0, z1, z2, c=self.world_axis_z_color, lw=self.world_axis_line_width)

    self.fig.canvas.mpl_connect('close_event', self.onClose)
    self.anim = FuncAnimation(self.fig, self.animate, frames = np.arange(0, 1000000, 1), interval=50)
    plt.show()