# CalibTrees for Lorentz Angle Analysis

## prepare workarea

```bash
export SCRAM_ARCH=slc7_amd64_gcc700
cmsrel CMSSW_10_6_13
cd CMSSW_10_6_13/src
cmsenv

git cms-init

git checkout from-CMSSW_10_6_13-UL-calibTrees_forLA

git cms-addpkg CalibTracker/Configuration
git cms-addpkg CalibTracker/SiStripChannelGain
git cms-addpkg CalibTracker/SiStripCommon
git cms-addpkg CalibTracker/SiStripLorentzAngle

scram b -j4
```
