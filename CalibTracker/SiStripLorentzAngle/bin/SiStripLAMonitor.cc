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
      std::cout << "*** SiStripLAMonitor ***: -errors- Please check your configuration file" << std::endl;
      return -1;
   }
   AnalyzeTheTree();
   // GetLorentzAngle(std::string method);
   WriteOutputs(saveHistos_);
   
   return 0;
}

void ProcessTheEvent()
{
   for ( size_t i = 0 ; i < rawid_->size(); ++i ) // loop over modules
   {
      // do whatever pre-selection needed
      int itrk = trackindex_->at(i);
      if ( ptmin_ > 0 )
      {
         if ( trackpt_->at(itrk) < ptmin_ ) continue;
      }
      if ( ptmax_ > 0 )
      {
         if ( trackpt_->at(itrk) > ptmax_ ) continue;
      }
      if ( etamin_ > -2.5 )
      {
         if ( tracketa_->at(itrk) < etamin_ ) continue;
      }
      if ( etamax_ < 2.5 )
      {
         if ( tracketa_->at(itrk) > etamax_ ) continue;
      }
      if ( hitsvalmin_ > 0 )
      {
         if ( int(trackhitsvalid_->at(itrk)) < hitsvalmin_ ) continue;
      }
      if ( chi2ndfmax_ > 0 )
      {
         if ( trackchi2ndof_->at(itrk) > chi2ndfmax_ ) continue;
      }
      
      if ( fabs(tracketa_->at(itrk)) > 0.2 ) continue;
      // process info
      ProcessTheModule(i);
   }
   
}


void ProcessTheModule(const unsigned int & i)
{
   unsigned int mod = rawid_->at(i);
   std::string locationtype = ModuleLocationType(mod);
   
   if ( locationtype == "" ) return;
   
   TVector3 localdir(localdirx_->at(i),localdiry_->at(i),localdirz_->at(i));
   int sign = orientation_[mod];
   float tantheta = sign*TMath::Tan(localdir.Theta());
   float cosphi   = TMath::Cos(localdir.Phi());
   
   unsigned short nstrips  = nstrips_->at(i);
   float variance = variance_->at(i);
   
   h1_[Form("%s_nstrips"    ,locationtype.c_str())] -> Fill(nstrips);
   h1_[Form("%s_tanthetatrk",locationtype.c_str())] -> Fill(sign*tantheta);
   h1_[Form("%s_cosphitrk"  ,locationtype.c_str())] -> Fill(cosphi);
   
   
   h2_[Form("%s_tanthcosphtrk_nstrip",locationtype.c_str())] -> Fill(sign*cosphi*tantheta,nstrips);
   if ( nstrips == 2 )
   {
      h1_[Form("%s_variance_w2"    ,locationtype.c_str())] -> Fill(variance);
      h2_[Form("%s_tanthcosphtrk_var2",locationtype.c_str())] -> Fill(sign*cosphi*tantheta,variance);
      h2_ct_var2_m_[mod] -> Fill(sign*cosphi*tantheta,variance);
   }
   if ( nstrips == 3 )
   {
      h1_[Form("%s_variance_w3"    ,locationtype.c_str())] -> Fill(variance);
      h2_[Form("%s_tanthcosphtrk_var3",locationtype.c_str())] -> Fill(sign*cosphi*tantheta,variance);
      h2_ct_var3_m_[mod] -> Fill(sign*cosphi*tantheta,variance);
   }
   
   h2_ct_w_m_[mod] -> Fill(sign*cosphi*tantheta,nstrips);
   
   
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
         for (fs::directory_entry& ls_file : fs::directory_iterator(calibtree_path))
         {
            std::string fileprefix = Form("calibTree_%d",run_);
            std::string filename = ls_file.path().filename().string();
            if ( filename.find(fileprefix) == std::string::npos || filename.find(".root") == std::string::npos ) continue;
            
            std::cout << "Working on file : " << ls_file.path().string() << std::endl;
            std::cout << "..." << std::endl;
            
//            TFile * f = TFile::Open("root://cms-xrd-global.cern.ch//store/group/dpg_tracker_strip/comm_tracker/Strip/Calibration/calibrationtree/GR17_Aag/calibTree_302031_50.root","OLD");
            TFile * f = TFile::Open(ls_file.path().string().c_str(),"OLD");
            
            // INFO TREE - should be called once per run, one file is enough. Fill maps with information for each module
            std::string infotree_path = Form("%s/tree",infoTreePath_.c_str());
            if ( orientation_.empty() )
            {
               TTree * infotree = (TTree*) f->Get(infotree_path.c_str());
               InfoTreeBranches(infotree);
            }
            // CALIB TREE
            std::string tree_path = Form("gainCalibrationTree%s/tree",calibrationMode_.c_str());
            TTree * tree = (TTree*) f->Get(tree_path.c_str());
            
            CalibTreeBranches(tree);
            
            // loop the events
            unsigned int nentries = tree->GetEntries();
            for (unsigned int ientry = 0; ientry < nentries; ientry++)
            {
               ++count_entries;
//               if ( count_entries%100 == 0 ) std::cout << "Processed " << count_entries << "..." << std::endl;
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
      if ( saveHistosProfile_ )
      {
         TProfile * hp = (TProfile*) h.second -> ProfileX();
         hp -> Write();
      }
   }
   if ( saveHistosMods_ )
   {
      for ( int i = 1 ; i <= nlayers_["TIB"]; ++i )
         out.mkdir(Form("modules/TIB/L%d",i));
      for ( int i = 1 ; i <= nlayers_["TOB"]; ++i )
         out.mkdir(Form("modules/TOB/L%d",i));
      for ( auto h : h2_ct_w_m_ )
      {
         if ( h.second -> GetEntries() == 0 ) continue;
         WriteOutputsModules(out,h.second);
         
//          if ( std::string(h.second->GetName()).find("TIB") != std::string::npos )
//          {
//             for ( int i = 1 ; i <= nlayers_["TIB"]; ++i )
//             {
//                if ( std::string(h.second->GetName()).find(Form("TIB_L%d",i)) != std::string::npos )
//                out.cd(Form("modules/TIB/L%d",i));
//             }
//          }
//          if ( std::string(h.second->GetName()).find("TOB") != std::string::npos )
//          {
//             for ( int i = 1 ; i <= nlayers_["TOB"]; ++i )
//             {
//                if ( std::string(h.second->GetName()).find(Form("TOB_L%d",i)) != std::string::npos )
//                out.cd(Form("modules/TOB/L%d",i));
//             }
//          }
//          h.second -> Write();
//          if ( saveHistosProfile_ )
//          {
//             TProfile * hp = (TProfile*) h.second -> ProfileX();
//             hp -> Write();
//          }
      }
      for ( auto h : h2_ct_var2_m_ )
      {
         if ( h.second -> GetEntries() == 0 ) continue;
         WriteOutputsModules(out,h.second);
      }
      for ( auto h : h2_ct_var3_m_ )
      {
         if ( h.second -> GetEntries() == 0 ) continue;
         WriteOutputsModules(out,h.second);
      }
   }
   out.Close();

}

void WriteOutputsModules(TFile & out, TH2F * h)
{
         if ( std::string(h->GetName()).find("TIB") != std::string::npos )
         {
            for ( int i = 1 ; i <= nlayers_["TIB"]; ++i )
            {
               if ( std::string(h->GetName()).find(Form("TIB_L%d",i)) != std::string::npos )
               out.cd(Form("modules/TIB/L%d",i));
            }
         }
         if ( std::string(h->GetName()).find("TOB") != std::string::npos )
         {
            for ( int i = 1 ; i <= nlayers_["TOB"]; ++i )
            {
               if ( std::string(h->GetName()).find(Form("TOB_L%d",i)) != std::string::npos )
               out.cd(Form("modules/TOB/L%d",i));
            }
         }
         h -> Write();
         if ( saveHistosProfile_ )
         {
            TProfile * hp = (TProfile*) h -> ProfileX();
            hp -> Write();
         }
   
}

void CalibTreeBranches(TTree * tree)
{
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
   tree -> SetBranchAddress((calibPrefix_ + "variance"   + calibSuffix_).c_str(), &variance_   );

   // track data
   tree -> SetBranchAddress((trackPrefix_ + "trackpt"        + trackSuffix_).c_str(), &trackpt_        );
   tree -> SetBranchAddress((trackPrefix_ + "tracketa"       + trackSuffix_).c_str(), &tracketa_       );
   tree -> SetBranchAddress((trackPrefix_ + "trackhitsvalid" + trackSuffix_).c_str(), &trackhitsvalid_ );
   tree -> SetBranchAddress((trackPrefix_ + "trackchi2ndof"  + trackSuffix_).c_str(), &trackchi2ndof_  );
}

void InfoTreeBranches(TTree * tree)
{
   tree -> SetBranchAddress("rawid", &inforawid_);
   tree -> SetBranchAddress("globalZofunitlocalY", &infoglobalZofunitlocalY_);
   tree->GetEntry(0);
   for ( size_t im = 0; im < inforawid_->size() ; ++im )
   {
      int mod = inforawid_->at(im);
      orientation_[mod] = infoglobalZofunitlocalY_->at(im) < 0 ? -1 : 1;
      // histograms for each module
      h2_ct_w_m_[mod] = new TH2F (Form("ct_w_m_%s_%d",ModuleLocationType(mod).c_str(),mod),"",300,-1.5,1.5,20,0,20);	
      h2_ct_var2_m_[mod] = new TH2F (Form("ct_var2_m_%s_%d",ModuleLocationType(mod).c_str(),mod),"",300,-1.5,1.5,100,0,1);	
      h2_ct_var3_m_[mod] = new TH2F (Form("ct_var3_m_%s_%d",ModuleLocationType(mod).c_str(),mod),"",300,-1.5,1.5,100,0,1);	
   }
}               


std::string ModuleLocationType(const unsigned int & mod)
{

  const SiStripDetId detid(mod);
  std::string subdet = "";
  unsigned int layer = 0;
  if ( detid.subDetector() == SiStripDetId::TIB )
  {
     subdet = "TIB";
     layer = TIBDetId(detid()).layer();
  }
  if ( detid.subDetector() == SiStripDetId::TOB )
  {
     subdet = "TOB";
     layer = TOBDetId(detid()).layer();
  }
  
  std::string type  = (detid.stereo() ? "s": "a");
  std::string d_l_t = Form("%s_L%d%s",subdet.c_str(),layer,type.c_str());
  
  if ( layer == 0 ) return subdet;
  
  return d_l_t;


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
            std::string locationtype = Form("%s_L%d%s",subdet.c_str(),l,t.c_str());
            //std::cout << "preparing histograms for " << locationtype << std::endl;
            h1_[Form("%s_nstrips"    ,locationtype.c_str())] = new TH1F (Form("%s_nstrips",locationtype.c_str()),     "", 20,0,20);
            h1_[Form("%s_tanthetatrk",locationtype.c_str())] = new TH1F (Form("%s_tanthetatrk",locationtype.c_str()), "", 300,-1.5,1.5);
            h1_[Form("%s_cosphitrk",locationtype.c_str())]   = new TH1F (Form("%s_cosphitrk",locationtype.c_str()), "", 40,-1,1);
            h1_[Form("%s_variance_w2" ,locationtype.c_str())] = new TH1F (Form("%s_variance_w2",locationtype.c_str()),     "", 100,0,1);
            h1_[Form("%s_variance_w3" ,locationtype.c_str())] = new TH1F (Form("%s_variance_w3",locationtype.c_str()),     "", 100,0,1);
            
            h2_[Form("%s_tanthetatrk_nstrip",locationtype.c_str())] = new TH2F (Form("%s_tanthetatrk_nstrip",locationtype.c_str()), "", 300,-1.5,1.5,20,0,20);
            h2_[Form("%s_cosphitrk_nstrip",locationtype.c_str())] = new TH2F (Form("%s_cosphitrk_nstrip",locationtype.c_str()), "", 40,-1,1,20,0,20);
            h2_[Form("%s_tanthcosphtrk_nstrip",locationtype.c_str())] = new TH2F (Form("%s_tanthcosphtrk_nstrip",locationtype.c_str()), "", 300,-1.5,1.5,20,0,20);
            h2_[Form("%s_tanthcosphtrk_var2",locationtype.c_str())] = new TH2F (Form("%s_tanthcosphtrk_var2",locationtype.c_str()), "", 300,-1.5,1.5,100,0,1);
            h2_[Form("%s_tanthcosphtrk_var3",locationtype.c_str())] = new TH2F (Form("%s_tanthcosphtrk_var3",locationtype.c_str()), "", 300,-1.5,1.5,100,0,1);
         }
      }
   }
   
   // add the run number to the output file(s)
   outputfile_ = std::regex_replace( outputfile_, std::regex(".root"), Form("_%d.root",run_) );
   
   return 0;
}

