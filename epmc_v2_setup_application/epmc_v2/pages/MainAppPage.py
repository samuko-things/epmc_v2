import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *

from epmc_v2.pages.I2CSetupPage import I2CSetupFrame
from epmc_v2.pages.ResetSetupPage import ResetSetupFrame
from epmc_v2.pages.EncSetupPage import EncSetupFrame
from epmc_v2.pages.PidSetupPage import PidSetupFrame




class MainAppFrame(tb.Frame):
  def __init__(self, parentFrame):
    super().__init__(master=parentFrame)


    # SIDEBAR NAVIGATION FRAME
    self.sideNavFrame = tb.LabelFrame(self, borderwidth=10)

    # MIAN CONTENT FRAME
    self.mainContentFrame = tb.Frame(self)


    #create widgets to be added to the sideNavFrame
    self.label = tb.Label(self.sideNavFrame, text="MENU", font=('Monospace',20, 'bold') ,bootstyle="secondary")

    buttonStyle = tb.Style()
    buttonStyleName = 'primary.Link.TButton'
    buttonStyle.configure(buttonStyleName, font=('Monospace',12, 'bold'))

    self.button1 = tb.Button(self.sideNavFrame, text="MOTOR 0 ENC", style=buttonStyleName,
                             command= lambda: self.displayPage(self.button1, self.displayMotor0EncSetupPage))
    
    self.button2 = tb.Button(self.sideNavFrame, text="MOTOR 0 PID", style=buttonStyleName,
                             command= lambda: self.displayPage(self.button2, self.displayMotor0PidSetupPage))
    
    self.button3 = tb.Button(self.sideNavFrame, text="MOTOR 1 ENC", style=buttonStyleName,
                             command= lambda: self.displayPage(self.button3, self.displayMotor1EncSetupPage))
    
    self.button4 = tb.Button(self.sideNavFrame, text="MOTOR 1 PID", style=buttonStyleName,
                             command= lambda: self.displayPage(self.button4, self.displayMotor1PidSetupPage))
    
    self.button5 = tb.Button(self.sideNavFrame, text="I2C SETUP", style=buttonStyleName,
                             command= lambda: self.displayPage(self.button5, self.displayI2CSetupPage))
    
    self.button6 = tb.Button(self.sideNavFrame, text="RESET PARAMS", style=buttonStyleName,
                             command= lambda: self.displayPage(self.button6, self.displayResetPage))
    
    
    # add widget to sideNavFrame
    self.label.pack(side="top", fill="x", padx=(40,0), pady=(0,25))
    self.button1.pack(side="top", fill="x", padx=5, pady=2)
    self.button2.pack(side="top", fill="x", padx=5, pady=(2,30))
    self.button3.pack(side="top", fill="x", padx=5, pady=2)
    self.button4.pack(side="top", fill="x", padx=5, pady=(2,30))
    self.button5.pack(side="top", fill="x", padx=5, pady=2)
    self.button6.pack(side="top", fill="x", padx=5, pady=2)


    
    ############Initialize the mainContentFrame ################
    self.displayPage(self.button6, self.displayResetPage)
    ############################################################


    #add framed widgets to MainAppFrame
    self.sideNavFrame.pack(side="left", fill="y", padx=10)
    self.mainContentFrame.pack(side="left", expand=True, fill="both", padx=5)


  
  def enable_all_nav_buttons(self):
    self.button1.configure(state="normal")
    self.button2.configure(state="normal")
    self.button3.configure(state="normal")
    self.button4.configure(state="normal")
    self.button5.configure(state="normal")
    self.button6.configure(state="normal")
  
  def displayPage(self, button, page):
    self.enable_all_nav_buttons()
    button.configure(state='disabled') # disable the clicked nav button
    self.delete_pages()
    page()

  def delete_pages(self):
    for frame in self.mainContentFrame.winfo_children():
      frame.destroy()


  
  
  def displayMotor0EncSetupPage(self):
    self.motor0EncSetupFrame = EncSetupFrame(self.mainContentFrame, motorNo=0)
    self.motor0EncSetupFrame.pack(side="left", expand=True, fill="both")
  
  def displayMotor0PidSetupPage(self):
    self.motor0PidSetupFrame = PidSetupFrame(self.mainContentFrame, motorNo=0)
    self.motor0PidSetupFrame.pack(side="left", expand=True, fill="both")

  def displayMotor1EncSetupPage(self):
    self.motor1EncSetupFrame = EncSetupFrame(self.mainContentFrame, motorNo=1)
    self.motor1EncSetupFrame.pack(side="left", expand=True, fill="both")

  def displayMotor1PidSetupPage(self):
    self.motor1PidSetupFrame = PidSetupFrame(self.mainContentFrame, motorNo=1)
    self.motor1PidSetupFrame.pack(side="left", expand=True, fill="both")

  def displayI2CSetupPage(self):
    self.i2cSetupFrame = I2CSetupFrame(self.mainContentFrame)
    self.i2cSetupFrame.pack(side="left", expand=True, fill="both")

  def displayResetPage(self):
    self.resetFrame = ResetSetupFrame(self.mainContentFrame)
    self.resetFrame.pack(side="left", expand=True, fill="both")