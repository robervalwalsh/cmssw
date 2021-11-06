// Last commit: $Id $

#ifndef CalibFormats_SiStripObjects_test_SiStripHashedDetId_H
#define CalibFormats_SiStripObjects_test_SiStripHashedDetId_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"


/**
   @class test_SiStripHashedDetId
   @author R.Bainbridge
   @brief Simple class that tests SiStripHashedDetId.
*/
class testSiStripHashedDetId : public edm::EDAnalyzer {
public:
  testSiStripHashedDetId(const edm::ParameterSet &);
  ~testSiStripHashedDetId();

  void initialize(edm::EventSetup const &);
  void analyze(const edm::Event &, const edm::EventSetup &);
  void endJob() { ; }
  
private:
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> geomToken_;
};

#endif  // CalibFormats_SiStripObjects_test_SiStripHashedDetId_H
