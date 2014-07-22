# ==============================================================================

class MyPSimHit:
   """
My PSimHit class
"""
   _time_of_flight = 0.0
   _energy_loss = 0.0
   _det_unit_id = 0
   _track_id = 0 
   
# constructor
#   def __init__(self):

# set methods                    
   def set_time_of_flight(self,value):
      self._time_of_flight = value
      
   def set_energy_loss(self,value):
      self._energy_loss = value
      
   def det_unit_id(self,value):
      self._det_unit_id = value
      
   def track_id(self,value):
      self._track_id = value
      
# get methods
   def time_of_flight(self):
      return self._time_of_flight

   def energy_loss(self):
      return self._energy_loss

   def det_unit_id(self):
      return self._det_unit_id

   def track_id(self):
      return self._track_id

# ______________________________________________________________________________

