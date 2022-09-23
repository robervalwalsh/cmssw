import FWCore.ParameterSet.Config as cms

process = cms.Process("Test")
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)
process.source = cms.Source("EmptySource",
    firstRun = cms.untracked.uint32(1),
    numberEventsInRun = cms.untracked.uint32(1),
)

process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO')
    ),
    destinations = cms.untracked.vstring('cout')
)

process.Timing = cms.Service("Timing")
process.PoolDBESSource = cms.ESSource("PoolDBESSource",
    BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService'),
    DBParameters = cms.PSet(
        messageLevel = cms.untracked.int32(2),
        authenticationPath = cms.untracked.string('/afs/cern.ch/cms/DB/conddb')
    ),
    timetype = cms.string('runnumber'),
    toGet = cms.VPSet(cms.PSet(
        record = cms.string('SiStripBadStripRcd'),
        tag = cms.string('SiStripBadComponents_merged_v0')
    )),
    connect = cms.string('sqlite_file:SiStripBadComponents_merged_v0.db')
)


##
## Database output service
##
process.load("CondCore.CondDB.CondDB_cfi")

##
## Output database (in this case local sqlite file)
##
process.CondDB.connect = 'sqlite_file:SiStripBadComponents_merged_noFED124.db'
process.PoolDBOutputService = cms.Service("PoolDBOutputService",
                                          process.CondDB,
                                          timetype = cms.untracked.string('runnumber'),
                                          toPut = cms.VPSet(cms.PSet(record = cms.string('SiStripBadStripRcd'),
                                                                     tag = cms.string('SiStripBadComponents_merged_noFED124')
                                                                     )
                                                            )
                                          )


process.prod = cms.EDAnalyzer("SiStripBadChannelPatcher",
                              Record = cms.string("SiStripBadStripRcd"))

process.print = cms.OutputModule("AsciiOutputModule")

process.p = cms.Path(process.prod)
#process.ep = cms.EndPath(process.print)
