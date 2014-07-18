import os
import re
import glob
# https://docs.python.org/2/library/optparse.html
from optparse import OptionParser


import das_client  # for the cms dataset database


# ==============================================================================

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

# ______________________________________________________________________________

# Get the list of files to be analysed according to the input give by the user
def get_list_of_files( opt_input ):
   
   intype = "dataset"  # default
   indata = [opt_input]
   if ( ":" in opt_input ):
      split_input = re.split(":",opt_input)
      intype = split_input[0]
      indata = re.split(",",split_input[1])
   
   files_list = indata
   
   if intype == "dir" :
      files_list = glob.glob(indata[0]+"*.root")
      
   if intype == "dataset" :
      dataset = indata[0]
      query = "file dataset=" + dataset
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
