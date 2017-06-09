#include "UHH2/MTopJet/include/SubjetHists_xcone.h"


SubjetHists_xcone::SubjetHists_xcone(uhh2::Context & ctx, const std::string & dirname, const std::string & type): Hists(ctx, dirname){
  // book all histograms here
  pt_had_subjets = book<TH1F>("pt_had_subjets", "p_{T}^{had subjets}", 50, 0, 500);
  pt_had_subjets_fine = book<TH1F>("pt_had_subjets_fine", "p_{T}^{had subjets}", 100, 0, 500);
  eta_had_subjets = book<TH1F>("eta_had_subjets", "#eta^{had subjets}", 100, -5, 5);
  area_had_subjets = book<TH1F>("area_had_subjets", "jet area (had subjets)", 100, 0, 5);
  area_had1_subjet = book<TH1F>("area_had1_subjet", "jet area (had subjet 1)", 100, 0, 5);
  area_had2_subjet = book<TH1F>("area_had2_subjet", "jet area (had subjet 2)", 100, 0, 5);
  area_had3_subjet = book<TH1F>("area_had3_subjet", "jet area (had subjet 3)", 100, 0, 5);
  pt_lep_subjets = book<TH1F>("pt_lep_subjets", "p_{T}^{lep subjets}", 100, 0, 500);
  eta_lep_subjets = book<TH1F>("eta_lep_subjets", "#eta^{lep subjets}", 100, -5, 5);
  area_lep_subjets = book<TH1F>("area_lep_subjets", "jet area (lep subjets)", 100, 0, 5);
  area_lep1_subjet = book<TH1F>("area_lep1_subjet", "jet area (lep subjet 1)", 100, 0, 5);
  area_lep2_subjet = book<TH1F>("area_lep2_subjet", "jet area (lep subjet 2)", 100, 0, 5);
  area_lep3_subjet = book<TH1F>("area_lep3_subjet", "jet area (lep subjet 3)", 100, 0, 5);
  area_all_subjets = book<TH1F>("area_all_subjets", "jet area (all subjets)", 50, 0, 1);
  pt_all_subjets = book<TH1F>("pt_all_subjets", "p_{T}^{all subjets}", 100, 0, 500);
  eta_all_subjets = book<TH1F>("eta_all_subjets", "#eta^{all subjets}", 100, -5, 5);
  JEC_all_subjets = book<TH1F>("JEC_all_subjets", "JEC factor", 100, 0, 2);
  JEC_L1_all_subjets = book<TH1F>("JEC_L1_all_subjets", "JEC factor L1", 100, 0, 2);
  JEC_L2L3_all_subjets = book<TH1F>("JEC_L2L3_all_subjets", "JEC factor L2L3", 100, 0, 2);

  pt_check = book<TH1F>("pt_check", "p_{T}^{all subjets} * 1/JEC_SF", 100, 0, 500);

  area_final = book<TH1F>("area_final", "jet area (final had jet)", 100, 0, 3);
  area_pt_final = book<TH2F>("area_pt_final", "x=area y=PT", 100, 0, 3,  100, 0, 2000);

  pt_lep_combine = book<TH1F>("pt_lep_combine", "p_{T}^{lep combined}", 50, 0, 1000);
  pt_had_combine = book<TH1F>("pt_had_combine", "p_{T}^{had combined}", 50, 0, 1000);
  mass_lep_combine = book<TH1F>("mass_lep_combine", "M^{lep combined}", 50, 0, 500);
  mass_had_combine = book<TH1F>("mass_had_combine", "M^{had combined}", 50, 0, 500);
  mass_had_combine_cut = book<TH1F>("mass_had_combine_cut", "M^{had combined}", 50, 0, 500);

  min_mass_Wjet = book<TH1F>("min_mass_Wjet", "min M_{ij}", 50, 50, 150);
  all_mass_Wjet = book<TH1F>("all_mass_Wjet", "all M_{ij}", 50, 50, 150);

  min_mass_Wjet_zoom = book<TH1F>("min_mass_Wjet_zoom", "min M_{ij}", 60, 60, 120);

  area_ak4 = book<TH1F>("area_ak4", "jet area (ak4)", 50, 0, 1);
  pt_ak4 = book<TH1F>("pt_ak4", "p_{T}^{ak4}", 100, 0, 500);
  eta_ak4 = book<TH1F>("eta_ak4", "#eta^{ak4}", 100, -5, 5);
  JEC_ak4 = book<TH1F>("JEC_ak4", "JEC factor", 100, 0, 2);
  JEC_L1_ak4 = book<TH1F>("JEC_L1_ak4", "JEC factor L1", 100, 0, 2);
  JEC_L2L3_ak4 = book<TH1F>("JEC_L2L3_ak4", "JEC factor L2L3", 100, 0, 2);

  // handle for jets
  if(type == "raw") h_recfatjets=ctx.get_handle<std::vector<TopJet>>("XConeTopJets_noJEC");
  else if(type == "jec") h_recfatjets=ctx.get_handle<std::vector<TopJet>>("XConeTopJets");
  else if(type == "cor") h_recfatjets=ctx.get_handle<std::vector<TopJet>>("XConeTopJets_Corrected");
}



void SubjetHists_xcone::fill(const Event & event){

  // get weight
  double weight = event.weight;

  //---------------------------------------------------------------------------------------
  //--------------------------------- define needed objects-----------------------------------
  //---------------------------------------------------------------------------------------
  // define all objects needed
  std::vector<TopJet> rec_fatjets = event.get(h_recfatjets);
  // std::vector<Jet> ak4_jets = event.jets;

  std::vector<Jet> had_subjets, lep_subjets;
  Particle lepton;
  if(event.muons->size() > 0 && event.electrons->size() > 0){
    return;
  }
  if(event.muons->size() > 0){
    lepton = event.muons->at(0);
  }
  else if(event.electrons->size() > 0){
    lepton = event.electrons->at(0);
  }
  double dR1 = uhh2::deltaR(lepton, rec_fatjets.at(0));
  double dR2 = uhh2::deltaR(lepton, rec_fatjets.at(1));
  if(dR1 > dR2){
    had_subjets = rec_fatjets.at(0).subjets();
    lep_subjets = rec_fatjets.at(1).subjets();
  }
  else if(dR1 < dR2){
    lep_subjets = rec_fatjets.at(0).subjets();
    had_subjets = rec_fatjets.at(1).subjets();
  }

  //---------------------------------------------------------------------------------------
  //------------------------ add subjets without and with  pt cut--------------------------
  //---------------------------------------------------------------------------------------
  double px=0, py=0, pz=0, E=0;
  double px_cut=0, py_cut=0, pz_cut=0, E_cut=0;
  TLorentzVector had_jet_v4, had_jet_v4_cut;
  for(unsigned int i=0; i < had_subjets.size(); ++i){
    px += had_subjets.at(i).v4().Px();
    py += had_subjets.at(i).v4().Py();
    pz += had_subjets.at(i).v4().Pz();
    E += had_subjets.at(i).v4().E();
    if(had_subjets.at(i).v4().Pt() > 30){
      px_cut += had_subjets.at(i).v4().Px();
      py_cut += had_subjets.at(i).v4().Py();
      pz_cut += had_subjets.at(i).v4().Pz();
      E_cut += had_subjets.at(i).v4().E();
    }
  }
  had_jet_v4.SetPxPyPzE(px, py, pz, E);
  had_jet_v4_cut.SetPxPyPzE(px_cut, py_cut, pz_cut, E_cut);

  px=0;
  py=0;
  pz=0;
  E=0;
  TLorentzVector lep_jet_v4;
  for(unsigned int i=0; i < lep_subjets.size(); ++i){
    px += lep_subjets.at(i).v4().Px();
    py += lep_subjets.at(i).v4().Py();
    pz += lep_subjets.at(i).v4().Pz();
    E += lep_subjets.at(i).v4().E();
  }
  lep_jet_v4.SetPxPyPzE(px, py, pz, E);


  //---------------------------------------------------------------------------------------
  //--------------------------------- add subjets to reconstruct W ------------------------
  //---------------------------------------------------------------------------------------
  TLorentzVector Wjet12, Wjet13, Wjet23;
  px = had_subjets.at(0).v4().Px() + had_subjets.at(1).v4().Px();
  py = had_subjets.at(0).v4().Py() + had_subjets.at(1).v4().Py();
  pz = had_subjets.at(0).v4().Pz() + had_subjets.at(1).v4().Pz();
  E = had_subjets.at(0).v4().E() + had_subjets.at(1).v4().E();
  Wjet12.SetPxPyPzE(px, py, pz, E);
  px = had_subjets.at(0).v4().Px() + had_subjets.at(2).v4().Px();
  py = had_subjets.at(0).v4().Py() + had_subjets.at(2).v4().Py();
  pz = had_subjets.at(0).v4().Pz() + had_subjets.at(2).v4().Pz();
  E = had_subjets.at(0).v4().E() + had_subjets.at(2).v4().E();
  Wjet13.SetPxPyPzE(px, py, pz, E);
  px = had_subjets.at(1).v4().Px() + had_subjets.at(2).v4().Px();
  py = had_subjets.at(1).v4().Py() + had_subjets.at(2).v4().Py();
  pz = had_subjets.at(1).v4().Pz() + had_subjets.at(2).v4().Pz();
  E = had_subjets.at(1).v4().E() + had_subjets.at(2).v4().E();
  Wjet23.SetPxPyPzE(px, py, pz, E);

  double M12, M13, M23, M_min = 1000;
  M12 = Wjet12.M();
  M13 = Wjet13.M();
  M23 = Wjet23.M();
  if(M12 < M13 && M12 < M23) M_min = M12;
  if(M13 < M12 && M13 < M23) M_min = M13;
  if(M23 < M12 && M23 < M13) M_min = M23;

  if(M_min != 1000){
    min_mass_Wjet->Fill(M_min, weight);
    min_mass_Wjet_zoom->Fill(M_min, weight);
  }
  all_mass_Wjet->Fill(M12, weight);
  all_mass_Wjet->Fill(M13, weight);
  all_mass_Wjet->Fill(M23, weight);

  //---------------------------------------------------------------------------------------
  //--------------------------------- Fill Hists here -------------------------------------
  //---------------------------------------------------------------------------------------
  double JEC_factor,JEC_L1factor;
  for(unsigned int i=0; i<event.jets->size(); i++){
    pt_ak4->Fill(event.jets->at(i).pt(), weight);
    eta_ak4->Fill(event.jets->at(i).eta(), weight);
    area_ak4->Fill(event.jets->at(i).jetArea(), weight);

    JEC_factor = 1./(event.jets->at(i).JEC_factor_raw());
    JEC_L1factor = event.jets->at(i).JEC_L1factor_raw();
    JEC_ak4->Fill(JEC_factor, weight);
    JEC_L1_ak4->Fill(JEC_L1factor, weight);
    JEC_L2L3_ak4->Fill(JEC_factor/JEC_L1factor, weight);
  }

  double tot_area=0;
  for(unsigned int i=0; i<had_subjets.size(); i++){
    pt_had_subjets->Fill(had_subjets.at(i).pt(), weight);
    pt_had_subjets_fine->Fill(had_subjets.at(i).pt(), weight);
    eta_had_subjets->Fill(had_subjets.at(i).eta(), weight);
    area_had_subjets->Fill(had_subjets.at(i).jetArea(), weight);
    pt_all_subjets->Fill(had_subjets.at(i).pt(), weight);
    eta_all_subjets->Fill(had_subjets.at(i).eta(), weight);
    area_all_subjets->Fill(had_subjets.at(i).jetArea(), weight);
    tot_area += had_subjets.at(i).jetArea();
    JEC_factor = 1./(had_subjets.at(i).JEC_factor_raw());
    JEC_L1factor = had_subjets.at(i).JEC_L1factor_raw();
    JEC_all_subjets->Fill(JEC_factor, weight);
    JEC_L1_all_subjets->Fill(JEC_L1factor, weight);
    JEC_L2L3_all_subjets->Fill(JEC_factor/JEC_L1factor, weight);
    pt_check->Fill((had_subjets.at(i).JEC_factor_raw()) * had_subjets.at(i).pt(), weight);
  }
  area_final->Fill(tot_area, weight);
  area_pt_final->Fill(tot_area, had_jet_v4.Pt(), weight);

  for(unsigned int i=0; i<lep_subjets.size(); i++){
    pt_lep_subjets->Fill(lep_subjets.at(i).pt(), weight);
    eta_lep_subjets->Fill(lep_subjets.at(i).eta(), weight);
    area_lep_subjets->Fill(lep_subjets.at(i).jetArea(), weight);
    pt_all_subjets->Fill(lep_subjets.at(i).pt(), weight);
    eta_all_subjets->Fill(lep_subjets.at(i).eta(), weight);
    area_all_subjets->Fill(lep_subjets.at(i).jetArea(), weight);

    JEC_factor = 1./(lep_subjets.at(i).JEC_factor_raw());
    JEC_L1factor = lep_subjets.at(i).JEC_L1factor_raw();
    JEC_all_subjets->Fill(JEC_factor, weight);
    JEC_L1_all_subjets->Fill(JEC_L1factor, weight);
    JEC_L2L3_all_subjets->Fill(JEC_factor/JEC_L1factor, weight);
    pt_check->Fill((lep_subjets.at(i).JEC_factor_raw()) * lep_subjets.at(i).pt(), weight);
  }
  area_had1_subjet->Fill(had_subjets.at(0).jetArea(), weight);
  area_had2_subjet->Fill(had_subjets.at(1).jetArea(), weight);
  area_had3_subjet->Fill(had_subjets.at(2).jetArea(), weight);
  area_lep1_subjet->Fill(lep_subjets.at(0).jetArea(), weight);
  area_lep2_subjet->Fill(lep_subjets.at(1).jetArea(), weight);
  area_lep3_subjet->Fill(lep_subjets.at(2).jetArea(), weight);
  pt_had_combine->Fill(had_jet_v4.Pt(), weight);
  pt_lep_combine->Fill(lep_jet_v4.Pt(), weight);
  mass_had_combine->Fill(had_jet_v4.M(), weight);
  mass_had_combine_cut->Fill(had_jet_v4_cut.M(), weight);
  mass_lep_combine->Fill(lep_jet_v4.M(), weight);
  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------
 

}

