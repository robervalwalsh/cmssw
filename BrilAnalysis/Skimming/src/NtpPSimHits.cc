
#include "BrilAnalysis/Skimming/interface/NtpPSimHits.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

namespace bril {

   //
   // constructors and destructor
   //
   NtpPSimHits::NtpPSimHits(const edm::ParameterSet& iConfig)
   
   {
      //now do what ever initialization is needed
      simhits_ = iConfig.getParameter<edm::InputTag>  ( "PSimHits" );
   }
   
   
   NtpPSimHits::~NtpPSimHits()
   {
    
      // do anything here that needs to be done at desctruction time
      // (e.g. close files, deallocate resources etc.)
   
   }
   
   
   //
   // member functions
   //
   
   // ------------ method called for each event  ------------
   void
   NtpPSimHits::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
   {
      using namespace edm;
   
      Handle<PSimHitContainer> simhits;
      iEvent.getByLabel(simhits_, simhits);
//      const PSimHitContainer & simhits = (*simhits_handle);
      
      nhits_ = std::distance(simhits->begin(),simhits->end());
      for (PSimHitContainer::const_iterator simhit = simhits->begin(); simhit != simhits->end(); ++simhit)
      {
         int i = std::distance(simhits->begin(),simhit);
         time_of_flight_ [i]    = simhit -> timeOfFlight();
         energy_loss_    [i]    = simhit -> energyLoss();
         det_id_         [i]    = simhit -> detUnitId();
         particle_type_  [i]    = simhit -> particleType();
         process_type_   [i]    = simhit -> processType();
         track_id_       [i]    = simhit -> trackId();
         p_abs_          [i]    = simhit -> pabs();
         theta_at_entry_ [i]    = simhit -> thetaAtEntry().value();
         phi_at_entry_   [i]    = simhit -> phiAtEntry().value();
         entry_point_    [i][0] = simhit -> entryPoint().x();
         entry_point_    [i][1] = simhit -> entryPoint().y();
         entry_point_    [i][2] = simhit -> entryPoint().z();
         exit_point_     [i][0] = simhit -> exitPoint().x();
         exit_point_     [i][1] = simhit -> exitPoint().y();
         exit_point_     [i][2] = simhit -> exitPoint().z();
      }
   }
   
   
   // ------------ method called once each job just before starting event loop  ------------
   void 
   NtpPSimHits::beginJob( TTree* tree, edm::Service<TFileService>& fs )
   {
      // PSimHits info
      tree -> Branch("nhits",          &nhits_,         "nhits/I"); 
      tree -> Branch("time_of_flight", time_of_flight_, "time_of_flight[nhits]/F"); 
      tree -> Branch("energy_loss",    energy_loss_,    "energy_loss[nhits]/F"); 
      tree -> Branch("det_id",         det_id_,         "det_id[nhits]/I"); 
      tree -> Branch("particle_type",  particle_type_,  "particle_type[nhits]/I");
      tree -> Branch("process_type",   process_type_,   "process_type[nhits]/i");
      tree -> Branch("track_id",       track_id_,       "track_id[nhits]/i");
      tree -> Branch("p_abs",          p_abs_,          "p_abs[nhits]/F");
      tree -> Branch("theta_at_entry", theta_at_entry_, "theta_at_entry[nhits]/F");
      tree -> Branch("phi_at_entry",   phi_at_entry_,   "phi_at_entry[nhits]/F");
      tree -> Branch("entry_point",    entry_point_,    "entry_point[nhits][3]/F");
      tree -> Branch("exit_point",     exit_point_,     "exit_point[nhits][3]/F");
      
   }

}
