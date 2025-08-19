import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *

import time

from epmc_v2.globalParams import g, selectSignal


class GraphFrame(tb.Frame):
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
    buttonStyle.configure(buttonStyleName, font=('Monospace',9, 'bold'))


    g.motorTargetVel[self.motorNo], g.motorActualVel[self.motorNo] = g.epmcV2.readPidVel(self.motorNo)

    self.actualText = tb.Label(self.textFrame1, text="ACTUAL(rad/s):", font=('Monospace',10, 'bold') ,bootstyle="danger")
    self.actualVal = tb.Label(self.textFrame1, text=g.motorActualVel[self.motorNo], font=('Monospace',10), bootstyle="dark")

    self.targetText = tb.Label(self.textFrame2, text="TARGET(rad/s):", font=('Monospace',10, 'bold') ,bootstyle="primary")
    self.targetVal = tb.Label(self.textFrame2, text=g.motorTargetVel[self.motorNo], font=('Monospace',10), bootstyle="dark")

    self.plotButton = tb.Button(self.displayFrame, text="START PLOT", style=buttonStyleName,
                            command=self.tryPlot)

    #add created widgets to displayFrame
    self.actualText.pack(side='left', fill='both')
    self.actualVal.pack(side='left', expand=True, fill='both')

    self.targetText.pack(side='left', fill='both')
    self.targetVal.pack(side='left', expand=True, fill='both')

    self.textFrame1.pack(side='left', expand=True, fill='both')
    self.textFrame2.pack(side='left', expand=True, fill='both')
    self.plotButton.pack(side='left', fill='both')


    #initialize graph parameters
    self.initGraphParameters()

    #create widgets to be added to the canvasFame
    self.canvas = tb.Canvas(self.canvasFrame, width=300, height=500,autostyle=False ,bg="#FFFFFF", relief='solid')

    #add created widgets to canvasFame
    self.canvas.pack(side='left', expand=True, fill='both')

    # initialize graph display
    self.drawGraphicalLine()


    # add displayFrame and canvasFrame to GraphFrame
    self.displayFrame.pack(side='top', expand=True, fill='x', padx=10)
    self.canvasFrame.pack(side='top', expand=True, fill='both', pady=(10,0))

    # start plotting process
    self.canvas.after(1, self.plot_graph)




  def initGraphParameters(self):
    # graph parameters
    self.w = 680
    self.h = 355

    self.xStartOffsetPnt = 35
    self.xStopOffsetPnt = 10

    self.yStartOffsetPnt = 10
    self.yStopOffsetPnt = 10

    self.xAxisLen = self.w - self.xStartOffsetPnt - self.xStopOffsetPnt
    self.yAxisLen = self.h - self.yStartOffsetPnt - self.yStopOffsetPnt

    self.initStartPnt = (self.xStartOffsetPnt, self.h/2) # x,y


    self.clearPlot = False
    self.doPlot = False
    self.doPlotTime = time.time()
    self.doPlotDuration = g.motorTestDuration[self.motorNo]

    self.currTime = 0.0
    self.prevTime = 0.0
    self.t = time.time()

    self.currValA = 0.0
    self.currValB = 0.0
    self.prevValA = 0.0
    self.prevValB = 0.0

    self.plotGraphBuffer = []
    self.plotLineBufferA = []
    self.plotLineBufferB = []

    self.maxXVal = self.doPlotDuration

    self.maxYVal = 2*g.motorMaxVel[self.motorNo]

    self.xScale = self.xAxisLen/self.maxXVal
    self.yScale = self.yAxisLen/self.maxYVal

    self.signalType = 'step'




  def drawGraphicalLine(self):
    self.deleteGraphParams(self.plotGraphBuffer)

    xAxisline = self.canvas.create_line(self.xStartOffsetPnt, self.h/2,
                                          self.xStartOffsetPnt+self.xAxisLen+self.xStopOffsetPnt, self.h/2,
                                          fill="black",width=2)
    self.plotGraphBuffer.append(xAxisline)
    text = self.canvas.create_text(self.xStartOffsetPnt+self.xAxisLen+self.xStopOffsetPnt, (self.h/2)+15,
                                    text="(sec)", fill="green", font=('Monospace 7 bold'), angle=90.0)
    self.plotGraphBuffer.append(text)
    
    yAxisline = self.canvas.create_line(self.xStartOffsetPnt, 0,
                                          self.xStartOffsetPnt, self.h,
                                          fill="black",width=2)
    self.plotGraphBuffer.append(yAxisline)
    text = self.canvas.create_text(self.xStartOffsetPnt-30, self.yStartOffsetPnt+8,
                                    text="(rad/s)", fill="green", font=('Helvetica 7 bold'), angle=90.0)
    self.plotGraphBuffer.append(text)
    
    text = self.canvas.create_text(self.xStartOffsetPnt-15, self.h/2,
                                    text="0.0", fill="black", font=('Helvetica 7 bold'))
    self.plotGraphBuffer.append(text)

    for i in range(1,6):
      yTickVal = i/5*self.maxYVal
      xAxisline = self.canvas.create_line(self.xStartOffsetPnt, (self.h/2)+((self.yScale/2)*yTickVal),
                                          self.xStartOffsetPnt+self.xAxisLen+self.xStopOffsetPnt, (self.h/2)+((self.yScale/2)*yTickVal),
                                          fill="grey",width=0.1, dash=(1,3))
      self.plotGraphBuffer.append(xAxisline)
      text = self.canvas.create_text(self.xStartOffsetPnt-15, (self.h/2)+((self.yScale/2)*yTickVal),
                                text=str(round(-yTickVal/2, 2)), fill="black", font=('Helvetica 7 bold'))
      self.plotGraphBuffer.append(text)


    for i in range(1,6):
      yTickVal = i/5*self.maxYVal
      xAxisline = self.canvas.create_line(self.xStartOffsetPnt, (self.h/2)-((self.yScale/2)*yTickVal),
                                          self.xStartOffsetPnt+self.xAxisLen+self.xStopOffsetPnt, (self.h/2)-((self.yScale/2)*yTickVal),
                                          fill="grey",width=0.1, dash=(1,3))
      self.plotGraphBuffer.append(xAxisline)
      text = self.canvas.create_text(self.xStartOffsetPnt-15, (self.h/2)-((self.yScale/2)*yTickVal),
                                text=str(round(yTickVal/2, 2)), fill="black", font=('Helvetica 7 bold'))
      self.plotGraphBuffer.append(text)
    
    for i in range(1,21):
      xTickVal = i/20*self.maxXVal
      yAxisline = self.canvas.create_line(self.xStartOffsetPnt+(self.xScale*xTickVal), 0,
                                               self.xStartOffsetPnt+(self.xScale*xTickVal), self.h,
                                               fill="grey",width=0.1, dash=(1,3))
      self.plotGraphBuffer.append(yAxisline)
      text = self.canvas.create_text(self.xStartOffsetPnt+(self.xScale*xTickVal), (self.h/2)+15,
                                text=str(round(xTickVal, 2)), fill="black", font=('Helvetica 7 bold'), angle=90.0)
      self.plotGraphBuffer.append(text)


  def deleteGraphParams(self, graphParams):
      for param in graphParams:
          self.canvas.delete(param)
          # root.update_idletasks()
      self.plotGraphBuffer = []


  def tryPlot(self):
    if self.clearPlot:
        self.deletePlot(self.plotLineBufferA, self.plotLineBufferB)
        self.plotButton.configure(text='START PLOT')
        self.clearPlot = False
        # g.epmcV2.setPidMode(self.motorNo, 0)
        time.sleep(0.1)

    elif self.doPlot:
        self.doPlot = False 
        # g.epmcV2.setPidMode(self.motorNo, 0)
        # print('stop plot')
    else:
        self.doPlot = True 
        self.doPlotTime = time.time()
        # g.epmcV2.setPidMode(self.motorNo, 1)
        # print('start plot')


  def deletePlot(self, linesA, linesB):
      for lineA in linesA:
          self.canvas.delete(lineA)
          # root.update_idletasks()
      for lineB in linesB:
          self.canvas.delete(lineB)
          # root.update_idletasks()
      self.plotLineBufferA = []
      self.plotLineBufferB = []


  def plot_graph(self):
      if self.doPlot and self.doPlotDuration < time.time()-self.doPlotTime:
          if g.motorIsOn[self.motorNo]:
            isSuccess = g.epmcV2.writeSpeed(self.motorNo, 0.0)
            if isSuccess:
              g.motorIsOn[self.motorNo] = False
              # print('Motor off', isSuccess)
          self.doPlot = False 
          self.clearPlot = True
          self.plotButton.configure(text='CLEAR PLOT')
          self.currValA = 0.0
          self.prevValA = 0.0
          self.currValB = 0.0
          self.prevValB = 0.0
          self.currTime = 0.0
          self.prevTime = 0.0
          self.t = time.time()
          # print('stop plot')
          self.canvas.after(1, self.plot_graph)

      elif self.doPlot:
          targetVel =selectSignal(type=g.motorTestSignal[self.motorNo],
                                  targetMax=g.motorTargetMaxVel[self.motorNo], 
                                  duration=self.doPlotDuration, 
                                  deltaT=time.time()-self.doPlotTime)
          
          if not g.motorIsOn[self.motorNo]:
            isSuccess = g.epmcV2.writeSpeed(self.motorNo, targetVel)

            if isSuccess:
              g.motorIsOn[self.motorNo] = True
              # print('Motor on', isSuccess)
          
          isSuccess = g.epmcV2.writeSpeed(self.motorNo, targetVel)

          try:
            g.motorTargetVel[self.motorNo], g.motorActualVel[self.motorNo] = g.epmcV2.readPidVel(self.motorNo)
            
          except:
            pass

          self.currValA = g.motorTargetVel[self.motorNo]
          self.currValB = g.motorActualVel[self.motorNo]
          self.currTime = time.time()-self.t

          # primary "#4582EC" danger "#D9534F"
          lineA = self.canvas.create_line(self.xStartOffsetPnt+(self.prevTime*self.xScale),-self.yScale*self.prevValA+self.h/2,
                                           self.xStartOffsetPnt+(self.currTime*self.xScale), -self.yScale*self.currValA+self.h/2,
                                           fill="#4582EC", width=1)
          lineB = self.canvas.create_line(self.xStartOffsetPnt+(self.prevTime*self.xScale),-self.yScale*self.prevValB+self.h/2,
                                           self.xStartOffsetPnt+(self.currTime*self.xScale), -self.yScale*self.currValB+self.h/2,
                                           fill="#D9534F", width=1)
          
          self.targetVal.configure(text=g.motorTargetVel[self.motorNo])

          self.actualVal.configure(text=g.motorActualVel[self.motorNo])

          self.plotButton.configure(text='STOP PLOT')

          self.plotLineBufferA.append(lineA)
          self.plotLineBufferB.append(lineB)
          # root.update_idletasks()

          self.prevValA = self.currValA
          self.prevValB = self.currValB

          self.prevTime = self.currTime
          
          self.canvas.after(1, self.plot_graph)

      else:
          if g.motorIsOn[self.motorNo]:
            isSuccess = g.epmcV2.writeSpeed(self.motorNo, 0.0)

            if isSuccess:
              self.clearPlot = True
              self.plotButton.configure(text='CLEAR PLOT')
              g.motorIsOn[self.motorNo] = False
              # print('Motor off', isSuccess)
          self.currValA = 0.0
          self.prevValA = 0.0
          self.currValB = 0.0
          self.prevValB = 0.0
          self.currTime = 0.0
          self.prevTime = 0.0
          self.t = time.time()
          self.canvas.after(1, self.plot_graph)