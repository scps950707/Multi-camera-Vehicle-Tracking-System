#!/usr/bin/python

from tkinter import *
from tkinter import filedialog
import os

class GUIDemo(Frame):
    def __init__(self, master=None):
        master.minsize(width=500, height=150)
        Frame.__init__(self, master)
        self.grid()
        self.createWidgets()

    def createWidgets(self):
        self.inputText = Label(self)
        self.inputText["text"] = "Kymco:"
        self.inputText.grid(row=0, column=0)

        self.kymcoPath = Entry(self)
        self.kymcoPath["width"] = 50
        self.kymcoPath.grid(row=0, column=1, columnspan=6)

        self.outputText = Label(self)
        self.outputText["text"] = "711"
        self.outputText.grid(row=1, column=0)

        self.sevenPath = Entry(self)
        self.sevenPath["width"] = 50
        self.sevenPath.grid(row=1, column=1, columnspan=6)

        self.buttonKymco = Button(self)
        self.buttonKymco["text"] = "LoadKymco"
        self.buttonKymco.grid(row=2, column=0)
        self.buttonKymco["command"] =  self.loadKymco

        self.buttonSeven = Button(self)
        self.buttonSeven["text"] = "Load711"
        self.buttonSeven.grid(row=2, column=1)
        self.buttonSeven["command"] =  self.loadSeven
        self.buttonRun = Button(self)
        self.buttonRun["text"] = "run"
        self.buttonRun.grid(row=2, column=2)
        self.buttonRun["command"] =  self.runProject

    def loadKymco(self):
        self.kymcoPath.delete(0, END)
        self.kymcoPath.insert(END,filedialog.askopenfilename())

    def loadSeven(self):
        self.sevenPath.delete(0, END)
        self.sevenPath.insert(END,filedialog.askopenfilename())

    def runProject(self):
        cmd = "./build/project -i " + self.kymcoPath.get() + " -j " + self.sevenPath.get()
        os.system(cmd)


if __name__ == '__main__':
    root = Tk()
    app = GUIDemo(master=root)
    app.mainloop()
