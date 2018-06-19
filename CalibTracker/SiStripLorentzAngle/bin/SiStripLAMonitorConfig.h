#ifndef CalibTracker_SiStripLorentzAngle_LAMonitorConfig_h_
#define CalibTracker_SiStripLorentzAngle_LAMonitorConfig_h_ 1

#include "boost/program_options.hpp"
#include "boost/algorithm/string.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

int SiStripLAMonitorConfig(int argc, char * argv[]);

std::string cfg_;

int nentriesmax_;
int run_;
bool saveHistos_;
bool saveHistos2D_;
bool saveHistosProfile_;
bool saveHistosMods_;
std::string inputlist_;
std::string inputFile_;
std::string calibTreeDir_;
std::string infoTreePath_;
std::string outputfile_;
std::string calibrationMode_;
std::string eventPrefix_;
std::string eventSuffix_;
std::string trackPrefix_;
std::string trackSuffix_;
std::string calibPrefix_;
std::string calibSuffix_;

float etamin_;
float etamax_;
float ptmin_;
float ptmax_;
int   hitsvalmin_;
float chi2ndfmax_;

int SiStripLAMonitorConfig(int argc, char * argv[])
{
   try
   {
      namespace po = boost::program_options;
      po::options_description desc("Options");
      desc.add_options()
         ("help,h","Show help messages")
         ("config,c",po::value<std::string>(&cfg_),"Configuration file name")
         ("run,r",po::value<int>(&run_),"Run number");
      
      po::options_description config("Configuration");
      config.add_options()
         ("InputFilesList",po::value <std::string> (&inputlist_)->default_value("CalibTreeList.txt"),"File with list of ntuples")
         ("InputFile",po::value <std::string> (&inputFile_)->default_value(""),"Given input file")
         ("CalibTreeDirectory",po::value <std::string> (&calibTreeDir_)->default_value("/eos/cms/store/group/dpg_tracker_strip/comm_tracker/Strip/Calibration/calibrationtree/GR17_Aag/"),"Direcotry of the calibTree files")
         ("OutputFile",po::value <std::string> (&outputfile_)->default_value("SiStripLAMonitor.root"),"Name of the root output file")
         ("Run",po::value <int> (&run_), "Run number")
         ("EntriesMax",po::value <int> (&nentriesmax_)->default_value(-1), "Maximum number of events")
         ("SaveHistograms",po::value <bool> (&saveHistos_)->default_value(true), "Save histograms")
         ("SaveHistogramsProfile",po::value <bool> (&saveHistosProfile_)->default_value(true), "Save histograms profile")
         ("SaveHistograms2D",po::value <bool> (&saveHistos2D_)->default_value(false), "Save histograms 2D")
         ("SaveHistogramsModules",po::value <bool> (&saveHistosMods_)->default_value(false), "Save histograms of single modules")
         ("CalibrationMode",po::value <std::string> (&calibrationMode_)->default_value("StdBunch"),"Calibration mode")
         ("InfoTreePath",po::value <std::string> (&infoTreePath_)->default_value("lorentzAngleRunInfo"),"Direcotry of the calibTree files")
         ("EventPrefix",po::value <std::string> (&eventPrefix_)->default_value(""),"Event prefix")
         ("EventSuffix",po::value <std::string> (&eventSuffix_)->default_value(""),"Event suffix")
         ("TrackPrefix",po::value <std::string> (&trackPrefix_)->default_value(""),"Track prefix")
         ("TrackSuffix",po::value <std::string> (&trackSuffix_)->default_value(""),"Track suffix")
         ("CalibPrefix",po::value <std::string> (&calibPrefix_)->default_value("GainCalibration"),"Calibration prefix")
         ("CalibSuffix",po::value <std::string> (&calibSuffix_)->default_value(""),"Calibration suffix")
         ("ptMin",po::value <float> (&ptmin_)->default_value(-1), "Min track pT")
         ("ptMax",po::value <float> (&ptmax_)->default_value(-1), "Max track pT")
         ("etaMin",po::value <float> (&etamin_)->default_value(-5), "Min track eta")
         ("etaMax",po::value <float> (&etamax_)->default_value(5) , "Max track eta")
         ("nHitsValidMin",po::value <int> (&hitsvalmin_)->default_value(-1) , "Min number of valid hits")
         ("chi2ndofMax",po::value <float> (&chi2ndfmax_)->default_value(-1) , "Max chi2/ndof");
      
      po::variables_map vm; 
      try
      {
         po::store(po::parse_command_line(argc, argv, desc), vm); // can throw
         // --help option
         
         if ( vm.count("help") )
         {
            std::cout << "SiStripLAMonitor" << std::endl
                      << desc << std::endl
                      << config << std::endl;
            return 1;
         }
         po::notify(vm);
         
         std::ifstream cfg_s(cfg_.c_str());
         po::store(po::parse_config_file(cfg_s, config), vm); // can throw
         if ( vm.count("config") )
         {

         }
         po::notify(vm);
      }
      catch(po::error& e)
      { 
         std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
         std::cerr << desc << std::endl; 
         return -1; 
      } 
      
   }
   catch(std::exception& e) 
   { 
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
      return -1; 
   } 
   
   return 0;}
#endif
