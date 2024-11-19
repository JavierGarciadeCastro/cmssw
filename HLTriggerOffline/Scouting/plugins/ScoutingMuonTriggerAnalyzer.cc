#include "ScoutingMuonTriggerAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
#include <cmath>


ScoutingMuonTriggerAnalyzer::ScoutingMuonTriggerAnalyzer(const edm::ParameterSet& iConfig) : 
  outputInternalPath_(iConfig.getParameter<std::string>("OutputInternalPath")),
  triggerCache_(triggerExpression::Data(iConfig.getParameterSet("triggerConfiguration"), consumesCollector())),
  vtriggerSelection_(iConfig.getParameter<vector<string>>("triggerSelection"))
  {
      //outputInternalPath_ = iConfig.getParameter<std::string>("OutputInternalPath");
      muonCollection_ = consumes<std::vector<pat::Muon>>(iConfig.getParameter<edm::InputTag>("MuonCollection"));
      scoutingMuonCollection_ = consumes<std::vector<Run3ScoutingMuon>>(iConfig.getParameter<edm::InputTag>("ScoutingMuonCollection"));
      vtriggerSelector_.reserve(vtriggerSelection_.size());
      for (auto const& vt:vtriggerSelection_) vtriggerSelector_.push_back(triggerExpression::parse(vt));
      algToken_ = consumes<BXVector<GlobalAlgBlk>>(iConfig.getParameter<edm::InputTag>("AlgInputTag"));
      l1GtUtils_ = std::make_shared<l1t::L1TGlobalUtil>(iConfig, consumesCollector(), l1t::UseEventSetupIn::RunAndEvent);
      l1Seeds_ = iConfig.getParameter<std::vector<std::string> >("l1Seeds");
      for (unsigned int i = 0; i < l1Seeds_.size(); i++){
        const auto& l1seed(l1Seeds_.at(i));
        l1Names[i] = TString(l1seed);
      }
      
}

ScoutingMuonTriggerAnalyzer::~ScoutingMuonTriggerAnalyzer(){
}

void ScoutingMuonTriggerAnalyzer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) {
  //edm::Handle<std::vector<pat::Muon>> patMuons;
  //iEvent.getByToken(muonCollection_, patMuons);
  edm::Handle<std::vector<Run3ScoutingMuon> > sctMuons;
  iEvent.getByToken(scoutingMuonCollection_, sctMuons);
    if (sctMuons.failedToGet()) {
    edm::LogWarning("ScoutingMonitoring")
        << "Run3ScoutingMuon collection not found.";
    return;
  }

  bool passHLTDenominator = false;
  if (triggerCache_.setEvent(iEvent, iSetup)){
    for (unsigned int i = 0; i < vtriggerSelector_.size(); i++){
      auto& vts(vtriggerSelector_.at(i));
      bool result = false;
      if (vts){
        //std::cout << "inside" << std::endl;
        if (triggerCache_.configurationUpdated()) vts->init(triggerCache_);
        result = (*vts)(triggerCache_);
      }
      //std::cout << result << std::endl;
      if (result)
        passHLTDenominator = true;
    }
   }
  //passHLTDenominator = true;
  if (sctMuons->size() > 0) {
    //pat::Muon leading_mu;
    //pat::Muon subleading_mu;
    Run3ScoutingMuon leading_mu;
    Run3ScoutingMuon subleading_mu;
    leading_mu = (*sctMuons)[0]; 
    subleading_mu = (*sctMuons)[0]; 
  
    for (const auto& muon : *sctMuons) {
      if (muon.pt() > leading_mu.pt()) {
        subleading_mu = leading_mu;  
        leading_mu = muon; 
      } 
      else if (muon.pt() > subleading_mu.pt()) {
        subleading_mu = muon;
      }
    }
    
    l1GtUtils_->retrieveL1(iEvent, iSetup, algToken_);
    if (passHLTDenominator){
      h_pt_l1_denominator->Fill(subleading_mu.pt());
      for (unsigned int i = 0; i < l1Seeds_.size(); i++){
        const auto& l1seed(l1Seeds_.at(i));
        bool l1htbit = 0;
        double prescale = -1;
        l1GtUtils_->getFinalDecisionByName(l1seed, l1htbit);
        l1GtUtils_->getPrescaleByName(l1seed, prescale);
        l1Result[i] = l1htbit;
        if (l1Result[i] == 1) {
          h_pt_l1_numerators[i]->Fill(subleading_mu.pt());
        }
      }
    }
  
    //}
  
    //std::cout << "Leading mu pt = " << leading_mu.pt() << std::endl;
    //std::cout << "Subeading mu pt = " << subleading_mu.pt() << std::endl;
  }

}


/*

void ScoutingMuonTriggerAnalyzer::fillHistograms_resonance(const kProbeKinematicMuonHistos histos, const Run3ScoutingMuon mu, const Run3ScoutingVertex vertex, const float inv_mass, const float lxy) const{
  histos.hPt->Fill(mu.pt());
  histos.hEta->Fill(mu.eta());
  histos.hPhi->Fill(mu.phi());
  histos.hInvMass->Fill(inv_mass);
  histos.hNormChisq->Fill(mu.normalizedChi2());
  histos.hTrk_dxy->Fill(mu.trk_dxy());
  histos.hTrk_dz->Fill(mu.trk_dz());
    histos.hLxy->Fill(lxy);
  histos.hXError->Fill(vertex.xError());
  histos.hYError->Fill(vertex.yError());
  histos.hChi2->Fill(vertex.chi2());
  histos.hZ->Fill(vertex.z());
}
 */

void ScoutingMuonTriggerAnalyzer::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& iSetup) {
    ibook.setCurrentFolder(outputInternalPath_);
    h_pt_l1_denominator = ibook.book1D("h_pt_denominator", ";Muon pt (GeV); Muons", 100, 0, 50.0);
    for (unsigned int i = 0; i < l1Seeds_.size(); i++) {
      const auto& l1seed = l1Seeds_.at(i);
      h_pt_l1_numerators.push_back(ibook.book1D(Form("h_pt_numerator_%s", l1seed.c_str()), ";Muon pt (GeV); Muons", 100, 0, 50.0));
    }
    //bookHistograms_resonance(ibook, run, iSetup, histos.resonanceJ_numerator, "resonanceJ_numerator");
    //bookHistograms_resonance(ibook, run, iSetup, histos.resonanceJ_denominator, "resonanceJ_denominator");
    //bookHistograms_resonance(ibook, run, iSetup, histos.resonanceJ_vertex, "resonanceJ_vertex");
    //bookHistograms_resonance(ibook, run, iSetup, histos.resonanceAll, "resonanceAll");
}

//}


// ------------ method fills 'descriptions' with the allowed parameters for the
// module  ------------
void ScoutingMuonTriggerAnalyzer::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions) {
  // The following says we do not know what parameters are allowed so do no
  // validation Please change this to state exactly what you do use, even if it
  // is no parameters
  edm::ParameterSetDescription desc;
  desc.add<std::string>("OutputInternalPath", "MY_FOLDER");
  desc.add<edm::InputTag>("MuonCollection", edm::InputTag("slimmedMuons"));
  desc.add<edm::InputTag>("AlgInputTag", edm::InputTag("gtStage2Digis"));
  desc.add<edm::InputTag>("l1tAlgBlkInputTag", edm::InputTag("gtStage2Digis"));
  desc.add<edm::InputTag>("l1tExtBlkInputTag", edm::InputTag("gtStage2Digis"));
  desc.setUnknown();
  descriptions.addDefault(desc);
  //desc.add<edm::InputTag>("l1Seeds", edm::InputTag("gtStage2Digis"));


  //desc.add<edm::InputTag>("ScoutingMuonCollection", edm::InputTag("Run3ScoutingMuons"));
  //desc.add<edm::InputTag>("ScoutingVtxCollection", edm::InputTag("hltScoutingMuonPackerNoVtx"));
  descriptions.add("ScoutingMuonTriggerAnalyzer", desc);
}

DEFINE_FWK_MODULE(ScoutingMuonTriggerAnalyzer);