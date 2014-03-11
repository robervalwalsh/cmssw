import FWCore.ParameterSet.Config as cms

generator = cms.EDProducer("FlatRandomPtGunProducer",
    PGunParameters = cms.PSet(
        MaxPt = cms.double(10.01),
        MinPt = cms.double(9.99),
        PartID = cms.vint32(13),
        MaxEta = cms.double(4.15340),
        MinEta = cms.double(4.14477),
        MaxPhi = cms.double(0.0043207),
        MinPhi = cms.double(-0.0043207) ## in radians

    ),
    Verbosity = cms.untracked.int32(0), ## set to 1 (or greater)  for printouts

    psethack = cms.string('single electron pt 10'),
    AddAntiParticle = cms.bool(True),
    firstRun = cms.untracked.uint32(1)
)
