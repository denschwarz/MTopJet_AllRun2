#include <iostream>
#include <memory>

#include <UHH2/core/include/AnalysisModule.h>
#include <UHH2/core/include/Event.h>
#include <UHH2/core/include/Selection.h>

#include <UHH2/common/include/NSelections.h>
#include <UHH2/common/include/LumiSelection.h>
#include <UHH2/common/include/TriggerSelection.h>
#include <UHH2/common/include/JetCorrections.h>
#include <UHH2/common/include/ObjectIdUtils.h>
#include <UHH2/common/include/JetIds.h>
#include <UHH2/common/include/TopJetIds.h>
#include <UHH2/common/include/TTbarGen.h>
#include <UHH2/common/include/Utils.h>
#include <UHH2/common/include/AdditionalSelections.h>
#include "UHH2/common/include/YearRunSwitchers.h"

// Hists
#include <UHH2/common/include/ElectronHists.h>
#include <UHH2/common/include/EventHists.h>
#include <UHH2/common/include/MuonHists.h>
#include <UHH2/common/include/JetHists.h>
#include <UHH2/common/include/TTbarGenHists.h>
#include <UHH2/MTopJet/include/MTopJetHists.h>
//
#include <UHH2/MTopJet/include/ModuleBASE.h>
#include <UHH2/MTopJet/include/RecoSelections.h>
#include <UHH2/MTopJet/include/GenSelections.h>
#include <UHH2/MTopJet/include/MTopJetUtils.h>

using namespace std;

class MTopJetPreSelectionModule : public ModuleBASE {

public:
  explicit MTopJetPreSelectionModule(uhh2::Context&);
  virtual bool process(uhh2::Event&) override;

protected:
  // enum lepton { muon, elec };
  // lepton channel_;

  // selections
  std::unique_ptr<uhh2::Selection> lumi_sel;

  std::unique_ptr<uhh2::Selection> genmttbar_sel;
  std::unique_ptr<uhh2::Selection> genflavor_sel;
  std::unique_ptr<uhh2::Selection> muon_sel;
  std::unique_ptr<uhh2::Selection> elec_sel;
  std::unique_ptr<uhh2::Selection> met_sel;
  std::unique_ptr<uhh2::Selection> SemiLepDecay;
  std::unique_ptr<uhh2::Selection> GenMuonPT;
  std::unique_ptr<uhh2::Selection> GenElecPT;

  std::unique_ptr<uhh2::AnalysisModule> ttgenprod;

  // handles for output
  Event::Handle<bool>h_recsel;
  Event::Handle<bool>h_gensel;
  Event::Handle<std::vector<GenTopJet>>h_GENfatjets;
  Event::Handle<std::vector<TopJet>>h_fatjets;
  Event::Handle<TTbarGen> h_ttbargen;

  // bools
  bool isMC;
  bool isPhotonStream;
  bool isElectronStream;

  // store Hist collection as member variables
  std::unique_ptr<Hists> h_ttbar;

  //Year year;
};

/*
███    ███  ██████  ██████  ██    ██ ██      ███████
████  ████ ██    ██ ██   ██ ██    ██ ██      ██
██ ████ ██ ██    ██ ██   ██ ██    ██ ██      █████
██  ██  ██ ██    ██ ██   ██ ██    ██ ██      ██
██      ██  ██████  ██████   ██████  ███████ ███████
*/

MTopJetPreSelectionModule::MTopJetPreSelectionModule(uhh2::Context& ctx){

  //// CONFIGURATION
  bool isherwig;
  if(ctx.get("dataset_version") == "TTbar_powheg-herwig") isherwig = true;
  else isherwig = false;

  isMC = (ctx.get("dataset_type") == "MC");

  //HIST-classes
  h_ttbar.reset(new TTbarGenHists(ctx, "TTbar"));

  //year = extract_year(ctx); // Ask for the year of Event

  // const std::string& channel = ctx.get("channel", ""); //define Channel
  // if     (channel == "muon") channel_ = muon;
  // else if(channel == "elec") channel_ = elec;
  // else {
  //
  //   std::string log("TTbarLJAnalysisLiteModule::TTbarLJAnalysisLiteModule -- ");
  //   log += "invalid argument for 'channel' key in xml file (must be 'muon' or 'elec'): \""+channel+"\"";
  //
  //   throw std::runtime_error(log);
  // }

  h_recsel = ctx.declare_event_output<bool>("passed_recsel");
  h_gensel = ctx.declare_event_output<bool>("passed_gensel");
  h_GENfatjets = ctx.get_handle<std::vector<GenTopJet>>("genXCone33TopJets");
  h_fatjets = ctx.get_handle<std::vector<TopJet>>("xconeCHS");
  h_ttbargen = ctx.get_handle<TTbarGen>("ttbargen");
  ////

  //// COMMON MODULES

  if(!isMC) lumi_sel.reset(new LumiSelection(ctx));

  /* GEN M-ttbar selection [TTbar MC "0.<M^{gen}_{ttbar}(GeV)<700.] */
  const std::string ttbar_gen_label("ttbargen");

  ttgenprod.reset(new TTbarGenProducer(ctx, ttbar_gen_label, false));

  TString samplename = ctx.get("dataset_version");
  if( samplename.Contains("Mtt0000to0700") ){
    genmttbar_sel.reset(new MttbarGenSelection(0., 700.));
  }
  else{
    genmttbar_sel.reset(new uhh2::AndSelection(ctx));
  }

  //else throw runtime_error("In PreSelectionModule: This Event is not from 2016v3, 2017v2 or 2018!");

  /******************************************************************/

  //// EVENT SELECTION REC
  met_sel.reset(new METCut(40, uhh2::infinity));
  muon_sel.reset(new NMuonSelection(1, -1, MuonId(PtEtaCut(50, 2.4 ))));
  elec_sel.reset(new NElectronSelection(1, -1, ElectronId(PtEtaCut(50, 2.4))));
  ////

  //// EVENTS SELECTION GEN
  if(isMC && !isherwig) SemiLepDecay.reset(new TTbarSemilep(ctx));
  else if(isherwig) SemiLepDecay.reset(new TTbarSemilep_herwig(ctx));
  if(isMC){
    GenMuonPT.reset(new GenMuonSel(ctx, 55.));
    GenElecPT.reset(new GenElecSel(ctx, 55.));
  }
  ////

}

/*
██████  ██████   ██████   ██████ ███████ ███████ ███████
██   ██ ██   ██ ██    ██ ██      ██      ██      ██
██████  ██████  ██    ██ ██      █████   ███████ ███████
██      ██   ██ ██    ██ ██      ██           ██      ██
██      ██   ██  ██████   ██████ ███████ ███████ ███████
*/

bool MTopJetPreSelectionModule::process(uhh2::Event& event){

  bool passed_recsel;
  bool passed_gensel;

  if(!event.isRealData){
    /* GEN M-ttbar selection */
    ttgenprod->process(event);
    if(!genmttbar_sel->passes(event)) return false;
  }

  /* CMS-certified luminosity sections */
  if(event.isRealData){
    if(!lumi_sel->passes(event)) return false;
  }

  const bool pass_lep1 = ((event.muons->size() >= 1) || (event.electrons->size() >= 1));
  const bool pass_met = met_sel->passes(event);
  const bool pass_lepsel = (muon_sel->passes(event) || elec_sel->passes(event));

  // cut on fatjet pt
  bool passed_fatpt=false;
  std::vector<TopJet> jets = event.get(h_fatjets);
  double ptcut = 200;
  for(auto jet: jets){
    if(jet.pt() > ptcut) passed_fatpt = true;
  }
  ///

  bool pass_semilep;
  if(isMC)pass_semilep = SemiLepDecay->passes(event);
  else pass_semilep=false;

  bool pass_genlepton = false;
  bool pass_genmuon = false;
  bool pass_genelec = false;
  if(isMC){
    pass_genmuon = GenMuonPT->passes(event);
    pass_genelec = GenElecPT->passes(event);
  }
  if(pass_genmuon || pass_genelec) pass_genlepton = true;

  // cut on GEN Jet PT
  bool passed_genpt=false;
  if(isMC && pass_semilep && pass_genlepton){
    const auto & ttbargen = event.get(h_ttbargen);
    std::vector<GenTopJet> jets = event.get(h_GENfatjets);
    if(jets.size() > 1){
      GenParticle lepton = ttbargen.ChargedLepton();

      float dR1 = deltaR(lepton, jets.at(0));
      float dR2 = deltaR(lepton, jets.at(1));

      std::vector<GenJet> had_subjets;
      if(dR2 < dR1) had_subjets = jets.at(0).subjets();
      else had_subjets = jets.at(1).subjets();

      double px=0, py=0, pz=0, E=0;
      TLorentzVector jet_v4;
      for(unsigned int i=0; i < had_subjets.size(); ++i){
        px += had_subjets.at(i).v4().Px();
        py += had_subjets.at(i).v4().Py();
        pz += had_subjets.at(i).v4().Pz();
        E += had_subjets.at(i).v4().E();
      }
      jet_v4.SetPxPyPzE(px, py, pz, E);
      double pt = jet_v4.Pt();
      if(pt > 330) passed_genpt = true;
    }
  }
  ///

  if(pass_lep1 && passed_fatpt && pass_met && pass_lepsel) passed_recsel = true;
  else passed_recsel = false;

  if(pass_semilep && passed_genpt && pass_genlepton) passed_gensel = true;
  else passed_gensel = false;

  if(!passed_recsel && !passed_gensel) return false;

  event.set(h_recsel, passed_recsel);
  event.set(h_gensel, passed_gensel);

  //FILL HISTS
  h_ttbar->fill(event);

  return true;

}

UHH2_REGISTER_ANALYSIS_MODULE(MTopJetPreSelectionModule)
