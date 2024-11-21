import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester
from HLTriggerOffline.Scouting.ScoutingMuonTriggerAnalyzer_cfi import *


muonEfficiencyNoVtx = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("/HLT/ScoutingOffline/Muons/NoVtx"),
    #outputFileName = cms.untracked.string("output.root"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages                                                                                                                                          
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_muonPt       'efficiency vs pt; Muon pt [GeV]; efficiency' resonanceJ_numerator_Probe_sctMuon_Pt       resonanceJ_denominator_Probe_sctMuon_Pt",
        "effic_muonEta       'efficiency vs eta; Muon eta; efficiency' resonanceJ_numerator_Probe_sctMuon_Eta       resonanceJ_denominator_Probe_sctMuon_Eta",
        "effic_muonPhi       'efficiency vs phi; Muon phi; efficiency' resonanceJ_numerator_Probe_sctMuon_Phi       resonanceJ_denominator_Probe_sctMuon_Phi",
        "effic_muondxy       'efficiency vs dxy; Muon dxy; efficiency' resonanceJ_numerator_Probe_sctMuon_dxy       resonanceJ_denominator_Probe_sctMuon_dxy",
        "effic_muonInvMass       'efficiency vs inv mass; Muon inv mass [GeV]; efficiency' resonanceJ_numerator_sctMuon_Invariant_Mass       resonanceJ_denominator_sctMuon_Invariant_Mass",
        "effic_muonNormChisq      'efficiency vs normChi2; Muon normChi2; efficiency' resonanceJ_numerator_Probe_sctMuon_NormChisq       resonanceJ_denominator_Probe_sctMuon_NormChisq",
        "effic_muonTrkdz       'efficiency vs Trkdz; Muon trkdz; efficiency' resonanceJ_numerator_Probe_sctMuon_Trk_dz       resonanceJ_denominator_Probe_sctMuon_Trkdz",
        "effic_muonTrkdxy       'efficiency vs Trkdxy; Muon trkdxy; efficiency' resonanceJ_numerator_Probe_sctMuon_Trk_dxy       resonanceJ_denominator_Probe_sctMuon_Trkdxy",
        "effic_muonlxy       'efficiency vs lxy; Muon lxy; efficiency' resonanceJ_numerator_Vertex_Lxy       resonanceJ_denominator_Vertex_Lxy",
        "effic_muonVertexYerror       'efficiency vs VertexYerror; Muon Vertex Yerror; efficiency' resonanceJ_numerator_Vertex_Yerror       resonanceJ_denominator_Vertex_Yerror",
        "effic_muonVertexXerror       'efficiency vs VertexXerror; Muon Vertex Xerror; efficiency' resonanceJ_numerator_Vertex_Xerror       resonanceJ_denominator_Vertex_Xerror",
        "effic_muonVertexChi2       'efficiency vs Vertexchi2; Muon Vertex chi2; efficiency' resonanceJ_numerator_Vertex_chi2       resonanceJ_denominator_Vertex_chi2",
        "effic_muonVertexYerror       'efficiency vs z; Muon Vertex z; efficiency' resonanceJ_numerator_Vertex_z       resonanceJ_denominator_Vertex_z",

    ),
    #
)
#
muonEfficiencyVtx = muonEfficiencyNoVtx.clone()
muonEfficiencyVtx.subDirs = cms.untracked.vstring("/HLT/ScoutingOffline/Muons/Vtx")
#
allSeeds = SingleMuL1 + DoubleMuL1
efficiencyList = ["effic_pt1_%s       '%s; Leading muon pt [GeV]; L1 efficiency' h_pt1_numerator_%s h_pt1_denominator"%(seed,seed, seed) for seed in allSeeds]+\
["effic_eta1_%s       '%s; Leading muon eta; L1 efficiency' h_eta1_numerator_%s h_eta1_denominator"%(seed,seed, seed) for seed in allSeeds]+\
["effic_phi1_%s       '%s; Leading muon phi; L1 efficiency' h_phi1_numerator_%s h_phi1_denominator"%(seed,seed, seed) for seed in allSeeds]+\
["effic_dxy1_%s       '%s; Leading muon dxy; L1 efficiency' h_dxy1_numerator_%s h_dxy1_denominator"%(seed,seed, seed) for seed in allSeeds]+\
["effic_pt2_%s       '%s; Subleading muon pt [GeV]; L1 efficiency' h_pt2_numerator_%s h_pt2_denominator"%(seed,seed, seed) for seed in allSeeds]+\
["effic_eta2_%s       '%s; Subleading muon eta; L1 efficiency' h_eta2_numerator_%s h_eta2_denominator"%(seed,seed, seed) for seed in allSeeds]+\
["effic_phi2_%s       '%s; Subleading muon phi; L1 efficiency' h_phi2_numerator_%s h_phi2_denominator"%(seed,seed, seed) for seed in allSeeds]+\
["effic_dxy2_%s       '%s; Subleading muon dxy; L1 efficiency' h_dxy2_numerator_%s h_dxy2_denominator"%(seed,seed, seed) for seed in allSeeds]
muonTriggerEfficiency = DQMEDHarvester("DQMGenericClient",
    subDirs        = cms.untracked.vstring("/HLT/ScoutingOffline/Muons/L1Efficiency"),
    #outputFileName = cms.untracked.string("output.root"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages                                                                                                                                          
    resolution     = cms.vstring(),
    efficiency     = cms.vstring( efficiencyList ),
)
    


 