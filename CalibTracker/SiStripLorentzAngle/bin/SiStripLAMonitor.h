#ifndef CalibTracker_SiStripLorentzAngle_SiStripLAMonitor_h
#define CalibTracker_SiStripLorentzAngle_SiStripLAMonitor_h 1

#include <string>
#include <vector>

#include "TChain.h"
#include "TH1.h" 
#include "TH2.h"
#include "TProfile.h"

int Init(int argc, char * argv[]);
void AnalyzeTheTree();
void ProcessTheEvent();
void ProcessTheModule(const unsigned int &);
void WriteOutputs(const bool & savehistos = true);
// void MeasureLorentzAngle(std::string method);

// event data
unsigned int eventnumber_ = 0;
unsigned int runnumber_   = 0;
unsigned int luminumber_  = 0;

// calib data
std::vector<unsigned int>   * trackindex_ = nullptr;
std::vector<unsigned int>   * rawid_      = nullptr;
std::vector<unsigned short> * nstrips_    = nullptr;
std::vector<float>          * localdirx_  = nullptr;
std::vector<float>          * localdiry_  = nullptr;
std::vector<float>          * localdirz_  = nullptr;

// track data
std::vector<float>        * trackpt_        = 0;
std::vector<float>        * tracketa_       = 0;
std::vector<unsigned int> * trackhitsvalid_ = 0;
std::vector<float>        * trackchi2ndof_  = 0;

// histogramming
std::map<std::string, TH1F*> h1_;
std::map<std::string, TH2F*> h2_;
std::map<std::string, TProfile*> hp_;

// info
std::map<std::string,int> nlayers_;
std::vector<std::string> modtypes_;


#endif
