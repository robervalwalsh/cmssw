from ROOT import TChain
from ROOT import gDirectory

class MyEvents:

   def __init__(self, files_list=None, label=None,  **kwargs):
      self._events = TChain(label)
      for f in files_list:
         self._events.Add(f)
      self._entries = self._events.GetEntries()
      self._index = 0

   def __iter__ (self):
      return self
      
   def next(self):
      if self._index < self._entries:
         index = self._index
         # get tree (necessary?)
         ientry = self._events.LoadTree( index )
         if ientry < 0:
            raise StopIteration
         # copy entry into memory
         nb = self._events.GetEntry( index )
         if nb < 0:
            next
         self._index += 1
         return self._events
      else:
         raise StopIteration()
            
def main():
   files = ["../data/output1.root"]
   label = "Bcm1fNtuple/PSimHits"
   events = MyEvents(files,label)
   for event in events:
      for i in xrange(event.nhits):
         print event.time_of_flight[i]
   
#   for event in events:
#      print event
      
if __name__ == '__main__':
   main()


#events = TChain("Bcm1fNtuple/PSimHits")
#events.Add("../data/output1.root")
#events.Add("../data/output2.root")
#entries = events.GetEntriesFast()




#for jentry in xrange( entries ):
# # get the next tree in the chain and verify
#   ientry = events.LoadTree( jentry )
#   if ientry < 0:
#      break
#
# # copy next entry into memory and verify
#   nb = events.GetEntry( jentry )
#   if nb <= 0:
#      continue
#
# # use the values directly from the tree
#   nhits = int(events.nhits)
#   if nhits < 0:
#      continue
#      
#   for i in xrange( nhits ):
#      print events.time_of_flight[i]
