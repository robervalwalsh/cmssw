#ifndef SiStripDeDx2DReader_H
#define SiStripDeDx2DReader_H

// system include files
//#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CondFormats/PhysicsToolsObjects/interface/Histogram2D.h"
#include "CondFormats/DataRecord/interface/SiStripDeDxProton_2D_Rcd.h"

#include <iostream>
#include <stdio.h>
#include <sys/time.h>

class SiStripDeDx2DReader : public edm::EDAnalyzer {
public:
  explicit SiStripDeDx2DReader(const edm::ParameterSet&);
  ~SiStripDeDx2DReader();

  void analyze(const edm::Event&, const edm::EventSetup&);

private:
  //  uint32_t printdebug_;
  edm::ESGetToken<PhysicsTools::Calibration::VHistogramD2D, SiStripDeDxProton_2D_Rcd> siStripDeDx2DToken_;
};

SiStripDeDx2DReader::SiStripDeDx2DReader(const edm::ParameterSet& iConfig)
//:  printdebug_(iConfig.getUntrackedParameter<uint32_t>("printDebug",1)){}
    : siStripDeDx2DToken_(esConsumes()) {}

SiStripDeDx2DReader::~SiStripDeDx2DReader() {}

void SiStripDeDx2DReader::analyze(const edm::Event& e, const edm::EventSetup& iSetup) {
  const auto & siStripDeDx2D = iSetup.get<SiStripDeDxProton_2D_Rcd>().get(siStripDeDx2DToken_);
  edm::LogInfo("SiStripDeDx2DReader") << "[SiStripDeDx2DReader::analyze] End Reading SiStripDeDxProton_2D" << std::endl;

  for (int ihisto = 0; ihisto < 3; ihisto++) {
    std::cout << (siStripDeDx2D.vHist)[ihisto].numberOfBinsX() << "   "
              << (siStripDeDx2D.vHist)[ihisto].numberOfBinsY() << std::endl;

    for (int ix = 0; ix < 300; ix++) {
      for (int iy = 0; iy < 1000; iy++) {
        std::cout << (siStripDeDx2D.vHist)[ihisto].binContent(ix, iy) << " "
                  << (siStripDeDx2D.vHist)[ihisto].value(ix / 100., iy) << std::endl;
      }
    }

    std::cout << "Value = " << (siStripDeDx2D.vValues)[ihisto] << std::endl;
  }

  //   std::vector<uint32_t> detid;
  //   siStripDeDx2D.getDetIds(detid);
  //   edm::LogInfo("Number of detids ")  << detid.size() << std::endl;

  //   if (printdebug_)
  //     for (size_t id=0;id<detid.size() && id<printdebug_;id++)
  //       {
  // 	siStripDeDx2D::Range range=siStripDeDx2D.getRange(detid[id]);

  // 	int apv=0;
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  edm::LogInfo("SiStripDeDx2DReader")  << "detid " << detid[id] << " \t"
  // 					     << " apv " << apv++ << " \t"
  // 					     << siStripDeDx2D.getDeDx2D(it,range)     << " \t"
  // 					     << std::endl;
  // 	}
  //       }
}

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(SiStripDeDx2DReader);

#endif
