import os
import re
import glob

import das_client  # for the cms dataset database


# ==============================================================================

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
class text_color:
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
