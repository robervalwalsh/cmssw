#include <iostream>
#include <regex>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


#include "SiStripLAMonitor.h"
#include "SiStripLAMonitorConfig.h"

#include "DataFormats/SiStripDetId/interface/TOBDetId.h"
#include "DataFormats/SiStripDetId/interface/TIBDetId.h"

#include "TFile.h" 
#include "TFileCollection.h"
#include "TVector3.h"
#include "TString.h"

// =============================================================================================   

int main(int argc, char * argv[])
{
   
   if ( Init(argc,argv) == -1 )
   {
      std::cout << "something is wrong with the configuration file" << std::endl;
      return -1;
   }
   AnalyzeTheTree();
   // GetLorentzAngle(std::string method);
   WriteOutputs(saveHistos_);
   
   return 0;
}


void ProcessTheModule(const unsigned int & i)
{
   const SiStripDetId detid(rawid_->at(i));
   std::string subdet = "";
   if ( detid.subDetector() == SiStripDetId::TIB ) subdet = "TIB";
   if ( detid.subDetector() == SiStripDetId::TOB ) subdet = "TOB";
   if ( subdet != "TIB" && subdet != "TOB" ) return;
   unsigned int layer = TIBDetId(detid()).layer();
   std::string type  = (detid.stereo() ? "s": "a");
   std::string hprefix = Form("%s_L%d%s",subdet.c_str(),layer,type.c_str());
   
   TVector3 localdir(localdirx_->at(i),localdiry_->at(i),localdirz_->at(i));
   
   float tantheta = TMath::Tan(localdir.Theta());
   float cosphi   = TMath::Cos(localdir.Phi());
   
   float nstrips  = nstrips_->at(i);
   
   h1_[Form("%s_nstrips"    ,hprefix.c_str())] -> Fill(nstrips);
   h1_[Form("%s_tanthetatrk",hprefix.c_str())] -> Fill(tantheta);
   h1_[Form("%s_cosphitrk"  ,hprefix.c_str())] -> Fill(cosphi);
   
   h2_[Form("%s_tanthetatrk_nstrip",hprefix.c_str())] -> Fill(tantheta,nstrips);
   h2_[Form("%s_cosphitrk_nstrip",hprefix.c_str())] -> Fill(cosphi,nstrips);
   h2_[Form("%s_tanthcosphtrk_nstrip",hprefix.c_str())] -> Fill(cosphi*tantheta,nstrips);
   
}

int Init(int argc, char * argv[])
{
   // read configuration
   if ( SiStripLAMonitorConfig(argc, argv) != 0 ) return -1;
   
   //
   nlayers_["TIB"] = 4;
   nlayers_["TOB"] = 6;
   modtypes_.push_back("s");
   modtypes_.push_back("a");
   

   // prepare histograms
   for ( auto & layers : nlayers_)
   {
      std::string subdet = layers.first;
      for ( int l = 1; l <= layers.second; ++l )
      {
         for ( auto & t : modtypes_ )
         {
            std::string hprefix = Form("%s_L%d%s",subdet.c_str(),l,t.c_str());
            //std::cout << "preparing histograms for " << hprefix << std::endl;
            h1_[Form("%s_nstrips"    ,hprefix.c_str())] = new TH1F (Form("%s_nstrips",hprefix.c_str()),     "", 20,0,20);
            h1_[Form("%s_tanthetatrk",hprefix.c_str())] = new TH1F (Form("%s_tanthetatrk",hprefix.c_str()), "", 100,-2.5,2.5);
            h1_[Form("%s_cosphitrk",hprefix.c_str())]   = new TH1F (Form("%s_cosphitrk",hprefix.c_str()), "", 40,-1,1);
            
            h2_[Form("%s_tanthetatrk_nstrip",hprefix.c_str())] = new TH2F (Form("%s_tanthetatrk_nstrip",hprefix.c_str()), "", 100,-2.5,2.5,20,0,20);
            h2_[Form("%s_cosphitrk_nstrip",hprefix.c_str())] = new TH2F (Form("%s_cosphitrk_nstrip",hprefix.c_str()), "", 40,-1,1,20,0,20);
            h2_[Form("%s_tanthcosphtrk_nstrip",hprefix.c_str())] = new TH2F (Form("%s_tanthcosphtrk_nstrip",hprefix.c_str()), "", 100,-2.5,2.5,20,0,20);
         }
      }
   }
   
   // add the run number to the output file(s)
   outputfile_ = std::regex_replace( outputfile_, std::regex(".root"), Form("_%d.root",run_) );
   
   return 0;
}

void ProcessTheEvent()
{
   for ( size_t i = 0 ; i < rawid_->size(); ++i ) // loop over modules
   {
      // do whatever pre-selection needed
      ProcessTheModule(i);
   }
   
}

void AnalyzeTheTree()
{
   int count_entries = 0;
   bool terminate = false;
   fs::path calibtree_path(calibTreeDir_);
   if ( fs::exists(calibtree_path) )
   {
      if ( fs::is_directory(calibtree_path) )
      {
         for (fs::directory_entry& x : fs::directory_iterator(calibtree_path))
         {
            std::string fileprefix = Form("calibTree_%d",run_);
            std::string filename = x.path().filename().string();
            if ( filename.find(fileprefix) == std::string::npos || filename.find(".root") == std::string::npos ) continue;
            
//            TFile * f = TFile::Open("root://cms-xrd-global.cern.ch//store/group/dpg_tracker_strip/comm_tracker/Strip/Calibration/calibrationtree/GR17_Aag/calibTree_302031_50.root","OLD");
            TFile * f = TFile::Open(x.path().string().c_str(),"OLD");
            std::string tree_path = Form("gainCalibrationTree%s/tree",calibrationMode_.c_str());
            TTree * tree = (TTree*) f->Get(tree_path.c_str());
            
            // event data
            tree -> SetBranchAddress((eventPrefix_ + "event" + eventSuffix_).c_str(), &eventnumber_ );
            tree -> SetBranchAddress((eventPrefix_ + "run"   + eventSuffix_).c_str(), &runnumber_   );
            tree -> SetBranchAddress((eventPrefix_ + "lumi"  + eventSuffix_).c_str(), &luminumber_  );
         
            // calib data
            tree -> SetBranchAddress((calibPrefix_ + "trackindex" + calibSuffix_).c_str(), &trackindex_ );
            tree -> SetBranchAddress((calibPrefix_ + "rawid"      + calibSuffix_).c_str(), &rawid_      );
            tree -> SetBranchAddress((calibPrefix_ + "nstrips"    + calibSuffix_).c_str(), &nstrips_    );
            tree -> SetBranchAddress((calibPrefix_ + "localdirx"  + calibSuffix_).c_str(), &localdirx_  );
            tree -> SetBranchAddress((calibPrefix_ + "localdiry"  + calibSuffix_).c_str(), &localdiry_  );
            tree -> SetBranchAddress((calibPrefix_ + "localdirz"  + calibSuffix_).c_str(), &localdirz_  );
            
            // track data
            tree -> SetBranchAddress((trackPrefix_ + "trackpt"        + trackSuffix_).c_str(), &trackpt_        );
            tree -> SetBranchAddress((trackPrefix_ + "tracketa"       + trackSuffix_).c_str(), &tracketa_       );
            tree -> SetBranchAddress((trackPrefix_ + "trackhitsvalid" + trackSuffix_).c_str(), &trackhitsvalid_ );
            tree -> SetBranchAddress((trackPrefix_ + "trackchi2ndof"  + trackSuffix_).c_str(), &trackchi2ndof_  );
            
            // loop the events
            unsigned int nentries = tree->GetEntries();
            for (unsigned int ientry = 0; ientry < nentries; ientry++)
            {
               ++count_entries;
               if ( count_entries%100 == 0 ) std::cout << "Processed " << count_entries << "..." << std::endl;
               if ( nentriesmax_ > 0 && count_entries > nentriesmax_ )
               {
                  terminate = true;
                  break;
               }
               tree->GetEntry(ientry);
               ProcessTheEvent();
            } // end of events loop
            if ( terminate ) break;
                     
         } // end of file list loop
      }
   }
   
   
}

void WriteOutputs(const bool & savehistos)
{
   if ( ! savehistos ) return;
   TFile out(outputfile_.c_str(),"RECREATE");
   for ( auto h : h1_ )
   {
      if ( h.second -> GetEntries() == 0 ) continue;
      h.second -> Write();
   }
   for ( auto h : h2_ )
   {
      if ( h.second -> GetEntries() == 0 ) continue;
      h.second -> Write();
      TProfile * hp = (TProfile*) h.second -> ProfileX();
      hp -> Write();
   }
}
