#include "CalibTracker/SiStripLorentzAngle/interface/ShallowLorentzAngleESProducer.h"
#include "CalibFormats/SiStripObjects/interface/SiStripGain.h" 
#include "CalibTracker/Records/interface/SiStripGainRcd.h"  
#include "RecoLocalTracker/SiStripClusterizer/interface/SiStripClusterInfo.h"
#include "CondFormats/SiStripObjects/interface/SiStripLorentzAngle.h"
#include "CondFormats/DataRecord/interface/SiStripLorentzAngleRcd.h"

using namespace edm;
using namespace reco;
using namespace std;

ShallowLorentzAngleESProducer::ShallowLorentzAngleESProducer(const edm::ParameterSet& iConfig)
  :  Suffix ( iConfig.getParameter<std::string>("Suffix") ),
     Prefix ( iConfig.getParameter<std::string>("Prefix") )
{
   produces <std::vector<unsigned int> >   ( Prefix + "rawid"               + Suffix );
   produces <std::vector<float> >          ( Prefix + "BdotY"               + Suffix );
   produces <std::vector<float> >          ( Prefix + "localB"              + Suffix );
   produces <std::vector<float> >          ( Prefix + "globalZofunitlocalY" + Suffix );  
   produces <std::vector<float> >          ( Prefix + "lorentzAngle"        + Suffix );  
   produces <std::vector<float> >          ( Prefix + "driftx"         + Suffix );
   produces <std::vector<float> >          ( Prefix + "drifty"         + Suffix );
   produces <std::vector<float> >          ( Prefix + "driftz"         + Suffix );
  
}

void ShallowLorentzAngleESProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   auto         rawid         = std::make_unique<std::vector<unsigned int>>   ();
   auto         BdotY         = std::make_unique<std::vector<float>>          ();
   auto         localB        = std::make_unique<std::vector<float>>          ();
   auto         globalZofunitlocalY = std::make_unique<std::vector<float>>    ();
   auto         lorentzAngle  = std::make_unique<std::vector<float>>          ();
   auto         driftx        = std::make_unique<std::vector<float>>          ();
   auto         drifty        = std::make_unique<std::vector<float>>          ();
   auto         driftz        = std::make_unique<std::vector<float>>          ();
  
  
   edm::ESHandle<TrackerGeometry> theTrackerGeometry;         iSetup.get<TrackerDigiGeometryRecord>().get( theTrackerGeometry );  
   edm::ESHandle<MagneticField> magfield;                     iSetup.get<IdealMagneticFieldRecord>().get(magfield);                      
   edm::ESHandle<SiStripLorentzAngle> SiStripLorentzAngle;    iSetup.get<SiStripLorentzAngleDepRcd>().get(SiStripLorentzAngle);      

   auto dets = theTrackerGeometry -> detsTIB();
   std::cout << "In TIB there are " << dets.size() << " detectors" << std::endl;
//   for ( size_t xx = 0; xx < dets.size() ; ++xx )
   for ( auto det : dets )
   {
      float    bdoty = -999;
      float    locb  = -999;
      float    globalzy = -999;
      float    larcd = -999;
      float    drfx = -999;
      float    drfy = -999;
      float    drfz = -999;
   
      const StripGeomDetUnit* theStripDet = dynamic_cast<const StripGeomDetUnit*>( theTrackerGeometry->idToDet( det->geographicalId () ) );
      int detid = det->geographicalId().rawId();
      if ( theStripDet ) 
      {
         bdoty      = (theStripDet->surface()).toLocal( magfield->inTesla(theStripDet->surface().position())).y();
         locb       = magfield->inTesla(theStripDet->surface().position()).mag() ;
         globalzy   = (theStripDet->toGlobal(LocalVector(0,1,0))).z();
         larcd      = SiStripLorentzAngle -> getLorentzAngle(detid);
         LocalVector drift = shallow::drift( theStripDet, *magfield, *SiStripLorentzAngle);
         drfx   = drift.x();
         drfy   = drift.y();
         drfz   = drift.z();
         // fill the vectors
         rawid ->push_back( detid );         
         BdotY->push_back( bdoty ); 
         localB->push_back( locb );
         driftx->push_back( drfx );
         drifty->push_back( drfy );
         driftz->push_back( drfz );
         globalZofunitlocalY->push_back( globalzy );
         lorentzAngle->push_back( larcd );
         
         
      }
   }

   iEvent.put(std::move(rawid),         Prefix + "rawid"         + Suffix );
   iEvent.put(std::move(BdotY),         Prefix + "BdotY"         + Suffix );
   iEvent.put(std::move(localB),        Prefix + "localB"        + Suffix );
   iEvent.put(std::move(driftx),        Prefix + "driftx"        + Suffix );
   iEvent.put(std::move(drifty),        Prefix + "drifty"        + Suffix );
   iEvent.put(std::move(driftz),        Prefix + "driftz"        + Suffix );
   iEvent.put(std::move(globalZofunitlocalY), Prefix + "globalZofunitlocalY" + Suffix );  
   iEvent.put(std::move(lorentzAngle), Prefix + "lorentzAngle" + Suffix );  
}

