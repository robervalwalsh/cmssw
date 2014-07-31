from data_model import SimHit
from data_model import SimpleHits

import ROOT
from DataFormats.FWLite import Events, Handle


class EdmSimAnalyzer:
   """
   My EDM simulation analyzer
"""
   
# constructor
   def __init__(self, files_list=None, pileup=0):
#      ''' https://cmssdt.cern.ch/SDT/doxygen/CMSSW_5_3_9/doc/html/d4/dab/classpython_1_1Events.html
#This is NOT a collection of fwlite::Event objects.
#The fwlite::Event object is obtained using the object() method (see below) '''
      self._events = Events(files_list)
      self._handle = Handle ('std::vector<PSimHit>')
      self._label = ('g4SimHits','BCM1FHits','SIM')
      self._pileup = pileup
      # Create histograms, etc.
      self._tof_hist = ROOT.TH1F ("tof", "time of flight", 100, 0, 100)
      
   def histograms(self):
      histograms = [self._tof_hist]
      return histograms 
            
   def analyze(self):
      pileup_counter = 0
      # loop over events
      simhits_bx = []  # list of simhits per bunch crossing
      for event in self._events:
         # use getByLabel, just like in cmsRun
         event.getByLabel (self._label, self._handle)
         psimhits = self._handle.product()
         for psimhit in psimhits:
            mysimhit = SimHit(psimhit)
            simhits_bx.append(mysimhit)
         if pileup_counter > self._pileup-1:
            if len(simhits_bx) > 0:
               hits = SimpleHits(simhits_bx)
               myhits = hits.emulated_hits()
            simhits_bx = []
            pileup_counter = 0
         pileup_counter += 1
      
