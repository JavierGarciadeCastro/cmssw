#ifndef DQMOffline_Scouting_ScoutingMuonTriggerAnalyzer_h
#define DQMOffline_Scouting_ScoutingMuonTriggerAnalyzer_h

#include <string>
#include <vector>

// user include files
#include "DQMServices/Core/interface/DQMGlobalEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Scouting/interface/Run3ScoutingMuon.h"
#include "DataFormats/Scouting/interface/Run3ScoutingVertex.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/L1TGlobal/interface/L1TGlobalUtil.h"
#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionData.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionEvaluator.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionParser.h"

/////////////////////////
//  Class declaration  //
/////////////////////////
//
//struct kProbeKinematicMuonHistos{
//  dqm::reco::MonitorElement* hPt;
//  dqm::reco::MonitorElement* hEta;
//  dqm::reco::MonitorElement* hPhi;
//  dqm::reco::MonitorElement* hInvMass;
//  dqm::reco::MonitorElement* hNormChisq;
//  dqm::reco::MonitorElement* hTrk_dxy;
//  dqm::reco::MonitorElement* hTrk_dz;
//  dqm::reco::MonitorElement* hLxy;
//  dqm::reco::MonitorElement* hXError;
//  dqm::reco::MonitorElement* hYError;
//  dqm::reco::MonitorElement* hChi2;
//  dqm::reco::MonitorElement* hZ;
//};
//
//
//struct kTagProbeMuonHistos {
//  kProbeKinematicMuonHistos resonanceJ_numerator;
//  kProbeKinematicMuonHistos resonanceJ_denominator;
//  //kProbeKinematicMuonHistos resonanceJ_vertex;
//  //kProbeKinematicMuonHistos resonanceAll;
//};
//
class ScoutingMuonTriggerAnalyzer: public DQMEDAnalyzer {
      public: 
        explicit ScoutingMuonTriggerAnalyzer(const edm::ParameterSet& conf);
        ~ScoutingMuonTriggerAnalyzer() override;
        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

      private:
        void analyze(const edm::Event & e, const edm::EventSetup & c) override;
        void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;

        // --------------------- member data  ----------------------
        std::string outputInternalPath_;
        edm::EDGetTokenT<std::vector<pat::Muon>> muonCollection_;
        edm::EDGetTokenT<std::vector<Run3ScoutingMuon>> scoutingMuonCollection_;
        triggerExpression::Data triggerCache_;
        std::vector<triggerExpression::Evaluator*> vtriggerSelector_;
        std::vector<std::string> vtriggerSelection_;
        edm::EDGetToken algToken_;
        std::shared_ptr<l1t::L1TGlobalUtil> l1GtUtils_;
        std::vector<std::string> l1Seeds_;
        TString l1Names[100] = {""};
        Bool_t l1Result[100] = {false};
        
        dqm::reco::MonitorElement* h_pt1_l1_denominator;
        dqm::reco::MonitorElement* h_eta1_l1_denominator;
        dqm::reco::MonitorElement* h_phi1_l1_denominator;
        dqm::reco::MonitorElement* h_dxy1_l1_denominator;

        dqm::reco::MonitorElement* h_pt2_l1_denominator;
        dqm::reco::MonitorElement* h_eta2_l1_denominator;
        dqm::reco::MonitorElement* h_phi2_l1_denominator;
        dqm::reco::MonitorElement* h_dxy2_l1_denominator;
  
        std::vector<dqm::reco::MonitorElement*> h_pt1_l1_numerators;
        std::vector<dqm::reco::MonitorElement*> h_eta1_l1_numerators;
        std::vector<dqm::reco::MonitorElement*> h_phi1_l1_numerators;
        std::vector<dqm::reco::MonitorElement*> h_dxy1_l1_numerators;

        std::vector<dqm::reco::MonitorElement*> h_pt2_l1_numerators;
        std::vector<dqm::reco::MonitorElement*> h_eta2_l1_numerators;
        std::vector<dqm::reco::MonitorElement*> h_phi2_l1_numerators;
        std::vector<dqm::reco::MonitorElement*> h_dxy2_l1_numerators;



    };

#endif