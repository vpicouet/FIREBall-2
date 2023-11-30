import matplotlib
import glob
import numpy as np
import select
import subprocess
import time

matplotlib.use("TkAgg")

from astropy.io import fits
from astropy.modeling import models,fitting
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg,NavigationToolbar2TkAgg
from matplotlib.figure import Figure
from Tkinter import *

def go():
    global profDirInput,regExInput,outputDir
    root = Tk()
    app = focusGUI(master=root,profDir=profDirInput,regEx=regExInput,outDir=outputDir)
    app.mainloop()
    root.destroy()

class focusGUI(Frame):
    
    
    def clear(self):
        self.ax.clear()
        self.figCanvas.draw()
    
    def updatePlot(self):
        self.ax.clear()
        self.figCanvas.draw()

        self.xdata = [] #For modeling purposes
        self.ydata = [] 
                
        stages = { n:[] for n in set(self.nums) }
        sigmas = { n:[] for n in set(self.nums) }
      
        for f in self.files:
            stage=0
            xsig =0
            ysig =0
            n = int(f.split('/')[-1][0])
	    if n not in stages.keys(): continue
	    elif self.starCheckVars[n].get():
	            for i,L in enumerate(open(f)):
        	        if   i==1: stage=float(L.split()[-3])#np.mean([ float(x) for x in L.split()[-3:] ])
        	        elif i==2: xsig=float(L.split()[3])
        	        elif i==3: ysig=float(L.split()[3])
        	    if xsig>0 and ysig>0 and stage>0: 
        	        stages[n].append(stage)
        	        sigmas[n].append(np.sqrt(xsig**2 + ysig**2))
	
        for n in stages.keys():
                self.ax.plot(stages[n],sigmas[n],'o',label="Star %i"%(n))
		for sj in stages[n]: self.xdata.append( sj )
                for yj in sigmas[n]:  self.ydata.append( yj )

        self.ax.legend()
        self.fig.tight_layout()
        self.figCanvas.draw()     
           
    def update(self):

        try:
            N0 = int(self.N0Entry.get())
            if self.N1Entry.get()=="*": N1 = np.inf
	    else: N1 = int(self.N1Entry.get())
	  
        except:
            print "Error parsing input. All values should be valid Integers."
            return
            
        self.getFiles(N0,N1) #Get new files in this image number range
        
        self.updateStars() #Update number of star checkboxes
                    
        self.updatePlot() #Update plotted values
        
    def fit(self):
    
        self.updatePlot()
        
        model_init = models.Polynomial1D(degree=int(self.polyK.get()))
        
        model_fit  = self.fitter(model_init,self.xdata,self.ydata)
         
        xSmooth = np.linspace(min(self.xdata)-1,max(self.xdata)+1,1000)
        self.ax.plot(xSmooth,model_fit(xSmooth),'k-')
        xBest = xSmooth[ np.nanargmin(model_fit(xSmooth)) ]
        
	xMed = np.median(self.xdata)
	bMed = xMed + self.dB
	cMed = xMed + self.dC

	xOff = xBest - xMed
	bBest = xBest + self.dB
	cBest = xBest + self.dC
        self.ax.plot([xBest,xBest],[0,10],'k--')
        self.ax.annotate("Fit minimum: %.2f,%.2f,%.2f\nMid-point: %.2f,%.2f,%.2f\nOffset: %+.2f" % (xBest,bBest,cBest,xMed,bMed,cMed,xOff), (xBest+0.02,self.ylim[1]-1)) 
        self.figCanvas.draw()
    
    def updateStars(self):
    	
	#Get currently selected stars
	self.stars = []
	for i,n in enumerate(self.starCheckVars): 
		if self.starCheckVars[n].get():
			self.stars.append(i)
 
	#Delete current check buttons
        for n,starCheck in self.starChecks.items():
            if starCheck.winfo_ismapped(): 
                starCheck.pack_forget()
        
	done = { n:0 for n in set(self.nums) }
        for i,n in enumerate(self.nums):
	    if not done[n]:
	    	self.starChecks[n]["text"] = "%i (%i,%i)" % (n,self.xPos[i],self.yPos[i])
            	self.starChecks[n].pack({"side":"left"})
		done[n] = 1

    def initElements(self):
        
        ## MAIN LAYOUT
        
        self.plotFrame   = Frame(self,bg="white")
        self.buttonFrame = Frame(self)
        self.optionFrame = Frame(self)
        
        self.optionFrame.pack({"side":"top"},fill=BOTH,expand=0)
        self.plotFrame.pack({"side":"top"},fill=BOTH,expand=1)
        self.buttonFrame.pack({"side":"top"},fill=BOTH,expand=0)
        
        ## BUTTONS
        
        self.quitButton     = Button(self.buttonFrame,text="Quit",command=self.quit)
        self.clearButton    = Button(self.buttonFrame,text="Clear",command=self.clear)
        self.updateButton   = Button(self.buttonFrame,text="Update",command=self.update)
        self.fitButton      = Button(self.buttonFrame,text="Fit",command=self.fit)
        self.makeFitsButton = Button(self.buttonFrame,text="Save Table",command=self.generateFitsTable)
        
        self.quitButton.pack({"side":"right"})
        self.clearButton.pack({"side":"right"})
        self.updateButton.pack({"side":"left"})
        self.fitButton.pack({"side":"left"})
        self.makeFitsButton.pack({"side":"left"})

        ### FIGURE 
        
        self.fig = Figure(figsize=self.figsize)
        self.ax = self.fig.add_subplot(111)
        self.ax.set_xlim(self.xlim)
        self.ax.set_ylim(self.ylim)
        self.figCanvas = FigureCanvasTkAgg(self.fig,self.plotFrame)
        self.fig.tight_layout()
        self.figCanvas.show()
        
        self.toolbar = NavigationToolbar2TkAgg(self.figCanvas,self.plotFrame)
        self.toolbar.update()
        
        self.figCanvas._tkcanvas.pack(side=TOP,fill=BOTH,expand=True)

        ## OPTIONS 

        self.N0Entry = Entry(self.optionFrame,width=10)
        self.N1Entry = Entry(self.optionFrame,width=10)

        self.N0Entry.bind("<Return>",self.update)
        self.N1Entry.bind("<Return>",self.update)
        
        self.N0Label = Label(self.optionFrame,text="N0")
        self.N1Label = Label(self.optionFrame,text="N1")

        self.N0Label.pack({"side":"left"})        
        self.N0Entry.pack({"side":"left"})      
        self.N1Label.pack({"side":"left"})        
        self.N1Entry.pack({"side":"left"})
        
        self.polyKLab = Label(self.optionFrame,text="PolyK")
        self.polyK = Entry(self.optionFrame,width=2)
        self.polyK.insert(END,'2')
        self.polyKLab.pack({"side":"left"})
        self.polyK.pack({"side":"left"})
        
        self.starCheckVars = {}
        self.starChecks    = {}
        
	for i in range(8):
            self.starCheckVars[i] = IntVar()
            self.starChecks[i] = Checkbutton(self.optionFrame,text=i,variable=self.starCheckVars[i],command=self.updateStars)
            self.starChecks[i].select()
            
            
        self.fitter = fitting.LinearLSQFitter()
        
    def getFiles(self,N0,N1):
        #try:
            files   = glob.glob("%s%s"%(self.profDir,self.regEx))
            imgNos  = [ int(f.split('/')[-1].split('_')[1]) for f in files ]              
            self.files   = [ files[i] for i in range(len(files)) if N0<=imgNos[i]<=N1]
            self.xPos,self.yPos,self.nums = [],[],[]
            for f in self.files:
                    for i,line in enumerate(open(f)):
                         if i==0:
                                 self.nums.append(int(line.split()[1]))
                                 self.xPos.append(int(line.split()[3]))
                                 self.yPos.append(int(line.split()[4]))
			 elif i>0: break

	    for i,line in enumerate(open(self.files[0])):
	            if i==1: 
			A,B,C = [float(k) for k in line.split()[-3:]]
			self.dB = B-A
			self.dC = C-A
		
        #except:
        #    print "Error loading files."
        #    self.files = []
        
        
    def generateFitsTable(self):
        
        starNos = []
        imgNos  = []
        stages  = []
        xprofs  = []
        yprofs  = []
        xparams = []
        yparams = []
        
        for f in self.files:
            
            starNo,imgNo = ( int(x) for x in f.split('/')[-1].split('_')[:2] )
            data  = np.loadtxt(f,skiprows=4,usecols=(1,2))

            stage,xparam,yparam = [],[],[]
            for i,L in enumerate(open(f)):
                if   i==1: stage =  [ float(x) for x in L.split()[-3:] ]
                elif i==2: xparam = [ float(x) for x in L.split()[1:]  ]
                elif i==3: yparam = [ float(x) for x in L.split()[1:]  ]
                
            starNos.append(starNo)
            imgNos.append(imgNo)
            stages.append(stage)
            xprofs.append(data[:,0])
            yprofs.append(data[:,1])
            xparams.append(xparam)
            yparams.append(yparam) 
            
        stages = np.array(stages)
        xprofs = np.array(xprofs)
        yprofs = np.array(yprofs)
        xparams = np.array(xparams)
        yparams = np.array(yparams)
   
        f_out = open(self.outDir + "focus_%i.txt"%imgNos[0],'w')
        f_out.write("#%7s%7s %8s %8s %8s" % ("Image","Star","A","B","C"))
        for s in ["xAmp","xMean","xSig","xConst"]: f_out.write(" %8s"%s)
        for s in ["yAmp","yMean","ySig","yConst"]: f_out.write(" %8s"%s)
        for i in  range(len(xprofs[0])): f_out.write(" %6s%02i"%("x",i))
        for i in  range(len(yprofs[0])): f_out.write(" %6s%02i"%("y",i))
        f_out.write("\n")
        for i in range(len(starNos)):
        
            f_out.write("%08i%7i" % (imgNos[i],starNos[i]))
            for sj in  stages[i]: f_out.write(" %8.2f"%sj)
            for xp in xparams[i]: f_out.write(" %8.2f"%xp)
            for yp in yparams[i]: f_out.write(" %8.2f"%yp)
            for xd in  xprofs[i]: f_out.write(" %8i"%int(xd))
            for yd in  yprofs[i]: f_out.write(" %8i"%int(yd))
            f_out.write("\n")
        f_out.close()
    def __init__(self,master=None,profDir=".",regEx="*prof.txt",outDir="."):
        Frame.__init__(self,master)
        self.pack(fill=BOTH,expand=1)

        # MODIFY THESE VARIABLES TO CHANGE GUI PARAMETERS ###
        self.master.title("FIREBALL-2 Focus GUI")
        self.figsize = (12,6)
        self.xlim    = [12,14]
        self.ylim    = [1 , 8 ]
        #####################################################

        self.profDir = profDir
        self.regEx   = regEx   
        self.outDir  = outDir
        self.initElements()

        # COMMENT OR CHANGE THESE TO REMOVE/CHANGE DEFAULT IMG NUMBERS ###
        self.N0Entry.insert(END,'22812775')
        self.N1Entry.insert(END,'22816000')
        ##################################################################
	# N0 + Nimg*Exp*3.8

# MODIFY THESE VARIABLES TO CHANGE PROFILE INPUT #############
#profDirInput = "/home/fireball2/fbground/fbground_v14/profiles/"

profDirInput = "/home/fireball2/fbground/fbground_v14/profiles/" 
outputDir    = "/home/fireball2/fbground/fbground_v14/focusFiles/"
#profDirInput = "/home/fireball2/skytesttf/"
#outputDir    = "/home/fireball2/skytesttf/focusFiles/"

regExInput   = "*prof.txt"
##############################################################

go()

