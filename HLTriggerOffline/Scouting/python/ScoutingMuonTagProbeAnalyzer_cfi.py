import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

ScoutingMuonTagProbeAnalysis = DQMEDAnalyzer('ScoutingMuonTagProbeAnalyzer',

    OutputInternalPath = cms.string('/HLT/ScoutingOffline/Muons'),
    MuonCollection = cms.InputTag('slimmedMuons'),
    ScoutingMuonCollection = cms.InputTag('hltScoutingMuonPackerNoVtx'),
    ScoutingVtxCollection = cms.InputTag('hltScoutingMuonPackerNoVtx','displacedVtx','HLT')
)

scoutingMonitoringTagProbeMuon = cms.Sequence(ScoutingMuonTagProbeAnalysis)
