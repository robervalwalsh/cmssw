#include "CalibTracker/SiStripLorentzAngle/interface/ShallowLorentzAngleRunProducer.h"
#include "CalibTracker/Records/interface/SiStripGainRcd.h"  

#include "CalibTracker/SiStripCommon/interface/ShallowTools.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "CondFormats/SiStripObjects/interface/SiStripLorentzAngle.h"
#include "CondFormats/DataRecord/interface/SiStripLorentzAngleRcd.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"

#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"


using namespace edm;
using namespace reco;
using namespace std;

ShallowLorentzAngleRunProducer::ShallowLorentzAngleRunProducer(const edm::ParameterSet& iConfig)
  :  Suffix ( iConfig.getParameter<std::string>("Suffix") ),
     Prefix ( iConfig.getParameter<std::string>("Prefix") )
{
   produces <std::vector<unsigned int>,edm::Transition::EndRun >   ( Prefix + "rawid"               + Suffix );
   produces <std::vector<float>,edm::Transition::EndRun >          ( Prefix + "BdotY"               + Suffix );
   produces <std::vector<float>,edm::Transition::EndRun >          ( Prefix + "localB"              + Suffix );
   produces <std::vector<float>,edm::Transition::EndRun >          ( Prefix + "globalZofunitlocalY" + Suffix );  
   produces <std::vector<float>,edm::Transition::EndRun >          ( Prefix + "lorentzAngle"        + Suffix );  
   produces <std::vector<float>,edm::Transition::EndRun >          ( Prefix + "driftx"         + Suffix );
   produces <std::vector<float>,edm::Transition::EndRun >          ( Prefix + "drifty"         + Suffix );
   produces <std::vector<float>,edm::Transition::EndRun >          ( Prefix + "driftz"         + Suffix );
  
}

ShallowLorentzAngleRunProducer::~ShallowLorentzAngleRunProducer()
{
}


void ShallowLorentzAngleRunProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
//   if ( ! newRun ) return;

   
//   newRun = false;
}

// ------------ method called when starting to processes a run  ------------

// ------------ method called when ending the processing of a run  ------------
void ShallowLorentzAngleRunProducer::endRunProduce(edm::Run & run, edm::EventSetup const& iSetup)
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
   dets.insert(dets.end(),(theTrackerGeometry -> detsTOB()).begin(),(theTrackerGeometry -> detsTOB()).end());
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

//    run.put(std::move(rawid),         Prefix + "rawid"         + Suffix );
//    run.put(std::move(BdotY),         Prefix + "BdotY"         + Suffix );
//    run.put(std::move(localB),        Prefix + "localB"        + Suffix );
//    run.put(std::move(driftx),        Prefix + "driftx"        + Suffix );
//    run.put(std::move(drifty),        Prefix + "drifty"        + Suffix );
//    run.put(std::move(driftz),        Prefix + "driftz"        + Suffix );
//    run.put(std::move(globalZofunitlocalY), Prefix + "globalZofunitlocalY" + Suffix );  
//    run.put(std::move(lorentzAngle), Prefix + "lorentzAngle" + Suffix );  
   
}

