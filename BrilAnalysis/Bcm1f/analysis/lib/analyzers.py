from data_model import SimHit
from data_model import SimpleHits

import ROOT

fwlite = False
try:
   from DataFormats.FWLite import Events, Handle
   fwlite = True
except ImportError:
   pass

class EdmSimAnalyzer:
   """
   My EDM simulation analyzer
"""
   
# constructor
   def __init__(self, files_list=None, **kwargs):
#      ''' https://cmssdt.cern.ch/SDT/doxygen/CMSSW_5_3_9/doc/html/d4/dab/classpython_1_1Events.html
#This is NOT a collection of fwlite::Event objects.
#The fwlite::Event object is obtained using the object() method (see below) '''
      if not files_list:
         raise RuntimeError, "No input files given"
      self._max_events = -1
      if kwargs.has_key ('maxEvents'):
         self._max_events = kwargs['maxEvents']
         del kwargs['maxEvents']
      if len(kwargs):
         raise RuntimeError, "Unknown arguments %s" % kwargs
      self._handle = Handle ('std::vector<PSimHit>')
      self._label = ('g4SimHits','BCM1FHits','SIM')
      self._pileup = 0
      self._bx = 1
      self._bx_space = 25.
      self._histograms = {}
      self._channels = []
      self.set_list_of_channels()
      # Create histograms, etc.
      self.set_histograms(self._channels)
      # THE EVENTS!!!
      if fwlite:
         self._events = Events(files_list,maxEvents=self._max_events)
      else:
         self._events = None
   
   def set_list_of_channels(self):
      for v in range(1,3):
         for dd in range(1,13):
            for p in range(1,3):
               self._channels += [v*1000+dd*10+p]
      
   def list_of_channels(self):
      if not self._channels:
         self.set_list_of_channels()
      return self._channels
   
   def set_pileup(self,pileup=0):
      self._pileup = pileup
      
   def set_bx(self,bx=1):
      self._bx = bx
      
   def set_bx_space(self,bx_space=25):
      self._bx_space = bx_space
      
   def set_histograms(self,channels=None):
      h_time = {}
      h_eloss = {}
      for channel in channels:
         h_name  = "time_"+str(channel)
         h_title = "time in orbit (channel"+str(channel)+")"
         h_time[channel] = ROOT.TH1F (h_name, h_title, 112500, 0, 90000)
         h_name  = "eloss_"+str(channel)
         h_title = "energy loss (channel"+str(channel)+")"
         h_eloss[channel] = ROOT.TH1F (h_name, h_title, 100000, 0, 0.001)
      self._histograms["time"] = h_time
      self._histograms["eloss"] = h_eloss
   
   def histograms(self):
      return self._histograms
      
   def fill_histograms(self,hits=None):
      for hit in hits:
         ch = hit.channel()
         time = hit.time_of_flight()
         eloss = hit.energy_loss()
         self._histograms["time"][ch].Fill(time)
         self._histograms["eloss"][ch].Fill(eloss)
            
   def analyze(self):
      pileup_counter = 0
      bx_counter = 0
      # loop over events
      simhits_bx = []  # list of simhits per bunch crossing
      for n,event in enumerate(self._events):
         if n>0 and n%10000==0: print n," events processed"
         # use getByLabel, just like in cmsRun
         event.getByLabel (self._label, self._handle)
         psimhits = self._handle.product()
         for psimhit in psimhits:
            mysimhit = SimHit(psimhit)
            tof = mysimhit.time_of_flight()
            mysimhit.set_time_of_flight(tof+bx_counter*self._bx_space)
            simhits_bx.append(mysimhit)
         if pileup_counter > self._pileup-1:  # reached the desired number of pile-up interactions
            if len(simhits_bx) > 0:
               hits = SimpleHits(simhits_bx)
               self.fill_histograms(hits.emulated())  
            simhits_bx = []
            pileup_counter = 0  # reset pileup counter and...
            bx_counter += 1     # go to the next BX
         if bx_counter >= self._bx:   # reached the desired number of bx in one orbit
            bx_counter = 0
         pileup_counter += 1
         
      # end of event loop
      
