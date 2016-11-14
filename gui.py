#!/usr/bin/python

from Tkinter import *
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
        master.minsize(width=450, height=150)
        Frame.__init__(self, master)
        self.grid()
        self.createWidgets()
        self.cmd=""

    def createWidgets(self):
        self.buttonKymco = Button(self)
        self.buttonKymco["text"] = "DEMO 1"
        self.buttonKymco.grid(row=0, column=0)
        self.buttonKymco["command"] =  self.DEMO1

        self.buttonSeven = Button(self)
        self.buttonSeven["text"] = "DEMO 2"
        self.buttonSeven.grid(row=0, column=1)
        self.buttonSeven["command"] =  self.DEMO2

        self.buttonRun = Button(self)
        self.buttonRun["text"] = "DEMO 3"
        self.buttonRun.grid(row=0, column=2)
        self.buttonRun["command"] =  self.DEMO3

        self.buttonRun = Button(self)
        self.buttonRun["text"] = "DEMO 4"
        self.buttonRun.grid(row=0, column=3)
        self.buttonRun["command"] =  self.DEMO4

        self.buttonRun = Button(self)
        self.buttonRun["text"] = "DEMO 5"
        self.buttonRun.grid(row=0, column=4)
        self.buttonRun["command"] =  self.DEMO5

        self.inputText = Label(self)
        self.inputText["text"] = "start time(sec):"
        self.inputText.grid(row=1,column=0,columnspan=2)
        self.inputText.config(font=("bold",15))

        self.startTime = Entry(self)
        self.startTime["width"] = 10
        self.startTime.grid(row=1,column=2)

    def DEMO1(self):
        self.runProject(self.seven1,self.kymco1)

    def DEMO2(self):
        self.runProject(self.seven2,self.kymco2)

    def DEMO3(self):
        self.runProject(self.seven3,self.kymco3)

    def DEMO4(self):
        self.runProject(self.seven4,self.kymco4)

    def DEMO5(self):
        self.runProject(self.seven5,self.kymco5)

    def runProject(self,seven,kymco):
        self.cmd = self.execute + " -i " + seven + " -j " + kymco
        time = self.startTime.get()
        if time.isdigit() and time!="0":
            self.cmd += " -t "+time
        os.system(self.cmd)

if __name__ == '__main__':
    root = Tk()
    root.title("Multi-camera Vehicle Tracking System")
    app = GUIDemo(master=root)
    app.mainloop()
