# can run fwlite flag
fwlite = False
try:
   import DataFormats.FWLite
   fwlite = True
except ImportError:
   pass

# edm file flag
edm = False


