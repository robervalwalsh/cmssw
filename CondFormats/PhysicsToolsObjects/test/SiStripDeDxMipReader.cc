#include "CondFormats/PhysicsToolsObjects/test/SiStripDeDxMipReader.h"

#include <iostream>
#include <stdio.h>
#include <sys/time.h>

using namespace cms;

SiStripDeDxMipReader::SiStripDeDxMipReader(const edm::ParameterSet& iConfig)
//:  printdebug_(iConfig.getUntrackedParameter<uint32_t>("printDebug",1)){}
    : siStripDeDxMipToken_(esConsumes()) {}
    
SiStripDeDxMipReader::~SiStripDeDxMipReader() {}

void SiStripDeDxMipReader::analyze(const edm::Event& e, const edm::EventSetup& iSetup) {
  const auto & siStripDeDxMip = iSetup.get<SiStripDeDxMipRcd>().get(siStripDeDxMipToken_);
  
  
  edm::LogInfo("SiStripDeDxMipReader") << "[SiStripDeDxMipReader::analyze] End Reading SiStripDeDxMip" << std::endl;
  std::cout << siStripDeDxMip.numberOfBinsX() << "   " << siStripDeDxMip.numberOfBinsY() << std::endl;
  for (int ix = 0; ix < 300; ix++) {
    for (int iy = 0; iy < 1000; iy++) {
      std::cout << siStripDeDxMip.binContent(ix, iy) << " " << siStripDeDxMip.value(ix / 100., iy) << std::endl;
    }
  }

  //   std::vector<uint32_t> detid;
  //   siStripDeDxMip.getDetIds(detid);
  //   edm::LogInfo("Number of detids ")  << detid.size() << std::endl;

  //   if (printdebug_)
  //     for (size_t id=0;id<detid.size() && id<printdebug_;id++)
  //       {
  // 	SiStripDeDxMip::Range range=siStripDeDxMip.getRange(detid[id]);

  // 	int apv=0;
  // 	for(int it=0;it<range.second-range.first;it++){
  // 	  edm::LogInfo("SiStripDeDxMipReader")  << "detid " << detid[id] << " \t"
  // 					     << " apv " << apv++ << " \t"
  // 					     << siStripDeDxMip.getDeDxMip(it,range)     << " \t"
  // 					     << std::endl;
  // 	}
  //       }
}

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(SiStripDeDxMipReader);
