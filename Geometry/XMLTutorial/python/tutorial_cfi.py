import FWCore.ParameterSet.Config as cms

XMLIdealGeometryESSource = cms.ESSource("XMLIdealGeometryESSource",


	geomXMLFiles = cms.vstring('Geometry/CMSCommonData/data/materials.xml',
                               'Geometry/CMSCommonData/data/rotations.xml',
                               'Geometry/CMSCommonData/data/extend/cmsextent.xml',
                               'Geometry/CMSCommonData/data/cms.xml',
                               'Geometry/CMSCommonData/data/cmsMother.xml',
			       	 'Geometry/TrackerCommonData/data/tracker.xml',
                               'Geometry/CMSCommonData/data/cmsTracker.xml',
							   #'Geometry/XMLTutorial/data/main.xml',
                               # 'Geometry/XMLTutorial/data/test.xml',
                               'Geometry/ForwardCommonData/data/plt.xml',
                               'Geometry/ForwardCommonData/data/pltbcm.xml'
                               ),
    rootNodeName = cms.string('cms:OCMS')
)
