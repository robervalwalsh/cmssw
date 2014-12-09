// -*- C++ -*-
//
// Package:    BrilAnalysis/Skimming
// Class:      BrilNtupleProducer
// 
/**\class BrilNtupleProducer BrilNtupleProducer.cc BrilAnalysis/Skimming/plugins/BrilNtupleProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Roberval Walsh Bastos Rangel
//         Created:  Tue, 15 Jul 2014 07:51:59 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "BrilAnalysis/Skimming/interface/NtpPSimHits.h"

#include <TH1.h>
#include <TFile.h>
#include <TTree.h>

//
// class declaration
//

class BrilNtupleProducer : public edm::EDAnalyzer {
   public:
      explicit BrilNtupleProducer(const edm::ParameterSet&);
      ~BrilNtupleProducer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      
      bool do_mc_;
      bool do_psimhits_;
      
      TTree * tree_psimhits_;
      
      bril::NtpPSimHits ntp_psimhits_;
      
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
BrilNtupleProducer::BrilNtupleProducer(const edm::ParameterSet& iConfig) : ntp_psimhits_(iConfig)

{
   //now do what ever initialization is needed
   do_mc_ = iConfig.getParameter<bool> (  "DoMC" );
   do_psimhits_ = iConfig.exists("PSimHits");

}


BrilNtupleProducer::~BrilNtupleProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
BrilNtupleProducer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   
   if ( do_mc_ )
   {
      if ( do_psimhits_ ) ntp_psimhits_.analyze(iEvent, iSetup);
      tree_psimhits_ -> Fill();
   }

}


// ------------ method called once each job just before starting event loop  ------------
void 
BrilNtupleProducer::beginJob()
{
   edm::Service<TFileService> fs;
   
   if ( do_mc_ )
   {
      tree_psimhits_ = fs->make<TTree>("PSimHits","PSimHits");
      if ( do_psimhits_ ) ntp_psimhits_.beginJob(tree_psimhits_, fs);
   }
}

// ------------ method called once each job just after ending the event loop  ------------
void 
BrilNtupleProducer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
BrilNtupleProducer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
BrilNtupleProducer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
BrilNtupleProducer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
BrilNtupleProducer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
BrilNtupleProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(BrilNtupleProducer);
