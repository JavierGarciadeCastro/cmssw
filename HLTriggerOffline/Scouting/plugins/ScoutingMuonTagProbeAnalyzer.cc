#include "ScoutingMuonTagProbeAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
#include <cmath>


ScoutingMuonTagProbeAnalyzer::ScoutingMuonTagProbeAnalyzer(const edm::ParameterSet& iConfig)
  : outputInternalPath_(iConfig.getParameter<std::string>("OutputInternalPath")),
    muonCollection_(consumes<std::vector<pat::Muon>>(iConfig.getParameter<edm::InputTag>("MuonCollection"))),
    scoutingMuonCollection_(consumes<std::vector<Run3ScoutingMuon>>(iConfig.getParameter<edm::InputTag>("ScoutingMuonCollection"))),
    scoutingVtxCollection_(consumes<std::vector<Run3ScoutingVertex>>(iConfig.getParameter<edm::InputTag>("ScoutingVtxCollection"))){
}

ScoutingMuonTagProbeAnalyzer::~ScoutingMuonTagProbeAnalyzer(){
}

void ScoutingMuonTagProbeAnalyzer::dqmAnalyze(edm::Event const& iEvent,
                                                  edm::EventSetup const& iSetup,
                                                  kTagProbeMuonHistos const& histos) const {

  edm::Handle<std::vector<pat::Muon>> patMuons;
  iEvent.getByToken(muonCollection_, patMuons);
  if (patMuons.failedToGet()) {
    edm::LogWarning("ScoutingMonitoring")
        << "pat::Muon collection not found.";
    return;
  }

  edm::Handle<std::vector<Run3ScoutingMuon> > sctMuons;
  iEvent.getByToken(scoutingMuonCollection_, sctMuons);
  if (sctMuons.failedToGet()) {
    edm::LogWarning("ScoutingMonitoring")
        << "Run3ScoutingMuon collection not found.";
    return;
  }
  
  edm::Handle<std::vector<Run3ScoutingVertex>> sctVertex;
  iEvent.getByToken(scoutingVtxCollection_, sctVertex);
  if (sctVertex.failedToGet()) {
    edm::LogWarning("ScoutingMonitoring")
        << "Run3ScoutingVertex collection not found.";
    return;
  }
  
  edm::LogInfo("ScoutingMonitoring")
      << "Process pat::Muons: " << patMuons->size();
  edm::LogInfo("ScoutingMonitoring")
      << "Process Run3ScoutingMuons: " << sctMuons->size();
  
  edm::LogInfo("ScoutingMonitoring")
      << "Process Run3ScoutingVertex: " << sctVertex->size();
  
  bool foundTag = false;
  for (const auto& sct_mu : *sctMuons){
    if (!scoutingMuonID(sct_mu)) continue;
    if (foundTag) continue;
    math::PtEtaPhiMLorentzVector tag_sct_mu(sct_mu.pt(), sct_mu.eta(), sct_mu.phi(), sct_mu.m());
    const std::vector<int> vtxIndx_tag = sct_mu.vtxIndx();

    for (const auto& sct_mu_second : *sctMuons){
      if(&sct_mu_second == &sct_mu) continue;
      math::PtEtaPhiMLorentzVector probe_sct_mu(sct_mu_second.pt(), sct_mu_second.eta(), sct_mu_second.phi(), sct_mu_second.m());
      if(sct_mu_second.pt() < 3) continue;
      const std::vector<int> vtxIndx_probe = sct_mu_second.vtxIndx();
      
      float invMass = (tag_sct_mu + probe_sct_mu).mass();
      edm::LogInfo("ScoutingMonitoring")
          << "Inv Mass: " << invMass;
      if((2.8 < invMass) && (invMass < 3.8)){ 
        std::cout << invMass << std::endl;

	      if (vtxIndx_tag.size() == 0) continue;
	      if (vtxIndx_probe.size() == 0) continue;

        for (const auto& commonIdx : vtxIndx_tag) {
          if (std::find(vtxIndx_probe.begin(), vtxIndx_probe.end(), commonIdx) != vtxIndx_probe.end()) {
            const auto& vertex = (*sctVertex)[commonIdx];
            float lxy = sqrt(vertex.x() * vertex.x() +  vertex.y() * vertex.y());
            if (scoutingMuonID(sct_mu_second)){
              fillHistograms_resonance(histos.resonanceJ_numerator,  sct_mu_second, vertex, invMass, lxy);
            }
            fillHistograms_resonance(histos.resonanceJ_denominator,  sct_mu_second, vertex, invMass, lxy);
            //fillHistograms_resonance(histos.resonanceJ_vertex,  vertex, lxy);
            //fillHistograms_resonance(histos.resonanceAll,sct_mu_second, invMass);
          }
        }
      }
      else{
        //fillHistograms_resonance(histos.resonanceAll, sct_mu_second, invMass);
      }

    }
    foundTag = true;
  }

}


bool ScoutingMuonTagProbeAnalyzer::scoutingMuonID(const Run3ScoutingMuon mu) const{
  math::PtEtaPhiMLorentzVector particle(mu.pt(), mu.eta(), mu.phi(), 0.10566);
  double normchisq_threshold = 3.0;
  double pt_threshold = 3.0;
  double eta_threshold = 2.4;
  int layer_threshold = 4;

  if (mu.pt() > pt_threshold && fabs(mu.eta()) < eta_threshold && mu.normalizedChi2() < normchisq_threshold && mu.isGlobalMuon() && mu.nTrackerLayersWithMeasurement() > layer_threshold) {
    return true;
  }
  return false;
}
void ScoutingMuonTagProbeAnalyzer::fillHistograms_resonance(const kProbeKinematicMuonHistos histos, const Run3ScoutingMuon mu, const Run3ScoutingVertex vertex, const float inv_mass, const float lxy) const{
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


//void ScoutingMuonTagProbeAnalyzer::fillHistograms_vertex(const kProbeKinematicVertexHistos histos_vertex, const Run3ScoutingVertex vertex, const float lxy) const{
//  histos_vertex.hLxy->Fill(lxy);
//  histos_vertex.hXError->Fill(vertex.xError());
//  histos_vertex.hYError->Fill(vertex.yError());
//  histos_vertex.hChi2->Fill(vertex.chi2());
//  histos_vertex.hZ->Fill(vertex.z());
//}


void ScoutingMuonTagProbeAnalyzer::bookHistograms(DQMStore::IBooker& ibook,
                                                       edm::Run const& run,
                                                       edm::EventSetup const& iSetup,
                                                       kTagProbeMuonHistos& histos) const{
    ibook.setCurrentFolder(outputInternalPath_);
    bookHistograms_resonance(ibook, run, iSetup, histos.resonanceJ_numerator, "resonanceJ_numerator");
    bookHistograms_resonance(ibook, run, iSetup, histos.resonanceJ_denominator, "resonanceJ_denominator");
    //bookHistograms_resonance(ibook, run, iSetup, histos.resonanceJ_vertex, "resonanceJ_vertex");
    //bookHistograms_resonance(ibook, run, iSetup, histos.resonanceAll, "resonanceAll");
 }

void  ScoutingMuonTagProbeAnalyzer::bookHistograms_resonance(DQMStore::IBooker& ibook,
                                                                 edm::Run const& run,
                                                                 edm::EventSetup const& iSetup,
                                                                 kProbeKinematicMuonHistos& histos,
                                                                 const std::string& name) const{
     ibook.setCurrentFolder(outputInternalPath_);
    
     histos.hPt = ibook.book1D(name + "_Probe_sctMuon_Pt",name + "_Probe_sctMuon_Pt", 100, 0, 50.0);    
     histos.hEta = ibook.book1D(name + "_Probe_sctMuon_Eta",name + "_Probe_sctMuon_Eta", 100, -5.0, 5.0);
     histos.hPhi = ibook.book1D(name + "_Probe_sctMuon_Phi",name + "_Probe_sctMuon_Phi", 100, -3.3, 3.3);
     histos.hInvMass = ibook.book1D(name + "_sctMuon_Invariant_Mass",name + "_sctMuon_Invariant_Mass", 800, 0, 5);
     histos.hNormChisq = ibook.book1D(name + "_Probe_sctMuon_NormChisq",name + "_Probe_sctMuon_NormChisq", 100, 0, 5.0);
     histos.hTrk_dxy = ibook.book1D(name + "_Probe_sctMuon_Trk_dxy",name + "_Probe_sctMuon_Trk_dxy", 100, 0, 5.0);
     histos.hTrk_dz = ibook.book1D(name + "_Probe_sctMuon_Trk_dz",name + "_Probe_sctMuon_Trk_dz", 100, 0, 20.0);
     histos.hLxy = ibook.book1D(name + "_Vertex_Lxy",name + "_Vertex_Lxy", 100, 0, 20);
     histos.hXError = ibook.book1D(name + "_Vertex_Xerror",name + "_Vertex_Xerror", 100, 0, 2);
     histos.hYError = ibook.book1D(name + "_Vertex_Yerror",name + "_Vertex_Yerror", 100, 0, 2);
     histos.hChi2 = ibook.book1D(name + "_Vertex_chi2",name + "_Vertex_chi2", 100, 0, 15);
     histos.hZ = ibook.book1D(name + "_Vertex_z",name + "_Vertex_z", 100, 0, 15);
     //error en x, error en y, lxy, normalized chi2, z (vertice)
}

//void  ScoutingMuonTagProbeAnalyzer::bookHistograms_vertex(DQMStore::IBooker& ibook,
//                                                                 edm::Run const& run,
//                                                                 edm::EventSetup const& iSetup,
//                                                                 kProbeKinematicVertexHistos& histos_vertex,
//                                                                 const std::string& name) const{
//     ibook.setCurrentFolder(outputInternalPath_);
//    
//     histos_vertex.hLxy = ibook.book1D(name + "_Vertex_Lxy",name + "_Vertex_Lxy", 100, 0, 70);
//     histos_vertex.hXError = ibook.book1D(name + "_Vertex_Xerror",name + "_Vertex_Xerror", 100, 0, 70);
//     histos_vertex.hYError = ibook.book1D(name + "_Vertex_Yerror",name + "_Vertex_Yerror", 100, 0, 70);
//     histos_vertex.hChi2 = ibook.book1D(name + "_Vertex_chi2",name + "_Vertex_chi2", 100, 0, 70);
//     histos_vertex.hZ = ibook.book1D(name + "_Vertex_z",name + "_Vertex_z", 100, 0, 70);
//}


// ------------ method fills 'descriptions' with the allowed parameters for the
// module  ------------
void ScoutingMuonTagProbeAnalyzer::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions) {
  // The following says we do not know what parameters are allowed so do no
  // validation Please change this to state exactly what you do use, even if it
  // is no parameters
  edm::ParameterSetDescription desc;
  desc.add<std::string>("OutputInternalPath", "MY_FOLDER");
  desc.add<edm::InputTag>("MuonCollection",
                          edm::InputTag("slimmedMuons"));
  desc.add<edm::InputTag>("ScoutingMuonCollection",
                          edm::InputTag("Run3ScoutingMuons"));
  desc.add<edm::InputTag>("ScoutingVtxCollection",
                          edm::InputTag("hltScoutingMuonPackerNoVtx"));
  descriptions.add("ScoutingMuonTagProbeAnalyzer", desc);
}



DEFINE_FWK_MODULE(ScoutingMuonTagProbeAnalyzer);
