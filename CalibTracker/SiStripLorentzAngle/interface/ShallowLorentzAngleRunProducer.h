#ifndef SHALLOW_LORENTZANGLERUN_PRODUCER
#define SHALLOW_LORENTZANGLERUN_PRODUCER

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"



#include <ext/hash_map>


class ShallowLorentzAngleRunProducer : public edm::EDProducer
{
   public:
      explicit ShallowLorentzAngleRunProducer(const edm::ParameterSet&);
   private:
      std::string Suffix;
      std::string Prefix;
      
      bool newRun;

      void produce( edm::Event &, const edm::EventSetup & );
      void beginRun(edm::Run const&, edm::EventSetup const&);
      void endRun(edm::Run const&, edm::EventSetup const&);
      
      

};
#endif



