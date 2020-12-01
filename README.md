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

## run test

```bash
cd SiStripCommon/test/MakeCalibrationTrees
cmsRun produceCalibrationTree_template_cfg.py \
inputCollection=ALCARECOSiStripCalCosmics \
inputFiles=/store/data/Run2018C/Cosmics/ALCARECO/SiStripCalCosmics-UL18-v1/230000/0C458F23-0922-164C-B1A6-306F77DDA853.root \
runNumber=319591 \
conditionGT=106X_dataRun2_v28 \
cosmicTriggers="HLT_L1SingleMuOpen_v*"
```

