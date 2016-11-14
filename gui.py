#!/usr/bin/python

from tkinter import *
from tkinter import filedialog
import os
import configparser

class GUIDemo(Frame):
    config=configparser.RawConfigParser()
    config.read("path.ini")
    execute= config.get('path','execute')
    seven1 = config.get('path','seven1')
    seven2 = config.get('path','seven2')
    seven3 = config.get('path','seven3')
    seven4 = config.get('path','seven4')
    seven5 = config.get('path','seven5')
    kymco1 = config.get('path','kymco1')
    kymco2 = config.get('path','kymco2')
    kymco3 = config.get('path','kymco3')
    kymco4 = config.get('path','kymco4')
    kymco5 = config.get('path','kymco5')

    def __init__(self, master=None):
        master.minsize(width=500, height=150)
        Frame.__init__(self, master)
        self.grid()
        self.createWidgets()

    def createWidgets(self):
        self.buttonKymco = Button(self)
        self.buttonKymco["text"] = "DEMO 1"
        self.buttonKymco.grid(row=1, column=0)
        self.buttonKymco["command"] =  self.DEMO1

        self.buttonSeven = Button(self)
        self.buttonSeven["text"] = "DEMO 2"
        self.buttonSeven.grid(row=1, column=1)
        self.buttonSeven["command"] =  self.DEMO2

        self.buttonRun = Button(self)
        self.buttonRun["text"] = "DEMO 3"
        self.buttonRun.grid(row=1, column=3)
        self.buttonRun["command"] =  self.DEMO3

        self.buttonRun = Button(self)
        self.buttonRun["text"] = "DEMO 4"
        self.buttonRun.grid(row=1, column=4)
        self.buttonRun["command"] =  self.DEMO4

        self.buttonRun = Button(self)
        self.buttonRun["text"] = "DEMO 5"
        self.buttonRun.grid(row=1, column=5)
        self.buttonRun["command"] =  self.DEMO5

    def DEMO1(self):
        cmd = self.execute + " -i " + self.seven1 + " -j " + self.kymco1
        os.system(cmd)

    def DEMO2(self):
        cmd = self.execute + " -i " + self.seven2 + " -j " + self.kymco2
        os.system(cmd)

    def DEMO3(self):
        cmd = self.execute + " -i " + self.seven3 + " -j " + self.kymco3
        os.system(cmd)

    def DEMO4(self):
        cmd = self.execute + " -i " + self.seven4 + " -j " + self.kymco4
        os.system(cmd)

    def DEMO5(self):
        cmd = self.execute + " -i " + self.seven5 + " -j " + self.kymco5
        os.system(cmd)

if __name__ == '__main__':
    root = Tk()
    root.title("Multi-camera Vehicle Tracking System")
    app = GUIDemo(master=root)
    app.mainloop()
