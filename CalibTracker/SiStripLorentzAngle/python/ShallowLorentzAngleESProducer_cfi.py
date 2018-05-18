import FWCore.ParameterSet.Config as cms

shallowLorentzAngleESProducer = cms.EDProducer("ShallowLorentzAngleESProducer",
                                      Prefix=cms.string("LorentzAngleESP"),
                                      Suffix=cms.string(""))
