#include "UHH2/MTopJet/include/GenHists_xconeN5.h"
#include "UHH2/MTopJet/include/JetCluster.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/PFParticle.h"

#include "UHH2/common/include/TTbarGen.h"

#include <math.h>
#include <vector>

#include "TH1F.h"
#include "TH2F.h"
#include <iostream>

using namespace uhh2;

GenHists_xconeN5::GenHists_xconeN5(uhh2::Context & ctx, const std::string & dirname, const std::string & jetname): Hists(ctx, dirname){
  // book all histograms here
  Combined_Mass1 = book<TH1F>("Combined_Mass1", "M_{jet}", 150, 0, 1500);
  Combined_PT1 = book<TH1F>("Combined_PT1", "p_{T}", 50, 0, 1000);
  Combined_Mass2 = book<TH1F>("Combined_Mass2", "M_{jet}", 50, 0, 500);
  Combined_PT2 = book<TH1F>("Combined_PT2", "p_{T}", 50, 0, 1000);
  Subjet1_Mass = book<TH1F>("Subjet1_Mass", "M_{subjet1}", 60, 0, 300);
  Subjet2_Mass = book<TH1F>("Subjet2_Mass", "M_{subjet2}", 60, 0, 300);
  Subjet3_Mass = book<TH1F>("Subjet3_Mass", "M_{subjet3}", 60, 0, 300);
  Subjet1_PT = book<TH1F>("Subjet1_PT", "p_{T, subjet1}", 50, 0, 500);
  Subjet2_PT = book<TH1F>("Subjet2_PT", "p_{T, subjet2}", 50, 0, 500);
  Subjet3_PT = book<TH1F>("Subjet3_PT", "p_{T, subjet3}", 50, 0, 500);
  deltaR_lep_jet1 = book<TH1F>("deltaR_lep_jet1", "#Delta R (lep, jet1)", 80, 0, 4);
  deltaR_lep_jet2 = book<TH1F>("deltaR_lep_jet2", "#Delta R (lep, jet2)", 80, 0, 4);
  deltaR_lep_jet3 = book<TH1F>("deltaR_lep_jet3", "#Delta R (lep, jet3)", 80, 0, 4);
  deltaR_lep_jet4 = book<TH1F>("deltaR_lep_jet4", "#Delta R (lep, jet4)", 80, 0, 4);
  deltaR_lep_combinedjet1 = book<TH1F>("deltaR_lep_combinedjet1", "#Delta R (lep, combined jet1)", 80, 0, 4);
  deltaR_lep_combinedjet2 = book<TH1F>("deltaR_lep_combinedjet2", "#Delta R (lep, combined jet2)", 80, 0, 4);
  deltaR_combinedjet1_combinedjet2 = book<TH1F>("deltaR_combinedjet1_combinedjet2", "#Delta R (combined jet1, combined jet2)", 80, 0, 4);
  quark_next_to_lep = book<TH1F>("quark_next_to_lep", "quark next to lep", 5,-0.5, 4.5);


  // handle for TTbarGen class
  h_ttbargen=ctx.get_handle<TTbarGen>("ttbargen");
  // handle for clustered jets
  h_jets=ctx.get_handle<std::vector<Jet>>(jetname);
}



void GenHists_xconeN5::fill(const Event & event){


  //---------------------------------------------------------------------------------------
  //--------------------------------- define needed objects-----------------------------------
  //---------------------------------------------------------------------------------------
  const auto & ttbargen = event.get(h_ttbargen);
  // define all objects needed
  std::vector<Jet> jets = event.get(h_jets);
  TLorentzVector jet1_v4, jet2_v4, jet3_v4, jet4_v4, combinedjet1_v4, combinedjet2_v4, subjet1_v4, subjet2_v4, subjet3_v4;
  Jet jet1,jet2,jet3,jet4;
  if(jets.size()>0) jet1 = jets.at(0);
  if(jets.size()>1) jet2 = jets.at(1);
  if(jets.size()>2) jet3 = jets.at(2);
  if(jets.size()>3) jet4 = jets.at(3);

  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------
  //--------------------------------- Matching --------------------------------------------
  //---------------------------------------------------------------------------------------
  // bool matched = false;
  // get stable particles from ttbar decay and sort them into leptonic and hadronic
  GenParticle bot, q1, q2, bot_lep, lep1, lep2, lepton;
  if(jets.size() > 0){
    if(ttbargen.IsTopHadronicDecay()){
      bot = ttbargen.bTop();
      q1 = ttbargen.Wdecay1();
      q2 = ttbargen.Wdecay2();
      bot_lep = ttbargen.bAntitop();
      lep1 = ttbargen.WMinusdecay1();
      lep2 = ttbargen.WMinusdecay2();
    }
    else if(ttbargen.IsAntiTopHadronicDecay()){
      bot = ttbargen.bAntitop();
      q1 = ttbargen.WMinusdecay1();
      q2 = ttbargen.WMinusdecay2();
      bot_lep = ttbargen.bTop();
      lep1 = ttbargen.Wdecay1();
      lep2 = ttbargen.Wdecay2();
    }
    else if(!(ttbargen.IsTopHadronicDecay() || ttbargen.IsAntiTopHadronicDecay())){
	throw runtime_error("no hadronic Top found");
    }
   //check which lep is neutrino and which is elec/muon
    if(abs(lep1.pdgId()) == 11 || abs(lep1.pdgId()) == 13){
      lepton = lep1;
    }
    else if(abs(lep2.pdgId()) == 11 || abs(lep2.pdgId()) == 13){
      lepton = lep2;
    }
  }
  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------
  
  //---------------------------------------------------------------------------------------
  //-------------------------- get nearest quark to lepton --------------------------------
  //---------------------------------------------------------------------------------------
  float dR_lep_botlep, dR_lep_bot, dR_lep_q1, dR_lep_q2;
  dR_lep_botlep = deltaR(lepton, bot_lep);
  dR_lep_bot = deltaR(lepton, bot);
  dR_lep_q1 = deltaR(lepton, q1);
  dR_lep_q2 = deltaR(lepton, q2);
  int next_quark = 0;
  if(dR_lep_botlep < dR_lep_bot && dR_lep_botlep < dR_lep_q1 && dR_lep_botlep < dR_lep_q1) next_quark = 1;
  if(dR_lep_bot < dR_lep_botlep && dR_lep_bot < dR_lep_q1 && dR_lep_bot < dR_lep_q1) next_quark = 2;
  if(dR_lep_q1 < dR_lep_botlep && dR_lep_q1 < dR_lep_bot && dR_lep_q1 < dR_lep_q2) next_quark = 3;
  if(dR_lep_q2 < dR_lep_botlep && dR_lep_q2 < dR_lep_bot && dR_lep_q2 < dR_lep_q1) next_quark = 4;
  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------


  //---------------------------------------------------------------------------------------
  //-------- set Lorentz Vectors of 4 jets and combine 3 jets to topjet -------------------
  //---------------------------------------------------------------------------------------
  float dR1, dR2, dR3, dR4;
  if(jets.size() > 3){
    jet1_v4.SetPxPyPzE(jets.at(0).v4().Px(), jets.at(0).v4().Py(), jets.at(0).v4().Pz(), jets.at(0).v4().E());
    jet2_v4.SetPxPyPzE(jets.at(1).v4().Px(), jets.at(1).v4().Py(), jets.at(1).v4().Pz(), jets.at(1).v4().E()); 
    jet3_v4.SetPxPyPzE(jets.at(2).v4().Px(), jets.at(2).v4().Py(), jets.at(2).v4().Pz(), jets.at(2).v4().E()); 
    jet4_v4.SetPxPyPzE(jets.at(3).v4().Px(), jets.at(3).v4().Py(), jets.at(3).v4().Pz(), jets.at(3).v4().E()); 
  
    // claculate distance to Lepton
    dR1 = deltaR(jet1, lepton);
    dR2 = deltaR(jet2, lepton);
    dR3 = deltaR(jet3, lepton);
    dR4 = deltaR(jet4, lepton);
    if(dR1 < dR2 && dR1 < dR3 && dR1 < dR4){
      subjet1_v4 = jet2_v4;
      subjet2_v4 = jet3_v4;
      subjet3_v4 = jet4_v4;
      combinedjet1_v4 = jet2_v4 + jet3_v4 + jet4_v4;
      combinedjet2_v4 = jet1_v4;
    }
    if(dR2 < dR1 && dR2 < dR3 && dR2 < dR4){
      subjet1_v4 = jet1_v4;
      subjet2_v4 = jet3_v4;
      subjet3_v4 = jet4_v4;
      combinedjet1_v4 = jet1_v4 + jet3_v4 + jet4_v4;
      combinedjet2_v4 = jet2_v4;
    }    
    if(dR3 < dR1 && dR3 < dR2 && dR3 < dR4){
      subjet1_v4 = jet1_v4;
      subjet2_v4 = jet2_v4;
      subjet3_v4 = jet4_v4;
      combinedjet1_v4 = jet1_v4 + jet2_v4 + jet4_v4;
      combinedjet2_v4 = jet3_v4;
    }
    if(dR4 < dR1 && dR4 < dR2 && dR4 < dR3){
      subjet1_v4 = jet1_v4;
      subjet2_v4 = jet2_v4;
      subjet3_v4 = jet3_v4;
      combinedjet1_v4 = jet1_v4 + jet2_v4 + jet3_v4;
      combinedjet2_v4 = jet4_v4;
    }
  }
  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------
   

  //---------------------------------------------------------------------------------------
  //--------------------------------- Fill Hists here -------------------------------------
  //---------------------------------------------------------------------------------------

  // get weight
  double weight = event.weight;
  ////

  if((jets.size()) > 3){
    Combined_Mass1->Fill(combinedjet1_v4.M(),weight);
    Combined_PT1->Fill(combinedjet1_v4.Pt(),weight);
    Combined_Mass2->Fill(combinedjet2_v4.M(),weight);
    Combined_PT2->Fill(combinedjet2_v4.Pt(),weight);
    Subjet1_Mass->Fill(subjet1_v4.M(), weight);
    Subjet2_Mass->Fill(subjet2_v4.M(), weight);
    Subjet3_Mass->Fill(subjet3_v4.M(), weight);
    Subjet1_PT->Fill(subjet1_v4.Pt(), weight);
    Subjet2_PT->Fill(subjet2_v4.Pt(), weight);
    Subjet3_PT->Fill(subjet3_v4.Pt(), weight);
    deltaR_lep_jet1->Fill(dR1, weight);
    deltaR_lep_jet2->Fill(dR2, weight);
    deltaR_lep_jet3->Fill(dR3, weight);
    deltaR_lep_jet4->Fill(dR4, weight);
  }

  double dR_lep_comb1, dR_lep_comb2, dR_comb1_comb2;
  if((jets.size()) > 3){
    dR_lep_comb1 = sqrt( (combinedjet1_v4.Eta() - lepton.eta())*(combinedjet1_v4.Eta() - lepton.eta()) + (combinedjet1_v4.Phi() - lepton.phi())*(combinedjet1_v4.Phi() - lepton.phi()) );
    dR_lep_comb2 = sqrt( (combinedjet2_v4.Eta() - lepton.eta())*(combinedjet2_v4.Eta() - lepton.eta()) + (combinedjet2_v4.Phi() - lepton.phi())*(combinedjet2_v4.Phi() - lepton.phi()) );
    dR_comb1_comb2 = sqrt( (combinedjet1_v4.Eta() - combinedjet2_v4.Eta())*(combinedjet1_v4.Eta() - combinedjet2_v4.Eta()) + (combinedjet1_v4.Phi() - combinedjet2_v4.Phi())*(combinedjet1_v4.Phi() - combinedjet2_v4.Phi()) );
    deltaR_lep_combinedjet1->Fill(dR_lep_comb1, weight);
    deltaR_lep_combinedjet2->Fill(dR_lep_comb2, weight);
    deltaR_combinedjet1_combinedjet2->Fill(dR_comb1_comb2, weight);
  }
  quark_next_to_lep->Fill(next_quark, weight);

  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------


  //---------------------------------------------------------------------------------------
  //--------------------------------- Clear all used objects ------------------------------
  //---------------------------------------------------------------------------------------
  jet1_v4.Delete();
  jet2_v4.Delete();
  jet3_v4.Delete();
  jet4_v4.Delete();
  //---------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------- 

}


