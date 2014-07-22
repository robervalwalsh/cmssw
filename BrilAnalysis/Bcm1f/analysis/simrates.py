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
from utils import MyOptionParser
from utils import TextColor
from utils import get_list_of_files
from data_model import MyPSimHit

import ROOT
from DataFormats.FWLite import Events, Handle

ROOT.gROOT.SetBatch()
ROOT.gROOT.SetStyle('Plain') # white background

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
      for i in xrange(nhits):
         myhit = MyPSimHit ()
         myhit.set_time_of_flight(psimhits[i].timeOfFlight())
         tof_hist.Fill(myhit.time_of_flight())
      event_counter += 1
          
   c1 = ROOT.TCanvas()
   tof_hist.Draw()
   c1.Print ("tof_hist.png")


# ___________________________________________________________________________________

print TextColor.EXEC

if __name__ == '__main__':
   main()

print TextColor.ENDC
