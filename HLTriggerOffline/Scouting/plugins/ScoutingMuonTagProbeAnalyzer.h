#ifndef DQMOffline_Scouting_ScoutingMuonTagProbeAnalyzer_h
#define DQMOffline_Scouting_ScoutingMuonTagProbeAnalyzer_h

#include <string>
#include <vector>
#include <iostream>

// user include files
#include "DQMServices/Core/interface/DQMGlobalEDAnalyzer.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Scouting/interface/Run3ScoutingMuon.h"
#include "DataFormats/Scouting/interface/Run3ScoutingVertex.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/////////////////////////
//  Class declaration  //
/////////////////////////

struct kProbeKinematicMuonHistos{
  dqm::reco::MonitorElement* hPt;
  dqm::reco::MonitorElement* hEta;
  dqm::reco::MonitorElement* hPhi;
  dqm::reco::MonitorElement* hInvMass;
  dqm::reco::MonitorElement* hNormChisq;
  dqm::reco::MonitorElement* hTrk_dxy;
  dqm::reco::MonitorElement* hTrk_dz;
  dqm::reco::MonitorElement* hnPixel;
  dqm::reco::MonitorElement* hnTracker;
  dqm::reco::MonitorElement* htrk_qoverp;


  dqm::reco::MonitorElement* hLxy;
  dqm::reco::MonitorElement* hXError;
  dqm::reco::MonitorElement* hYError;
  dqm::reco::MonitorElement* hChi2;
  dqm::reco::MonitorElement* hZ;
};


struct kTagProbeMuonHistos {
  kProbeKinematicMuonHistos resonanceJ_numerator;
  kProbeKinematicMuonHistos resonanceJ_denominator;
};

class ScoutingMuonTagProbeAnalyzer: public DQMGlobalEDAnalyzer<kTagProbeMuonHistos> {
      public: 
        explicit ScoutingMuonTagProbeAnalyzer(const edm::ParameterSet& conf);
        ~ScoutingMuonTagProbeAnalyzer() override;
        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

      private:
        void dqmAnalyze(const edm::Event & e, const edm::EventSetup & c, kTagProbeMuonHistos const&) const override;
        void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &, kTagProbeMuonHistos &) const override;
        void bookHistograms_resonance(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &, kProbeKinematicMuonHistos &, const std::string &) const;
        void fillHistograms_resonance(const kProbeKinematicMuonHistos histos, const Run3ScoutingMuon mu, const Run3ScoutingVertex vertex, const float inv_mass, const float lxy) const;
        bool scoutingMuonID(const Run3ScoutingMuon mu) const;

        // --------------------- member data  ----------------------
        std::string outputInternalPath_;
        edm::EDGetTokenT<std::vector<pat::Muon>> muonCollection_;
        edm::EDGetTokenT<std::vector<Run3ScoutingMuon>> scoutingMuonCollection_;
        edm::EDGetTokenT<std::vector<Run3ScoutingVertex>> scoutingVtxCollection_;
        Bool_t runWithoutVtx_;

    };

#endif
