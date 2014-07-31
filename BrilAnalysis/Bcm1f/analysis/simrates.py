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
from analyzers import EdmSimAnalyzer

import ROOT

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
   pileup = options.pileup
   
   analysis = EdmSimAnalyzer(files_list,pileup)
   analysis.analyze()
   
   tof_hist = analysis.histograms()[0]
   
   c1 = ROOT.TCanvas()
   tof_hist.Draw()
   c1.Print ("tof_hist.png")


# ___________________________________________________________________________________

print TextColor.EXEC

if __name__ == '__main__':
   main()

print TextColor.ENDC
