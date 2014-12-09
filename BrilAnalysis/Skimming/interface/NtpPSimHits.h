#ifndef BRILANALYSIS_SKIMMING_NTPPSIMHITS_H_
#define BRILANALYSIS_SKIMMING_NTPPSIMHITS_H_ 1

// -*- C++ -*-
//
// Package:    BrilAnalysis/Skimming
// Class:      NtpPSimHits
// 
/**\class NtpPSimHits NtpPSimHits.cc BrilAnalysis/Skimming/src/NtpPSimHits.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Roberval Walsh Bastos Rangel
//         Created:  Mon, 14 Jul 2014 14:24:08 GMT
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

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TTree.h"

//
// class declaration
//

namespace bril {

   class NtpPSimHits {
      public:
         explicit NtpPSimHits(const edm::ParameterSet&);
         ~NtpPSimHits();
         virtual void beginJob( TTree* , edm::Service<TFileService>& );
         virtual void analyze(const edm::Event&, const edm::EventSetup&);
   
      private:
   
         // ----------member data ---------------------------
         
         int            nhits_ ;
         float          time_of_flight_  [1000];
         float          energy_loss_     [1000];
         int            det_id_          [1000];
         int            particle_type_   [1000];
         unsigned short process_type_    [1000];
         unsigned short track_id_        [1000];
         float          p_abs_           [1000];
         float          theta_at_entry_  [1000];
         float          phi_at_entry_    [1000];
         float          entry_point_     [1000][3];
         float          exit_point_      [1000][3];
         
         
         
         edm::InputTag simhits_;
   };

}

#endif  // BRILANALYSIS_SKIMMING_NTPPSIMHITS_H_
