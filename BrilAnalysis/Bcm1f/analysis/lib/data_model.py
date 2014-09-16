from utils import TextColor
from ROOT import TChain
from ROOT import gDirectory


def split_simhits_into_channels(simhits):
   simhits_ch = {}
   for h in simhits:
      channel = h.channel()
      if channel in simhits_ch:
         simhits_ch[channel] += [h]
      else:
         simhits_ch[channel] = [h]
   return simhits_ch
      

# ==============================================================================

class MyEvents:
   """
My ntuple events
"""
   def __init__(self, files_list=None, label=None,  **kwargs):
      if not files_list:
         raise RuntimeError, "No input files given"
      if not label:
         raise RuntimeError, "No directory given"
         
      self._max_events = -1
      if kwargs.has_key ('maxEvents'):
         self._max_events = kwargs['maxEvents']
         del kwargs['maxEvents']
      if len(kwargs):
         raise RuntimeError, "Unknown arguments %s" % kwargs
         
      self._events = TChain(label)
      for f in files_list:
         self._events.Add(f)
      self._entries = self._max_events
      if self._max_events < 0:
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
               
            

# ==============================================================================
class SimHit:
   """
My SimHit class
"""
# constructor
   def __init__(self, psimhit=None):
      if ( psimhit ):
         self.set_time_of_flight(psimhit.timeOfFlight()) 
         self.set_energy_loss(psimhit.energyLoss()) 
         self.set_channel(psimhit.detUnitId()) 
         self.set_pdg_id(psimhit.particleType()) 
         self.set_track_id(psimhit.trackId()) 

# set methods                    
   def set_time_of_flight(self,value):
      self._time_of_flight = value
      
   def set_energy_loss(self,value):
      self._energy_loss = value
      
   def set_channel(self,value):
      self._channel = value
      
   def set_track_id(self,value):
      self._track_id = value
      
   def set_pdg_id(self,value):
      self._track_id = value
      
# get methods
   def time_of_flight(self):
      return self._time_of_flight

   def energy_loss(self):
      return self._energy_loss

   def channel(self):
      return self._channel

   def track_id(self):
      return self._track_id

   def pdg_id(self):
      return self._pdg_id
# ______________________________________________________________________________

class Hit:
   """
My Hit class
"""
# constructor
   def __init__(self, time_in_orbit):
      self._time_of_flight = 0.
      self._arrival_time = 0.
      self._amplitude = 0.
      self._energy_loss = 0.
      self._channel = 0
      self._simhits = []
      self._time_in_orbit = time_in_orbit

# set methods                    
   def set_time_of_flight(self,value):
      self._time_of_flight = value
      
   def set_arrival_time(self,value):
      self._arrival_time = value
      
   def set_energy_loss(self,value):
      self._energy_loss = value
      
   def set_channel(self,value):
      self._channel = value
      
   def set_amplitude(self,value):
      self._amplitude = value
      
   def add_simhit(self,simhit):
      self._simhits.append(simhit)
      
   def add_simhits(self,simhits):
      self._simhits += simhits
      
   def clean_simhits(self):
      self._simhits = []
      
   def hit(self,simhits):
      hit = Hit(self._time_in_orbit)
      if simhits:
         self.clean_simhits()
         self.add_simhits(simhits)
      first_simhit = self._simhits[0]
      time = first_simhit.time_of_flight()
      channel = first_simhit.channel()
      total_energy = 0
      for simhit in self._simhits:
         total_energy += simhit.energy_loss()
      hit.set_time_of_flight(time)
      hit.set_arrival_time(time+self._time_in_orbit)
      hit.set_energy_loss(total_energy)
      hit.set_channel(channel)
      return hit
      
# get methods
   def time_of_flight(self):
      return self._time_of_flight

   def arrival_time(self):
      return self._arrival_time

   def energy_loss(self):
      return self._energy_loss

   def channel(self):
      return self._channel

   def amplitude(self):
      return self._amplitude

# ______________________________________________________________________________

class SimpleHits:
   """
My hits class, for emulated or reconstructed hits
"""
   # constructor
   def __init__(self,simhits,bx_counter, bx_space=25., settings=None):
      self._threshold = 0.
      self._deadtime = 10.
      self._simhits = simhits
      self._simhits_ch = split_simhits_into_channels(simhits)
      self._channels = self._simhits_ch.keys()
      self._bx_counter = bx_counter
      self._bx_space = bx_space
      # sanity checks
      if settings and type(settings) is dict:
         if 'threshold' in settings: self._threshold = settings['threshold']
         if 'deadtime'  in settings: self._deadtime  = settings['deadtime']
      if type(self._simhits) is not list:
         print TextColor.WARNING + "*** warning *** : List of simhits is not a list." + TextColor.EXEC
      else:
         if len(self._simhits) == 0:
            print TextColor.WARNING + "*** warning *** : List of simhits is empty." + TextColor.EXEC
   
   # get methods
   def emulated(self):
      if not self._simhits or len(self._simhits) == 0:
         print TextColor.WARNING + "*** warning *** : Cannot emulate hits without any simhits." + TextColor.EXEC
         return 0
         
      time_in_orbit = self._bx_counter*self._bx_space - 5.17
      hits = []
      for ch in self._channels:
         total_energy = 0
         # sorting the simhits list per channel in time_of_flight 
         simhits = sorted(self._simhits_ch[ch], key=lambda x: x.time_of_flight, reverse=False) 
         first_arrival = simhits[0].time_of_flight()
         previous_arrival = first_arrival

         simhits_list = []
         for i,simhit in enumerate(simhits):
            arrival = simhit.time_of_flight()
            arrival_consecutive = arrival - previous_arrival
            if arrival_consecutive < self._deadtime:
               simhits_list += [simhit]
            if arrival_consecutive >= self._deadtime or i == len(simhits)-1:
               hit = Hit(time_in_orbit)
               hits.append(hit.hit(simhits_list))
               simhits_list = []
            previous_arrival = arrival
         
      # end loop of channels
      return hits
      
# ______________________________________________________________________________

class PSimHit:
   """
My PSimHit class from flat ntuple
"""
# constructor
   def __init__(self):
      self._time_of_flight = -1.
      self._energy_loss = -1.
      self._channel = -1
      self._track_id = -1
      self._pdg_id = 0

# set methods                    
   def set_time_of_flight(self,value):
      self._time_of_flight = value
      
   def set_energy_loss(self,value):
      self._energy_loss = value
      
   def set_channel(self,value):
      self._channel = value
      
   def set_track_id(self,value):
      self._track_id = value
      
   def set_pdg_id(self,value):
      self._pdg_id = value
      
# get methods         
   def time_of_flight(self):
      return self._time_of_flight
      
   def energy_loss(self):
      return self._energy_loss
      
   def channel(self):
      return self._channel
      
   def track_id(self):
      return self._track_id
      
   def pdg_id(self):
      return self._pdg_id
      
# CMSSW PSimHit-like get methods
   def timeOfFlight(self):
      return self._time_of_flight
      
   def energyLoss(self):
      return self._energy_loss
      
   def detUnitId(self):
      return self._channel
      
   def trackId(self):
      return self._track_id
      
   def particleType(self):
      return self._pdg_id
      
# ==============================================================================

class PSimHitsCollection:

   def __init__(self, event=None, **kwargs):
      if not event:
         raise RuntimeError, "No TCahin event given"
      if len(kwargs):
         raise RuntimeError, "Unknown arguments %s" % kwargs
         
      self._event = event
      self._entries = event.nhits
      self._index = 0
      
   def __iter__ (self):
      return self
      
   def next(self):
      if self._index < self._entries:
         index = self._index
         
         psimhit = PSimHit()
         psimhit.set_time_of_flight(self._event.time_of_flight[index])
         psimhit.set_energy_loss(self._event.energy_loss[index])
         psimhit.set_channel(self._event.det_id[index])
         psimhit.set_track_id(self._event.track_id[index])
         psimhit.set_pdg_id(self._event.particle_type[index])
         
         self._index += 1
         return psimhit
      else:
         raise StopIteration()
         
     

