import tkinter as tk
import ttkbootstrap as tb
from ttkbootstrap.constants import *
from ttkbootstrap.dialogs import Messagebox

from termcolor import colored

from epmc_v2.globalParams import g


class ResetSetupFrame(tk.Frame):
  def __init__(self, parentFrame):
    super().__init__(master=parentFrame)

    self.label = tb.Label(self, text="RESET ALL PARAMETERS", font=('Monospace',16, 'bold') ,bootstyle="dark")
    self.frame = tb.Frame(self)

    #create widgets to be added to frame1
    buttonStyle = tb.Style()
    buttonStyleName = 'primary.TButton'
    buttonStyle.configure(buttonStyleName, font=('Monospace',10,'bold'))

    self.useIMUButton = tb.Button(self.frame, text="TURN ON/OFF IMU",
                               style=buttonStyleName, padding=20,
                               command=self.open_imu_dialog_event)
    
    self.resetButton = tb.Button(self.frame, text="RESET ALL PARAMETERS",
                               style=buttonStyleName, padding=20,
                               command=self.open_reset_dialog_event)

    #add framed widgets to frame
    self.useIMUButton.pack(side='top', expand=True, fill="both", pady=20)
    self.resetButton.pack(side='top', expand=True, fill="both")

    #add frame1, frame2 and frame3 to MainFrame
    self.label.pack(side="top", fill="x", padx=(220,0), pady=(5,0))
    self.frame.place(relx=0.5, rely=0.5, anchor="center")


  def open_imu_dialog_event(self):
    use_imu = int(g.epmcV2.getUseIMU())

    dialog = None
    if use_imu == 1:
      dialog = Messagebox.show_question(title="TURNING OFF IMU!!!", message="This will turn off IMU mode and turn on motor controller I2C communication capabilities.\nAre you sure you want to continue?")
    else:
      dialog = Messagebox.show_question(title="TURNING ON IMU!!!", message="This will turn on IMU mode and turn off motor controller I2C communication capabilities.\nAre you sure you want to continue?")

    if dialog == "Yes":
      isSuccessful = None
      if use_imu == 1:
        isSuccessful = self.deactivateIMU()
      else:
        isSuccessful = self.activateIMU()

      if isSuccessful:
        Messagebox.show_info("SUCCESS:\n\nOperation Successful\nReset Controller and Restart Application", "SUCCESS")
      else:
        Messagebox.show_error("ERROR:\n\nSomething went wrong\nOperation not successful", "ERROR")
    
    else:
      Messagebox.show_error("INFO:\n\nOperation Was Canceled", "ERROR")


  def open_reset_dialog_event(self):
    dialog = Messagebox.show_question(title="RESET WARNING!!!", message="This will reset all parameters on the controller's EEPROM to default.\nAre you sure you want to continue?")

    if dialog == "Yes":
      isSuccessful = self.resetAllParams()
      if isSuccessful:
        Messagebox.show_info("SUCCESS:\n\nParameters Reset was successful\nReset Controller and Restart Application", "SUCCESS")
      else:
        Messagebox.show_error("ERROR:\n\nSomething went wrong\nAttempt to reset was unsuccessful", "ERROR")
    
    else:
      Messagebox.show_error("INFO:\n\nOperation Was Cancelled", "ERROR")

  def resetAllParams(self):
    isSuccessful = g.epmcV2.resetAllParams()
    return isSuccessful
  
  def activateIMU(self):
    isSuccessful = g.epmcV2.setUseIMU(1)
    return isSuccessful
  
  def deactivateIMU(self):
    isSuccessful = g.epmcV2.setUseIMU(0)
    return isSuccessful
