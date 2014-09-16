from data_model import SimHit
from data_model import SimpleHits
from data_model import MyEvents
from data_model import PSimHitsCollection
from data_model import PSimHit

import settings

import ROOT


try:
   from DataFormats.FWLite import Events, Handle
except ImportError:
   pass

# ___________________________________________________________

def channel_characteristic(ch):
   channel_characteristic = channel_parity(ch)+"_"+channel_side(ch)+"_"+channel_plane(ch)
   return channel_characteristic

# ___________________________________________________________

def channel_parity(ch):
   parity = ""
   if ch % 2 == 0: parity = "even"
   else:           parity = "odd"
   return parity
   
# ___________________________________________________________

def channel_plane(ch):
   plane = ""
   vol = int(ch/1000.)
   if vol == 1: plane = "+z"
   else:        plane = "-z"
   return plane
   
# ___________________________________________________________

def channel_side(ch):
   side = ""
   vol = int(ch/1000.)
   dd = int((ch-vol*1000.)/10.)
   if dd <= 6: side = "far"
   else:       side = "near"
   return side
   
# ___________________________________________________________

def histograms_or(h1,h2,name=None):
   # todo: test if histograms have the same parameters, bin, range etc.
   nbins = h1.GetNbinsX()
   if name:
      h = h1.Clone(name)
   else:
      h = h1.Clone()
   h.Reset()
   for i in xrange(1,nbins+1):
      c1 = h1.GetBinContent(i)
      c2 = h2.GetBinContent(i)
      if c1 or c2:
         h.SetBinContent(i,1.0)
   return h
      
      

# ==========================================================

class SimAnalyzer:
   """
   My PSimHits simulation analyzer
"""
   
# ___________________________________________________________

# constructor
   def __init__(self, files_list=None, **kwargs):
#      ''' https://cmssdt.cern.ch/SDT/doxygen/CMSSW_5_3_9/doc/html/d4/dab/classpython_1_1Events.html
#This is NOT a collection of fwlite::Event objects.
#The fwlite::Event object is obtained using the object() method (see below) '''
      if not files_list:
         raise RuntimeError, "No input files given"
      self._max_events = -1
      if kwargs.has_key ('maxEvents'):
         self._max_events = kwargs['maxEvents']
         del kwargs['maxEvents']
      self._hist_add_type = ''
      if kwargs.has_key ('histAddType'):
         self._hist_add_type = kwargs['histAddType']
         del kwargs['histAddType']
      if len(kwargs):
         raise RuntimeError, "Unknown arguments %s" % kwargs
      self._handle = ''
      self._label = 'Bcm1fNtuple/PSimHits'
      if settings.edm:
         self._handle = Handle ('std::vector<PSimHit>')
         self._label = ('g4SimHits','BCM1FHits','SIM')
      self._pileup = 0
      self._bx = 1
      self._nscans = 1
      self._bx_space = 25.
      self._histograms = {}
      self._channels = []
      self._outputfile = "simrates_histograms.root"
      # Create histograms, etc.
      self.set_list_of_channels()
      self.set_histograms()
      # THE EVENTS!!!
      if settings.edm :
         self._events = Events(files_list,maxEvents=self._max_events)
      else:
         self._events = MyEvents(files_list,self._label,maxEvents=self._max_events)
   
# ___________________________________________________________

   def analyze(self):
      pileup_counter = 0
      scan_counter = 1
      bx_counter = 1
      bx_space = self._bx_space
      simhits_bx = []  # list of simhits per bunch crossing

      # loop over events
      for n,event in enumerate(self._events):
         if n>0 and n%10000==0: print n," events processed"
         if settings.edm:
            # use getByLabel, just like in cmsRun
            event.getByLabel (self._label, self._handle)
            psimhits = self._handle.product()
         else:
            psimhits = PSimHitsCollection(event)
         for psimhit in psimhits:
            mysimhit = SimHit(psimhit)
            tof = mysimhit.time_of_flight()
#            mysimhit.set_arrival_time(tof+bx_counter*self._bx_space)
            simhits_bx.append(mysimhit)
         if pileup_counter > self._pileup-1:  # reached the desired number of pile-up interactions
            if len(simhits_bx) > 0:
               hits = SimpleHits(simhits_bx, bx_counter, bx_space)
               self.fill_histograms(hits.emulated())  
            simhits_bx = []
            pileup_counter = 0  # reset pileup counter and...
            bx_counter += 1     # go to the next BX
         if bx_counter > self._bx:   # reached the desired number of bx in one orbit
            bx_counter = 1
         pileup_counter += 1
        
      # end of event loop
      self.save_histograms()


# ___________________________________________________________

   def fill_histograms(self,hits=None):
      h_aux_ch = {}  # auxiliar histograms
      for ch in self._channels:
         name_h = "temp"+str(ch)
         h_aux_ch[name_h] = self._histograms["time"][ch].Clone(name_h)
         h_aux_ch[name_h].Reset()
         
      for i,hit in enumerate(hits):
         ch = hit.channel()
         parity = channel_parity(ch)
         
#         if channel_characteristic(ch) == "even_far_-z":
#            print ch
         
         time = hit.arrival_time()
         eloss = hit.energy_loss()
         
         self._histograms["time"][ch].Fill(time)
         self._histograms["time"]["all"].Fill(time)
         self._histograms["time"][parity].Fill(time)
         
         self._histograms["eloss"][ch].Fill(eloss)
         self._histograms["eloss"]["all"].Fill(eloss)
         self._histograms["eloss"][parity].Fill(eloss)
         
         self._histograms["eloss_time"]["all"].Fill(eloss,time)
         self._histograms["eloss_time"][parity].Fill(eloss,time)
         
         h_aux_ch["temp"+str(ch)].Fill(time)
         
      if self._hist_add_type.lower() == 'or':
         # OR logics of main histos
         h_aux = {}    # auxiliar histograms
         for key in self._histograms["time"]:
           if "main_" in str(key):
              h_aux[key] = self._histograms["time"][key].Clone(key.replace("main","temp"))
              h_aux[key].Reset()
         for ch in self._channels:
            c_channel = channel_characteristic(ch)
            h = h_aux_ch["temp"+str(ch)]
            name_h = "main_"+c_channel+"_or"
            h_aux[name_h] = histograms_or(h_aux[name_h],h,name_h.replace("main","temp"))
         
         # Other OR logics derived from main
         # near_+z = odd_near_+z OR even_near_+z etc
         h_aux["near_+z_or"] = histograms_or(h_aux["main_odd_near_+z_or"] , h_aux["main_even_near_+z_or"], "temp_near_+z_or")
         h_aux["near_-z_or"] = histograms_or(h_aux["main_odd_near_-z_or"] , h_aux["main_even_near_-z_or"], "temp_near_-z_or")
         h_aux["far_+z_or"]  = histograms_or(h_aux["main_odd_far_+z_or"]  , h_aux["main_even_far_+z_or"] , "temp_far_+z_or")
         h_aux["far_-z_or"]  = histograms_or(h_aux["main_odd_far_-z_or"]  , h_aux["main_even_far_-z_or"] , "temp_far_-z_or")
         h_aux["odd_+z_or"]  = histograms_or(h_aux["main_odd_near_+z_or"] , h_aux["main_odd_far_+z_or"]  , "temp_odd_+z_or")
         h_aux["odd_-z_or"]  = histograms_or(h_aux["main_odd_near_-z_or"] , h_aux["main_odd_far_-z_or"]  , "temp_odd_-z_or")
         h_aux["even_+z_or"] = histograms_or(h_aux["main_even_near_+z_or"], h_aux["main_even_far_+z_or"] , "temp_even_+z_or")
         h_aux["even_-z_or"] = histograms_or(h_aux["main_even_near_-z_or"], h_aux["main_even_far_-z_or"] , "temp_even_-z_or")
         h_aux["+z_or"]      = histograms_or(h_aux["far_+z_or"]           , h_aux["near_+z_or"]          , "temp_+z_or")
         h_aux["-z_or"]      = histograms_or(h_aux["far_-z_or"]           , h_aux["near_-z_or"]          , "temp_-z_or")
         h_aux["odd_or"]     = histograms_or(h_aux["odd_+z_or"]           , h_aux["even_+z_or"]          , "temp_odd_or")
         h_aux["even_or"]    = histograms_or(h_aux["odd_-z_or"]           , h_aux["even_-z_or"]          , "temp_even_or")
         h_aux["all_or"]     = histograms_or(h_aux["+z_or"]               , h_aux["-z_or"]               , "temp_all_or")
   
         # sum to the accumulated histograms      
         for key in h_aux:
            self._histograms["time"][key].Add(h_aux[key])
         
#         for key in h_aux:
#            print h_aux[key].GetName()
#         print " =============== end of event ==========="

         
# ___________________________________________________________

   def save_histograms(self):
      f = ROOT.TFile(self._outputfile, "recreate")
      f.mkdir("general")
      if self._hist_add_type.lower() == 'or':
         f.mkdir("or_logics")
      f.mkdir("channels")
      for key1 in self._histograms.keys():
         for key2 in self._histograms[key1].keys():
            if type(key2) == int:
               f.cd("channels")
            elif "or" in key2:
               f.cd("or_logics")
            else:
               f.cd("general")
            self._histograms[key1][key2].Write()
      
#      for key in self._histograms["time"].keys():
#         self._histograms["time"][key].Write()
#         self._histograms["eloss"][key].Write()

      f.Write()
      f.Close()
      
# ___________________________________________________________

   def histograms(self):
      return self._histograms
      
# ___________________________________________________________

   def set_list_of_channels(self):
      for v in range(1,3):
         for dd in range(1,13):
            for p in range(1,3):
               self._channels += [v*1000+dd*10+p]
      
# ___________________________________________________________

   def list_of_channels(self):
      if not self._channels:
         self.set_list_of_channels()
      return self._channels      
   
# ___________________________________________________________

   def set_pileup(self,pileup=0):
      self._pileup = pileup
      
# ___________________________________________________________

   def set_bx(self,bx=1):
      self._bx = bx
      
# ___________________________________________________________

   def set_vdm(self,nscans=1):
      self._nscans = nscans
      
# ___________________________________________________________

   def set_bx_space(self,bx_space=25):
      self._bx_space = bx_space
      
# ___________________________________________________________

   def set_outputfile(self,filename):
      self._outputfile = filename
      
# ___________________________________________________________

   def set_histograms(self):
   
      bin = {}
#      bin["time"]  = {"n":14400,"min":0.,"max":90000.}  # 6.25ns binning
      bin["time"]  = {"n":90000,"min":0.,"max":90000.}
      bin["eloss"] = {"n":500,   "min":0.,"max":0.005}
      h_time = {}
      h_time["all"]       = ROOT.TH1F ("time_all",       "time in orbit (all channels)",       bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
      h_time["odd"]       = ROOT.TH1F ("time_odd",       "time in orbit (odd channels)",       bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
      h_time["even"]      = ROOT.TH1F ("time_even",      "time in orbit (even channels)",      bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])

      h_eloss = {}
      h_eloss["all"]  = ROOT.TH1F ("eloss_all",  "energy loss (all channels)",  bin["eloss"]["n"], bin["eloss"]["min"], bin["eloss"]["max"])
      h_eloss["odd"]  = ROOT.TH1F ("eloss_odd",  "energy loss (odd channels)",  bin["eloss"]["n"], bin["eloss"]["min"], bin["eloss"]["max"])
      h_eloss["even"] = ROOT.TH1F ("eloss_even", "energy loss (even channels)", bin["eloss"]["n"], bin["eloss"]["min"], bin["eloss"]["max"])

      h_eloss_time = {}
      h_eloss_time["all"]  = ROOT.TH2F ("eloss_time_all",  "energy loss x time in orbit (all channels)",  bin["eloss"]["n"], bin["eloss"]["min"], bin["eloss"]["max"] , 1125, 0, 900)
      h_eloss_time["odd"]  = ROOT.TH2F ("eloss_time_odd",  "energy loss x time in orbit (odd channels)",  bin["eloss"]["n"], bin["eloss"]["min"], bin["eloss"]["max"] , 1125, 0, 900)
      h_eloss_time["even"] = ROOT.TH2F ("eloss_time_even", "energy loss x time in orbit (even channels)", bin["eloss"]["n"], bin["eloss"]["min"], bin["eloss"]["max"] , 1125, 0, 900)

      for channel in self._channels:
         h_name  = "time_"+str(channel)
         h_title = "time in orbit (channel"+str(channel)+")"
         h_time[channel] = ROOT.TH1F (h_name, h_title, bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_name  = "eloss_"+str(channel)
         h_title = "energy loss (channel"+str(channel)+")"
         h_eloss[channel] = ROOT.TH1F (h_name, h_title, bin["eloss"]["n"], bin["eloss"]["min"], bin["eloss"]["max"])
         
      if self._hist_add_type.lower() == 'or':
         h_time["all_or"]    = ROOT.TH1F ("time_all_or",    "time in orbit (or all channels)",    bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["odd_or"]    = ROOT.TH1F ("time_odd_or",    "time in orbit (or odd channels)",    bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["even_or"]   = ROOT.TH1F ("time_even_or",   "time in orbit (or even channels)",   bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["+z_or"]     = ROOT.TH1F ("time_+z_or",     "time in orbit (or +z channels)",     bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["-z_or"]     = ROOT.TH1F ("time_-z_or",     "time in orbit (or -z channels)",     bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["odd_+z_or"] = ROOT.TH1F ("time_odd_+z_or", "time in orbit (or odd +z channels)", bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["odd_-z_or"] = ROOT.TH1F ("time_odd_-z_or", "time in orbit (or odd -z channels)", bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["even_+z_or"]= ROOT.TH1F ("time_even_+z_or","time in orbit (or even +z channels)",bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["even_-z_or"]= ROOT.TH1F ("time_even_-z_or","time in orbit (or even -z channels)",bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["far_+z_or"] = ROOT.TH1F ("time_far_+z_or", "time in orbit (or far +z channels)", bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["far_-z_or"] = ROOT.TH1F ("time_far_-z_or", "time in orbit (or far -z channels)", bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["near_+z_or"]= ROOT.TH1F ("time_near_+z_or","time in orbit (or near +z channels)",bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["near_-z_or"]= ROOT.TH1F ("time_near_-z_or","time in orbit (or near -z channels)",bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
   
         h_time["main_odd_far_+z_or"]  = ROOT.TH1F ("time_odd_far_+z_or",  "time in orbit (or odd far +z channels)",  bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["main_odd_far_-z_or"]  = ROOT.TH1F ("time_odd_far_-z_or",  "time in orbit (or odd far -z channels)",  bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["main_even_far_+z_or"] = ROOT.TH1F ("time_even_far_+z_or", "time in orbit (or even far +z channels)", bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["main_even_far_-z_or"] = ROOT.TH1F ("time_even_far_-z_or", "time in orbit (or even far -z channels)", bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["main_odd_near_+z_or"] = ROOT.TH1F ("time_odd_near_+z_or", "time in orbit (or odd near +z channels)", bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["main_odd_near_-z_or"] = ROOT.TH1F ("time_odd_near_-z_or", "time in orbit (or odd near -z channels)", bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["main_even_near_+z_or"]= ROOT.TH1F ("time_even_near_+z_or","time in orbit (or even near +z channels)",bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])
         h_time["main_even_near_-z_or"]= ROOT.TH1F ("time_even_near_-z_or","time in orbit (or even near -z channels)",bin["time"]["n"], bin["time"]["min"], bin["time"]["max"])

      self._histograms["time"] = h_time
      self._histograms["eloss"] = h_eloss
      self._histograms["eloss_time"] = h_eloss_time
   
# ___________________________________________________________

            
