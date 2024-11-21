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
        if (triggerCache_.configurationUpdated()) vts->init(triggerCache_);
        result = (*vts)(triggerCache_);
      }
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
  
    std::vector<Run3ScoutingMuon> sorted_mu;
    for (const auto& muon : *sctMuons) {
      sorted_mu.push_back(muon);
    }
    std::sort(std::begin(sorted_mu),std::end(sorted_mu), [&](Run3ScoutingMuon mu1, Run3ScoutingMuon mu2){return mu1.pt() > mu2.pt();});
    leading_mu = sorted_mu.at(0);
    if (sorted_mu.size() > 1)
      subleading_mu = sorted_mu.at(1);
    
    l1GtUtils_->retrieveL1(iEvent, iSetup, algToken_);

    if (passHLTDenominator){
      h_pt1_l1_denominator->Fill(leading_mu.pt());
      h_eta1_l1_denominator->Fill(leading_mu.eta());
      h_phi1_l1_denominator->Fill(leading_mu.phi());
      h_dxy1_l1_denominator->Fill(leading_mu.trk_dxy());
      if (sorted_mu.size() > 1){
        h_pt2_l1_denominator->Fill(subleading_mu.pt());
        h_eta2_l1_denominator->Fill(subleading_mu.eta());
        h_phi2_l1_denominator->Fill(subleading_mu.phi());
        h_dxy2_l1_denominator->Fill(subleading_mu.trk_dxy());
      }

      for (unsigned int i = 0; i < l1Seeds_.size(); i++){
        const auto& l1seed(l1Seeds_.at(i));
        bool l1htbit = 0;
        double prescale = -1;
        l1GtUtils_->getFinalDecisionByName(l1seed, l1htbit);
        l1GtUtils_->getPrescaleByName(l1seed, prescale);
        l1Result[i] = l1htbit;
        if (l1Result[i] == 1) {
          h_pt1_l1_numerators[i]->Fill(leading_mu.pt());
          h_eta1_l1_numerators[i]->Fill(leading_mu.eta());
          h_phi1_l1_numerators[i]->Fill(leading_mu.phi());
          h_dxy1_l1_numerators[i]->Fill(leading_mu.trk_dxy());
          if (sorted_mu.size() > 1){
            h_pt2_l1_numerators[i]->Fill(subleading_mu.pt());
            h_eta2_l1_numerators[i]->Fill(subleading_mu.eta());
            h_phi2_l1_numerators[i]->Fill(subleading_mu.phi());
            h_dxy2_l1_numerators[i]->Fill(subleading_mu.trk_dxy());
          }
        }
      }
    }  
  }
}
void ScoutingMuonTriggerAnalyzer::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& iSetup) {
    ibook.setCurrentFolder(outputInternalPath_);
    h_pt1_l1_denominator = ibook.book1D("h_pt1_denominator", ";Leading muon pt (GeV); Muons", 100, 0, 50.0);
    h_eta1_l1_denominator = ibook.book1D("h_eta1_denominator", ";Leading muon eta; Muons", 100, -5.0, 5.0);
    h_phi1_l1_denominator = ibook.book1D("h_phi1_denominator", ";Leading muon phi; Muons", 100, -3.3, 3.3);
    h_dxy1_l1_denominator = ibook.book1D("h_dxy1_denominator", ";Leading muon dxy; Muons", 100, 0, 5.0);

    h_pt2_l1_denominator = ibook.book1D("h_pt2_denominator", ";Subleading muon pt (GeV); Muons", 100, 0, 50.0);
    h_eta2_l1_denominator = ibook.book1D("h_eta2_denominator", ";Subleading muon eta; Muons", 100, -5.0, 5.0);
    h_phi2_l1_denominator = ibook.book1D("h_phi2_denominator", ";Subleading muon phi; Muons", 100, -3.3, 3.3);
    h_dxy2_l1_denominator = ibook.book1D("h_dxy2_denominator", ";Subleaing muon dxy; Muons", 100, 0, 5.0);

    for (unsigned int i = 0; i < l1Seeds_.size(); i++) {
      const auto& l1seed = l1Seeds_.at(i);
      h_pt1_l1_numerators.push_back(ibook.book1D(Form("h_pt1_numerator_%s", l1seed.c_str()), ";Leading muon pt (GeV); Muons", 100, 0, 50.0));
      h_eta1_l1_numerators.push_back(ibook.book1D(Form("h_eta1_numerator_%s", l1seed.c_str()),";Leading muon eta; Muons", 100, -5.0, 5.0));
      h_phi1_l1_numerators.push_back(ibook.book1D(Form("h_phi1_numerator_%s", l1seed.c_str()), ";Leading muon phi; Muons", 100, 3.3, -3.3));
      h_dxy1_l1_numerators.push_back(ibook.book1D(Form("h_dxy1_numerator_%s", l1seed.c_str()), ";Leading muon dxy; Muons", 100, 0, 5.0));

      h_pt2_l1_numerators.push_back(ibook.book1D(Form("h_pt2_numerator_%s", l1seed.c_str()), ";Subleading muon pt (GeV); Muons", 100, 0, 50.0));
      h_eta2_l1_numerators.push_back(ibook.book1D(Form("h_eta2_numerator_%s", l1seed.c_str()),";Subleading muon eta; Muons", 100, -5.0, 5.0));
      h_phi2_l1_numerators.push_back(ibook.book1D(Form("h_phi2_numerator_%s", l1seed.c_str()), ";Subleading muon phi; Muons", 100, 3.3, -3.3));
      h_dxy2_l1_numerators.push_back(ibook.book1D(Form("h_dxy2_numerator_%s", l1seed.c_str()), ";Subleading muon dxy; Muons", 100, 0, 5.0));

    }
}

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
  descriptions.add("ScoutingMuonTriggerAnalyzer", desc);
}

DEFINE_FWK_MODULE(ScoutingMuonTriggerAnalyzer);