#ifndef SHALLOW_LORENTZANGLERUN_PRODUCER
#define SHALLOW_LORENTZANGLERUN_PRODUCER

#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"



#include <ext/hash_map>


class ShallowLorentzAngleRunProducer : public edm::one::EDProducer<edm::EndRunProducer>
{
   public:
      explicit ShallowLorentzAngleRunProducer(const edm::ParameterSet&);
      ~ShallowLorentzAngleRunProducer() override;
   private:
      std::string Suffix;
      std::string Prefix;
      
//      bool newRun;

      void produce( edm::Event &, const edm::EventSetup & ) override;
      void endRunProduce(edm::Run&, edm::EventSetup const&) override;
      
      

};
#endif



