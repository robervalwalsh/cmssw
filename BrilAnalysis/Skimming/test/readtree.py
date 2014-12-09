from ROOT import TFile
from ROOT import gDirectory

# open the file
myfile = TFile( '../data/output.root' )

# retrieve the ntuple of interest
mychain = gDirectory.Get( 'Bcm1fNtuple/PSimHits' )
entries = mychain.GetEntriesFast()

for jentry in xrange( entries ):
 # get the next tree in the chain and verify
   ientry = mychain.LoadTree( jentry )
   if ientry < 0:
      break

 # copy next entry into memory and verify
   nb = mychain.GetEntry( jentry )
   if nb <= 0:
      continue

 # use the values directly from the tree
   nhits = int(mychain.nhits)
   if nhits < 0:
      continue
      
   for i in xrange( nhits ):
      print mychain.time_of_flight[i]
