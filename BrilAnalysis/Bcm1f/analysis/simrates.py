#!/usr/bin/python2.6 -tt

import sys
import os
import json
import subprocess
import re
import glob

# https://docs.python.org/2/library/optparse.html
from optparse import OptionParser

sys.path.append( "lib" )
from utils import text_color
from utils import get_list_of_files

import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gROOT.SetBatch()
ROOT.gROOT.SetStyle('Plain') # white background

# ___________________________________________________________________________________

class MyOptionParser:
    """
My option parser
"""
    def __init__(self):
        usage = "Usage: %prog [options]\n"
        usage += "For more help..."
        self.parser = OptionParser(usage=usage)
        input_help = "Give the input data. Possible types are file:, dir: or dataset: (default, at DESY T2)."
        self.parser.add_option("--input", action="store", type="string", default="",
                               dest="input", help=input_help)
        nevents_help = "Number of events to be processed. Default = -1 (all events)"
        self.parser.add_option("--nevents", action="store", type="int", default=-1,
                               dest="nevents", help=nevents_help)
                               
    def help(self):
       print text_color.HELP
       self.parser.print_help()
       print text_color.EXEC
    
    def opt_status(self):
       options, args = self.parser.parse_args()
       if options.input == "" :
          print text_color.WARNING + "*** warning *** : You must provide an input." + text_color.EXEC
          self.help()
          return 0
       intype = "dataset"  # default
       if ( ":" in options.input ):
          intype = re.split(":",options.input)[0]
       if intype != "file" and  intype != "dir" and intype != "dataset" :
          print text_color.FAIL + "*** error *** : Input type not recognized." + text_color.EXEC
          self.help()
          return 0
       return 1
    
    def get_opt(self):
        """
Returns parse list of options
"""
        return self.parser.parse_args()

# ___________________________________________________________________________________

def main():

   # options from command line
   optmgr = MyOptionParser()
   if not optmgr.opt_status() :
      return 0
      
   options, _ = optmgr.get_opt() # tuple, unpack the return value and assign it the variable named to the left; _ single value to unpack
   
   # get list of files
   files_list = get_list_of_files( options.input )
   nevents = options.nevents
   
   event_counter = 0
#      ''' https://cmssdt.cern.ch/SDT/doxygen/CMSSW_5_3_9/doc/html/d4/dab/classpython_1_1Events.html
#This is NOT a collection of fwlite::Event objects.
#The fwlite::Event object is obtained using the object() method (see below) '''
   events = Events(files_list)
   handle = Handle ('std::vector<PSimHit>')
   label = ('g4SimHits','BCM1FHits','SIM')
   
   # Create histograms, etc.
   tof_hist = ROOT.TH1F ("tof", "time of flight", 100, 0, 100)   
   # loop over events
   for event in events:
      # use getByLabel, just like in cmsRun
      event.getByLabel (label, handle)
      psimhits = handle.product()
      nhits = len(psimhits)
      print event_counter, nhits
      for i in xrange(nhits):
         hit = psimhits[i]
         tof_hist.Fill(hit.timeOfFlight())
         print hit.timeOfFlight()
      event_counter += 1
          
   c1 = ROOT.TCanvas()
   tof_hist.Draw()
   c1.Print ("tof_hist.png")


# ___________________________________________________________________________________

print text_color.EXEC

if __name__ == '__main__':
   main()

print text_color.ENDC
