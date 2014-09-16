import os
import re
import glob
# https://docs.python.org/2/library/optparse.html
from optparse import OptionParser

import settings

das_client = False
try:
   __import__('imp').find_module('das_client')
   das_client = True
   # Make things with supposed existing module
except ImportError:
   pass

#import das_client  # for the cms dataset database

# ==============================================================================

class MyOptionParser:
   """
My option parser
"""
   def __init__(self):
       usage = "Usage: %prog [options]\n"
       usage += "For more help..."
       self.parser = OptionParser(usage=usage)
       input_help = "Give the input data with its type. Possible types are file: (default), dir: or dataset: (at DESY T2)."
       self.parser.add_option("--input", action="store", type="string", default="",
                              dest="input", help=input_help)
       input_format_help = "Give the input data format. Possible types are edm or ntuple."
       self.parser.add_option("--input_format", action="store", type="string", default="ntuple",
                              dest="input_format", help=input_format_help)
       nevents_help = "Number of events to be processed. Default = -1 (all events)"
       self.parser.add_option("--max_events", action="store", type="int", default=-1,
                              dest="max_events", help=nevents_help)
       pileup_help = "Number of pile-up events. Default = 0 (no pileup)"
       self.parser.add_option("--pileup", action="store", type="int", default=0,
                              dest="pileup", help=pileup_help)
       vdmscan_help = "Number of VdM scan points. Default = 1 (no scan)"
       self.parser.add_option("--n_scans", action="store", type="int", default=1,
                              dest="n_scans", help=vdmscan_help)
       bx_help = "Number of bunch crossings. Default = 1"
       self.parser.add_option("--bx", action="store", type="int", default=1,
                              dest="bx", help=vdmscan_help)
                              
   def help(self):
      print TextColor.HELP
      self.parser.print_help()
      print TextColor.EXEC
   
   def opt_status(self):
      options, args = self.parser.parse_args()
      if options.input == "" :
         print TextColor.FAIL + "*** error *** : You must provide an input." + TextColor.EXEC
         self.help()
         return 0
      intype = "file"  # default
      if ":" in options.input:
         intype = re.split(":",options.input)[0]
      if intype != "file" and  intype != "dir" and intype != "dataset" :
         print TextColor.FAIL + "*** error *** : Input type not recognized." + TextColor.EXEC
         self.help()
         return 0
      
      if intype == "dataset":   # dataset is always edm
         options.input_format = "edm"
      format = options.input_format
      settings.edm = format == "edm"
         
      
      if format != "edm" and format != "ntuple":
         print TextColor.FAIL + "*** error *** : Input format not recognized." + TextColor.EXEC
         self.help()
         return 0
      if format == "edm" and not settings.fwlite:
         print TextColor.FAIL + "*** error *** : FWLite must be setup to run on EDM files." + TextColor.EXEC
         self.help()
         return 0
         
      if intype == "dataset" and not das_client:
         print TextColor.FAIL + "*** error *** : Cannot load das_client.py. Check your environment settings." + TextColor.EXEC
         self.help()
         return 0
         
         
      return 1
   
   def get_opt(self):
       """
Returns parse list of options
"""
       return self.parser.parse_args()

# ______________________________________________________________________________

# Get the list of files to be analysed according to the input give by the user
def get_list_of_files( opt_input ):
   
   intype = "file"  # default
   indata = [opt_input]
   if ( ":" in opt_input ):
      split_input = re.split(":",opt_input)
      intype = split_input[0]
      indata = re.split(",",split_input[1])
   
   files_list = indata
   
   if intype == "dir" :
      files_list = glob.glob(indata[0]+"/*.root")
      
   if intype == "dataset" :
      dataset = indata[0]
      query = "file instance=prod/phys03 dataset=" + dataset
      # Using DAS client to retrieve the filenames
      das_client_command = "das_client.py --limit=0 --query='" + query +"'"
      # print "Running the command ", das_client_command, "..."
      filenames = os.popen(das_client_command).read().split('\n')
      # Full file name at the desy tier2
      site = "dcap://dcache-cms-dcap.desy.de//pnfs/desy.de/cms/tier2"
      files_list = [site+the_file for the_file in filenames[:-1]] # the last entry is empty!?
   
   return files_list

# ______________________________________________________________________________

# For the printouts
class TextColor:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    HELP = '\033[36m'
    EXEC = '\033[97m'

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''
        self.HELP = ''
        self.EXEC = ''

# ______________________________________________________________________________
