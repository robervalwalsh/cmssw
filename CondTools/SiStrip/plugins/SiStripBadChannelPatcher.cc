// system include files
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

// user include files
#include "CommonTools/ConditionDBWriter/interface/ConditionDBWriter.h"
#include "CondFormats/SiStripObjects/interface/SiStripBadStrip.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "CondFormats/DataRecord/interface/SiStripBadStripRcd.h"
#include "CondFormats/SiStripObjects/interface/SiStripBadStrip.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

class SiStripBadChannelPatcher : public edm::one::EDAnalyzer<> {
public:
  explicit SiStripBadChannelPatcher(const edm::ParameterSet& iConfig)
      : m_Record(iConfig.getParameter<std::string>("Record")), badStripToken_(esConsumes()) {}
  ~SiStripBadChannelPatcher() override = default;

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  std::unique_ptr<SiStripBadStrip> getNewObject(const edm::EventSetup& iSetup);

  // member data
  const std::string m_Record;
  const edm::ESGetToken<SiStripBadStrip, SiStripBadStripRcd> badStripToken_;
};

void SiStripBadChannelPatcher::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  std::unique_ptr<SiStripBadStrip> theBadStrips = this->getNewObject(iSetup);
  // write out the BadStrip record
  edm::Service<cond::service::PoolDBOutputService> poolDbService;

  if (poolDbService.isAvailable()) {
    poolDbService->writeOneIOV(*theBadStrips, poolDbService->currentTime(), m_Record);
  } else {
    throw std::runtime_error("PoolDBService required.");
  }
}

std::unique_ptr<SiStripBadStrip> SiStripBadChannelPatcher::getNewObject(const edm::EventSetup& iSetup) {
  edm::LogInfo("SiStripBadChannelPatcher") << "... creating dummy SiStripBadStrip Data" << std::endl;

  std::vector<uint32_t> detIdsToExclude = {
      369174220, 369174216, 369174204, 369174200, 369174212, 369174196, 369175268, 369174148, 369175220, 369175224,
      369175272, 369174152, 369175228, 369174156, 369175276, 369175288, 369175304, 369175308, 369175284, 369174164,
      369175300, 369175292, 369174172, 369174168, 369174184, 369174188, 369174180, 369175244, 369175260, 369175240,
      369175256, 369175252, 369175236, 369174232, 369174248, 369174236, 369174252, 369174228, 369174244};

  // this is the output object
  auto obj = std::make_unique<SiStripBadStrip>();

  std::vector<uint32_t> detid;
  const auto& payload = iSetup.getData(badStripToken_);
  payload.getDetIds(detid);

  for (uint32_t id = 0; id < detid.size(); id++) {
    if (std::count(detIdsToExclude.begin(), detIdsToExclude.end(), detid[id])) {
      std::cout << "I AM GOING TO EXCLUDE DETID: " << detid[id] << std::endl;
      continue;
    } else {
      std::cout << "I AM GOING TO KEEP DETID: " << detid[id] << std::endl;
    }

    SiStripBadStrip::Range range = payload.getRange(detid[id]);
    std::vector<unsigned int> theSiStripVector;
    unsigned int theBadStripRange;
    for (std::vector<unsigned int>::const_iterator badStrip = range.first; badStrip != range.second; ++badStrip) {
      unsigned short firstBadStrip = payload.decode(*badStrip).firstStrip;
      unsigned short NconsecutiveBadStrips = payload.decode(*badStrip).range;
      theBadStripRange = obj->encode(firstBadStrip, NconsecutiveBadStrips);
      theSiStripVector.push_back(theBadStripRange);
    }
    SiStripBadStrip::Range outRange(theSiStripVector.begin(), theSiStripVector.end());
    if (!obj->put(detid[id], outRange))
      edm::LogError("SiStripBadChannelPatcher")
          << "[SiStripBadChannelPatcher::analyze] detid already exists" << std::endl;
  }  // loop on the detids
  return obj;
}

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(SiStripBadChannelPatcher);
