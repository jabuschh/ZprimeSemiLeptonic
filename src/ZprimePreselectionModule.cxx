#include <iostream>
#include <memory>

#include <UHH2/core/include/AnalysisModule.h>
#include <UHH2/core/include/Event.h>
#include <UHH2/core/include/Selection.h>
#include "UHH2/common/include/PrintingModules.h"

#include <UHH2/common/include/CleaningModules.h>
#include <UHH2/common/include/NSelections.h>
#include <UHH2/common/include/LumiSelection.h>
#include <UHH2/common/include/TriggerSelection.h>
#include <UHH2/common/include/JetCorrections.h>
#include <UHH2/common/include/ObjectIdUtils.h>
#include <UHH2/common/include/MuonIds.h>
#include <UHH2/common/include/ElectronIds.h>
#include <UHH2/common/include/JetIds.h>
#include <UHH2/common/include/TopJetIds.h>
#include <UHH2/common/include/TTbarGen.h>
#include <UHH2/common/include/Utils.h>
#include <UHH2/common/include/AdditionalSelections.h>
#include "UHH2/common/include/LuminosityHists.h"
#include <UHH2/common/include/MuonHists.h>
#include <UHH2/common/include/ElectronHists.h>
#include <UHH2/common/include/JetHists.h>
#include <UHH2/common/include/EventHists.h>

#include <UHH2/ZprimeSemiLeptonic/include/ModuleBASE.h>
#include <UHH2/ZprimeSemiLeptonic/include/ZprimeSemiLeptonicSelections.h>
#include <UHH2/ZprimeSemiLeptonic/include/ZprimeSemiLeptonicModules.h>
#include <UHH2/ZprimeSemiLeptonic/include/ZprimeSemiLeptonicPreselectionHists.h>
#include <UHH2/ZprimeSemiLeptonic/include/ZprimeSemiLeptonicGeneratorHists.h>

using namespace std;

class ZprimePreselectionModule : public ModuleBASE {

public:
  explicit ZprimePreselectionModule(uhh2::Context&);
  virtual bool process(uhh2::Event&) override;
  void book_histograms(uhh2::Context&, vector<string>);
  void fill_histograms(uhh2::Event&, string);

protected:

  // Corrections
  std::unique_ptr<JetCorrector>                    jet_corrector_B, jet_corrector_C, jet_corrector_DE, jet_corrector_F, jet_corrector_MC;
  std::unique_ptr<TopJetCorrector>                 topjet_corrector_B, topjet_corrector_C, topjet_corrector_DE, topjet_corrector_F, topjet_corrector_MC;
  std::unique_ptr<GenericTopJetCorrector>          topjet_puppi_corrector_B, topjet_puppi_corrector_C, topjet_puppi_corrector_DE, topjet_puppi_corrector_F, topjet_puppi_corrector_MC;
  std::unique_ptr<SubJetCorrector>                 topjet_subjet_corrector_B, topjet_subjet_corrector_C, topjet_subjet_corrector_DE, topjet_subjet_corrector_F, topjet_subjet_corrector_MC;
  std::unique_ptr<GenericSubJetCorrector>          topjet_puppi_subjet_corrector_B, topjet_puppi_subjet_corrector_C, topjet_puppi_subjet_corrector_DE, topjet_puppi_subjet_corrector_F, topjet_puppi_subjet_corrector_MC;

  std::unique_ptr<JetLeptonCleaner_by_KEYmatching> JLC_B, JLC_C, JLC_DE, JLC_F, JLC_MC;
  std::unique_ptr<JetLeptonCleaner_by_KEYmatching> TopJLC_B, TopJLC_C, TopJLC_DE, TopJLC_F, TopJLC_MC;
  std::unique_ptr<JetLeptonCleaner_by_KEYmatching> TopJLC_puppi_B, TopJLC_puppi_C, TopJLC_puppi_DE, TopJLC_puppi_F, TopJLC_puppi_MC;

  std::unique_ptr<GenericJetResolutionSmearer>     JER_smearer;
  std::unique_ptr<GenericJetResolutionSmearer>     TopJER_smearer;
  std::unique_ptr<GenericJetResolutionSmearer>     TopJER_puppi_smearer;


  // Cleaners
  std::unique_ptr<MuonCleaner>                     muon_cleaner;
  std::unique_ptr<ElectronCleaner>                 electron_cleaner;

  std::unique_ptr<JetCleaner>                      jet_IDcleaner, jet_cleaner1, jet_cleaner2;
  std::unique_ptr<TopJetCleaner>                   topjet_IDcleaner, topjet_cleaner, topjet_puppi_IDcleaner, topjet_puppi_cleaner;



  // Selections
  std::unique_ptr<uhh2::Selection> lumi_sel;
  std::unique_ptr<uhh2::AndSelection> metfilters_sel;

  std::unique_ptr<uhh2::Selection> genflavor_sel;

  std::unique_ptr<uhh2::Selection> jet1_sel;
  std::unique_ptr<uhh2::Selection> jet2_sel;
  std::unique_ptr<uhh2::Selection> met_sel;

  bool isMC, ispuppi;

  std::unique_ptr<Hists> lumihists;
  TString METcollection;


  // Runnumbers for applying different corrections
  constexpr static int s_runnr_B  = 299329; //up to this one, including this one
  constexpr static int s_runnr_C  = 302029; //up to this one, including this one
  constexpr static int s_runnr_DE = 304826; //up to this one, including this one
  constexpr static int s_runnr_F  = 306462; //up to this one, including this one

};

void ZprimePreselectionModule::book_histograms(uhh2::Context& ctx, vector<string> tags){
  for(const auto & tag : tags){
    string mytag = tag+"_General";
    book_HFolder(mytag, new ZprimeSemiLeptonicPreselectionHists(ctx,mytag));
    mytag = tag+"_Muons";
    book_HFolder(mytag, new MuonHists(ctx,mytag));
    mytag = tag+"_Electrons";
    book_HFolder(mytag, new ElectronHists(ctx,mytag));
    mytag = tag+"_Jets";
    book_HFolder(mytag, new JetHists(ctx,mytag));
    mytag = tag+"_Event";
    book_HFolder(mytag, new EventHists(ctx,mytag));
    mytag = tag+"_Generator";
    book_HFolder(mytag, new ZprimeSemiLeptonicGeneratorHists(ctx,mytag));
  }
}

void ZprimePreselectionModule::fill_histograms(uhh2::Event& event, string tag){
  string mytag = tag+"_General";
  HFolder(mytag)->fill(event);
  mytag = tag+"_Muons";
  HFolder(mytag)->fill(event);
  mytag = tag+"_Electrons";
  HFolder(mytag)->fill(event);
  mytag = tag+"_Jets";
  HFolder(mytag)->fill(event);
  mytag = tag+"_Event";
  HFolder(mytag)->fill(event);
  mytag = tag+"_Generator";
  HFolder(mytag)->fill(event);
}


ZprimePreselectionModule::ZprimePreselectionModule(uhh2::Context& ctx){

  for(auto & kv : ctx.get_all()){
    cout << " " << kv.first << " = " << kv.second << endl;
  }

  //// CONFIGURATION
  // const std::string& _METcollection = ctx.get("METName");
  const TString METcollection = ctx.get("METName");
  const bool isMC = (ctx.get("dataset_type") == "MC");
  const bool ispuppi = (ctx.get("is_puppi") == "true");
  cout << "Is this running on puppi: " << ispuppi << endl;

  ElectronId eleID = ElectronID_Fall17_tight_noIso;
  MuonId muID      = MuonID(Muon::CutBasedIdGlobalHighPt);
  double electron_pt(50.);
  double muon_pt(55.);
  double jet1_pt(50.);
  double jet2_pt(30.);
  double MET(50.);

  // COMMON MODULES

  if(!isMC) lumi_sel.reset(new LumiSelection(ctx));

  // MET filters
  metfilters_sel.reset(new uhh2::AndSelection(ctx, "metfilters"));
  if(!isMC){
    metfilters_sel->add<TriggerSelection>("1-good-vtx", "Flag_goodVertices");
    metfilters_sel->add<TriggerSelection>("globalTightHalo2016Filter", "Flag_globalTightHalo2016Filter");
    metfilters_sel->add<TriggerSelection>("HBHENoiseFilter", "Flag_HBHENoiseFilter");
    metfilters_sel->add<TriggerSelection>("HBHENoiseIsoFilter", "Flag_HBHENoiseIsoFilter");
    metfilters_sel->add<TriggerSelection>("EcalDeadCellTriggerPrimitiveFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter");
    if(!isMC)  metfilters_sel->add<TriggerSelection>("eeBadScFilter", "Flag_eeBadScFilter");
    metfilters_sel->add<TriggerSelection>("chargedHadronTrackResolutionFilter", "Flag_chargedHadronTrackResolutionFilter");
    metfilters_sel->add<TriggerSelection>("muonBadTrackFilter", "Flag_muonBadTrackFilter");
  }


  /******************************************************************/

  /* GEN Flavor selection [W+jets flavor-splitting] */
  if(ctx.get("dataset_version").find("WJets") != std::string::npos){

    if     (ctx.get("dataset_version").find("_B") != std::string::npos) genflavor_sel.reset(new GenFlavorSelection("b"));
    else if(ctx.get("dataset_version").find("_C") != std::string::npos) genflavor_sel.reset(new GenFlavorSelection("c"));
    else if(ctx.get("dataset_version").find("_L") != std::string::npos) genflavor_sel.reset(new GenFlavorSelection("l"));

    else genflavor_sel.reset(new uhh2::AndSelection(ctx));
  }
  else genflavor_sel.reset(new uhh2::AndSelection(ctx));
  /**************************************************/

  ////

  // Cleaning: Mu, Ele, Jets
  const MuonId muonID(AndId<Muon>(PtEtaCut(muon_pt, 2.4), muID));
  const ElectronId electronID(AndId<Electron>(PtEtaSCCut(electron_pt, 2.5), eleID));
  const JetId jetID(JetPFID(JetPFID::WP_TIGHT));


  muon_cleaner.reset(new MuonCleaner(muonID));
  electron_cleaner.reset(new ElectronCleaner(electronID));
  jet_IDcleaner.reset(new JetCleaner(ctx, jetID));
  jet_cleaner1.reset(new JetCleaner(ctx, 15., 3.0));
  jet_cleaner2.reset(new JetCleaner(ctx, 30., 2.4));
  topjet_IDcleaner.reset(new TopJetCleaner(ctx, jetID, "topjets"));
  topjet_cleaner.reset(new TopJetCleaner(ctx, TopJetId(PtEtaCut(400., 2.4)), "topjets"));
  topjet_puppi_IDcleaner.reset(new TopJetCleaner(ctx, jetID, "toppuppijets"));
  topjet_puppi_cleaner.reset(new TopJetCleaner(ctx, TopJetId(PtEtaCut(400., 2.4)), "toppuppijets"));

  // Jet energy corrections
  std::vector<std::string> JEC_AK4_MC, JEC_AK8_MC, JEC_AK4_MC_Puppi, JEC_AK8_MC_Puppi;
  std::vector<std::string> JEC_AK4_B,       JEC_AK4_C,       JEC_AK4_DE,       JEC_AK4_F;
  std::vector<std::string> JEC_AK4_B_Puppi, JEC_AK4_C_Puppi, JEC_AK4_DE_Puppi, JEC_AK4_F_Puppi;
  std::vector<std::string> JEC_AK8_B,       JEC_AK8_C,       JEC_AK8_DE,       JEC_AK8_F;
  std::vector<std::string> JEC_AK8_B_Puppi, JEC_AK8_C_Puppi, JEC_AK8_DE_Puppi, JEC_AK8_F_Puppi;

  JEC_AK4_B       = JERFiles::Fall17_17Nov2017_V32_B_L123_AK4PFchs_DATA;
  JEC_AK4_C       = JERFiles::Fall17_17Nov2017_V32_C_L123_AK4PFchs_DATA;
  JEC_AK4_DE       = JERFiles::Fall17_17Nov2017_V32_DE_L123_AK4PFchs_DATA;
  JEC_AK4_F       = JERFiles::Fall17_17Nov2017_V32_F_L123_AK4PFchs_DATA;
  JEC_AK4_MC       = JERFiles::Fall17_17Nov2017_V32_L123_AK4PFchs_MC;

  JEC_AK8_B       = JERFiles::Fall17_17Nov2017_V32_B_L123_AK8PFchs_DATA;
  JEC_AK8_C       = JERFiles::Fall17_17Nov2017_V32_C_L123_AK8PFchs_DATA;
  JEC_AK8_DE       = JERFiles::Fall17_17Nov2017_V32_DE_L123_AK8PFchs_DATA;
  JEC_AK8_F       = JERFiles::Fall17_17Nov2017_V32_F_L123_AK8PFchs_DATA;
  JEC_AK8_MC       = JERFiles::Fall17_17Nov2017_V32_L123_AK8PFchs_MC;

  JEC_AK4_B_Puppi = JERFiles::Fall17_17Nov2017_V32_B_L123_AK4PFPuppi_DATA;
  JEC_AK4_C_Puppi = JERFiles::Fall17_17Nov2017_V32_C_L123_AK4PFPuppi_DATA;
  JEC_AK4_DE_Puppi = JERFiles::Fall17_17Nov2017_V32_DE_L123_AK4PFPuppi_DATA;
  JEC_AK4_F_Puppi = JERFiles::Fall17_17Nov2017_V32_F_L123_AK4PFPuppi_DATA;
  JEC_AK4_MC_Puppi = JERFiles::Fall17_17Nov2017_V32_L123_AK4PFPuppi_MC;

  JEC_AK8_B_Puppi = JERFiles::Fall17_17Nov2017_V32_B_L123_AK8PFPuppi_DATA;
  JEC_AK8_C_Puppi = JERFiles::Fall17_17Nov2017_V32_C_L123_AK8PFPuppi_DATA;
  JEC_AK8_DE_Puppi = JERFiles::Fall17_17Nov2017_V32_DE_L123_AK8PFPuppi_DATA;
  JEC_AK8_F_Puppi = JERFiles::Fall17_17Nov2017_V32_F_L123_AK8PFPuppi_DATA;
  JEC_AK8_MC_Puppi = JERFiles::Fall17_17Nov2017_V32_L123_AK8PFPuppi_MC;

  if(!ispuppi){
    jet_corrector_B.reset(new JetCorrector(ctx,             JEC_AK4_B));
    jet_corrector_C.reset(new JetCorrector(ctx,             JEC_AK4_C));
    jet_corrector_DE.reset(new JetCorrector(ctx,            JEC_AK4_DE));
    jet_corrector_F.reset(new JetCorrector(ctx,             JEC_AK4_F));
    jet_corrector_MC.reset(new JetCorrector(ctx,            JEC_AK4_MC));
  }
  else{
    jet_corrector_B.reset(new JetCorrector(ctx,             JEC_AK4_B_Puppi));
    jet_corrector_C.reset(new JetCorrector(ctx,             JEC_AK4_C_Puppi));
    jet_corrector_DE.reset(new JetCorrector(ctx,            JEC_AK4_DE_Puppi));
    jet_corrector_F.reset(new JetCorrector(ctx,             JEC_AK4_F_Puppi));
    jet_corrector_MC.reset(new JetCorrector(ctx,            JEC_AK4_MC_Puppi));
  }

  topjet_corrector_B.reset(new TopJetCorrector(ctx,       JEC_AK8_B));
  topjet_corrector_C.reset(new TopJetCorrector(ctx,       JEC_AK8_C));
  topjet_corrector_DE.reset(new TopJetCorrector(ctx,      JEC_AK8_DE));
  topjet_corrector_F.reset(new TopJetCorrector(ctx,       JEC_AK8_F));
  topjet_corrector_MC.reset(new TopJetCorrector(ctx,      JEC_AK8_MC));

  topjet_puppi_corrector_B.reset(new GenericTopJetCorrector(ctx,  JEC_AK8_B_Puppi, "toppuppijets"));
  topjet_puppi_corrector_C.reset(new GenericTopJetCorrector(ctx,  JEC_AK8_C_Puppi, "toppuppijets"));
  topjet_puppi_corrector_DE.reset(new GenericTopJetCorrector(ctx, JEC_AK8_DE_Puppi, "toppuppijets"));
  topjet_puppi_corrector_F.reset(new GenericTopJetCorrector(ctx,  JEC_AK8_F_Puppi, "toppuppijets"));
  topjet_puppi_corrector_MC.reset(new GenericTopJetCorrector(ctx, JEC_AK8_MC_Puppi, "toppuppijets"));

  topjet_subjet_corrector_B.reset(new SubJetCorrector(ctx,               JEC_AK4_B));
  topjet_subjet_corrector_C.reset(new SubJetCorrector(ctx,               JEC_AK4_C));
  topjet_subjet_corrector_DE.reset(new SubJetCorrector(ctx,              JEC_AK4_DE));
  topjet_subjet_corrector_F.reset(new SubJetCorrector(ctx,               JEC_AK4_F));
  topjet_subjet_corrector_MC.reset(new SubJetCorrector(ctx,              JEC_AK4_MC));

  topjet_puppi_subjet_corrector_B.reset(new GenericSubJetCorrector(ctx,  JEC_AK4_B_Puppi, "toppuppijets"));
  topjet_puppi_subjet_corrector_C.reset(new GenericSubJetCorrector(ctx,  JEC_AK4_C_Puppi, "toppuppijets"));
  topjet_puppi_subjet_corrector_DE.reset(new GenericSubJetCorrector(ctx,  JEC_AK4_DE_Puppi, "toppuppijets"));
  topjet_puppi_subjet_corrector_F.reset(new GenericSubJetCorrector(ctx,  JEC_AK4_F_Puppi, "toppuppijets"));
  topjet_puppi_subjet_corrector_MC.reset(new GenericSubJetCorrector(ctx, JEC_AK4_MC_Puppi, "toppuppijets"));

  if(!ispuppi){
    JLC_B.reset(new JetLeptonCleaner_by_KEYmatching(ctx,           JEC_AK4_B));
    JLC_C.reset(new JetLeptonCleaner_by_KEYmatching(ctx,           JEC_AK4_C));
    JLC_DE.reset(new JetLeptonCleaner_by_KEYmatching(ctx,          JEC_AK4_DE));
    JLC_F.reset(new JetLeptonCleaner_by_KEYmatching(ctx,           JEC_AK4_F));
    JLC_MC.reset(new JetLeptonCleaner_by_KEYmatching(ctx,          JEC_AK4_MC));
  }
  else{
    JLC_B.reset(new JetLeptonCleaner_by_KEYmatching(ctx,           JEC_AK4_B_Puppi));
    JLC_C.reset(new JetLeptonCleaner_by_KEYmatching(ctx,           JEC_AK4_C_Puppi));
    JLC_DE.reset(new JetLeptonCleaner_by_KEYmatching(ctx,          JEC_AK4_DE_Puppi));
    JLC_F.reset(new JetLeptonCleaner_by_KEYmatching(ctx,           JEC_AK4_F_Puppi));
    JLC_MC.reset(new JetLeptonCleaner_by_KEYmatching(ctx,          JEC_AK4_MC_Puppi));
  }

  TopJLC_B.reset(new JetLeptonCleaner_by_KEYmatching(ctx,        JEC_AK8_B, "topjets"));
  TopJLC_C.reset(new JetLeptonCleaner_by_KEYmatching(ctx,        JEC_AK8_C, "topjets"));
  TopJLC_DE.reset(new JetLeptonCleaner_by_KEYmatching(ctx,       JEC_AK8_DE, "topjets"));
  TopJLC_F.reset(new JetLeptonCleaner_by_KEYmatching(ctx,        JEC_AK8_F, "topjets"));
  TopJLC_MC.reset(new JetLeptonCleaner_by_KEYmatching(ctx,       JEC_AK8_MC, "topjets"));

  TopJLC_puppi_B.reset(new JetLeptonCleaner_by_KEYmatching(ctx,  JEC_AK8_B_Puppi, "toppuppijets"));
  TopJLC_puppi_C.reset(new JetLeptonCleaner_by_KEYmatching(ctx,  JEC_AK8_C_Puppi, "toppuppijets"));
  TopJLC_puppi_DE.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JEC_AK8_DE_Puppi, "toppuppijets"));
  TopJLC_puppi_F.reset(new JetLeptonCleaner_by_KEYmatching(ctx,  JEC_AK8_F_Puppi, "toppuppijets"));
  TopJLC_puppi_MC.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JEC_AK8_MC_Puppi, "toppuppijets"));

  if(isMC){
    ctx.declare_event_input<std::vector<Particle> >(ctx.get("TopJetCollectionGEN"), "topjetsGEN");
    if(!ispuppi) JER_smearer.reset(new GenericJetResolutionSmearer(ctx, "jets", "genjets", JERSmearing::SF_13TeV_Fall17_V3, "Fall17_V3_MC_PtResolution_AK4PFchs.txt"));
    else JER_smearer.reset(new GenericJetResolutionSmearer(ctx, "jets", "genjets", JERSmearing::SF_13TeV_Fall17_V3, "Fall17_V3_MC_PtResolution_AK4PFPuppi.txt"));
    TopJER_smearer.reset(new GenericJetResolutionSmearer(ctx, "topjets", "topjetsGEN", JERSmearing::SF_13TeV_Fall17_V3, "Fall17_V3_MC_PtResolution_AK8PFchs.txt"));
    TopJER_puppi_smearer.reset(new GenericJetResolutionSmearer(ctx, "toppuppijets", "topjetsGEN", JERSmearing::SF_13TeV_Fall17_V3, "Fall17_V3_MC_PtResolution_AK8PFPuppi.txt"));
  }


  //// EVENT SELECTION
  jet1_sel.reset(new NJetSelection(1, -1, JetId(PtEtaCut(jet1_pt, 2.4))));
  jet2_sel.reset(new NJetSelection(2, -1, JetId(PtEtaCut(jet2_pt, 2.4))));
  met_sel  .reset(new METCut  (MET   , uhh2::infinity));


  // Book histograms
  vector<string> histogram_tags = {"Input", "Lumiselection", "Metfilters", "Lepton1", "JetCleaner1", "JetCleaner2", "TopjetCleaner", "Jet1", "Jet2", "MET"};
  book_histograms(ctx, histogram_tags);


  lumihists.reset(new LuminosityHists(ctx, "lumi"));
}








bool ZprimePreselectionModule::process(uhh2::Event& event){


  fill_histograms(event, "Input");
  // Lumi selection
  if(event.isRealData){
    if(!lumi_sel->passes(event)) return false;
    lumihists->fill(event);
  }
  fill_histograms(event, "Lumiselection");

  // MET filters
  if(!metfilters_sel->passes(event)) return false;
  fill_histograms(event, "Metfilters");

  // GEN ME quark-flavor selection
  if(!event.isRealData){
    if(!genflavor_sel->passes(event)) return false;
  }


  //// LEPTON selection
  muon_cleaner->process(event);
  sort_by_pt<Muon>(*event.muons);

  electron_cleaner->process(event);
  sort_by_pt<Electron>(*event.electrons);

  const bool pass_lep1 = ((event.muons->size() >= 1) || (event.electrons->size() >= 1));
  if(!pass_lep1) return false;

  fill_histograms(event, "Lepton1");


  //// JET selection
  jet_IDcleaner->process(event);

  if(event.isRealData){
    bool apply_B = false;
    bool apply_C = false;
    bool apply_DE = false;
    bool apply_F = false;
    if(event.run <= s_runnr_B)  apply_B = true;
    else if(event.run <= s_runnr_C) apply_C = true;
    else if(event.run <= s_runnr_DE) apply_DE = true;
    else if(event.run <= s_runnr_F) apply_F = true;
    else throw std::runtime_error("run number not covered by if-statements in process-routine.");

    if(apply_B+apply_C+apply_DE+apply_F != 1) throw std::runtime_error("In ZprimePreselectionModule.cxx: Sum of apply_* when applying JECs is not == 1. Fix this.");

    //apply proper JECs
    if(apply_B){
      JLC_B->process(event);
      TopJLC_B->process(event);
      TopJLC_puppi_B->process(event);
      jet_corrector_B->process(event);
      topjet_corrector_B->process(event);
      topjet_puppi_corrector_B->process(event);
      jet_corrector_B->correct_met(event);
      topjet_subjet_corrector_B->process(event);
      topjet_puppi_subjet_corrector_B->process(event);
    }
    if(apply_C){
      JLC_C->process(event);
      TopJLC_C->process(event);
      TopJLC_puppi_C->process(event);
      jet_corrector_C->process(event);
      topjet_corrector_C->process(event);
      topjet_puppi_corrector_C->process(event);
      jet_corrector_C->correct_met(event);
      topjet_subjet_corrector_C->process(event);
      topjet_puppi_subjet_corrector_C->process(event);
    }
    if(apply_DE){
      JLC_DE->process(event);
      TopJLC_DE->process(event);
      TopJLC_puppi_DE->process(event);
      jet_corrector_DE->process(event);
      topjet_corrector_DE->process(event);
      topjet_puppi_corrector_DE->process(event);
      jet_corrector_DE->correct_met(event);
      topjet_subjet_corrector_DE->process(event);
      topjet_puppi_subjet_corrector_DE->process(event);
    }
    if(apply_F){
      JLC_F->process(event);
      TopJLC_F->process(event);
      TopJLC_puppi_F->process(event);
      jet_corrector_F->process(event);
      topjet_corrector_F->process(event);
      topjet_puppi_corrector_F->process(event);
      jet_corrector_F->correct_met(event);
      topjet_subjet_corrector_F->process(event);
      topjet_puppi_subjet_corrector_F->process(event);
    }
  }
  else{ //MC
    JLC_MC->process(event);
    TopJLC_MC->process(event);
    TopJLC_puppi_MC->process(event);
    jet_corrector_MC->process(event);
    topjet_corrector_MC->process(event);
    topjet_puppi_corrector_MC->process(event);
    if(JER_smearer.get()) JER_smearer->process(event);
    if(TopJER_smearer.get()) TopJER_smearer->process(event);
    if(TopJER_puppi_smearer.get()) TopJER_puppi_smearer->process(event);
    jet_corrector_MC->correct_met(event);
    topjet_subjet_corrector_MC->process(event);
    topjet_puppi_subjet_corrector_MC->process(event);
  }

  jet_cleaner1->process(event);
  sort_by_pt<Jet>(*event.jets);
  fill_histograms(event, "JetCleaner1");

  // Lepton-2Dcut variables
  for(auto& muo : *event.muons){

    float    dRmin, pTrel;
    std::tie(dRmin, pTrel) = drmin_pTrel(muo, *event.jets);

    muo.set_tag(Muon::twodcut_dRmin, dRmin);
    muo.set_tag(Muon::twodcut_pTrel, pTrel);
  }

  for(auto& ele : *event.electrons){

    float    dRmin, pTrel;
    std::tie(dRmin, pTrel) = drmin_pTrel(ele, *event.jets);

    ele.set_tag(Electron::twodcut_dRmin, dRmin);
    ele.set_tag(Electron::twodcut_pTrel, pTrel);
  }


  jet_cleaner2->process(event);
  sort_by_pt<Jet>(*event.jets);
  fill_histograms(event, "JetCleaner2");

  topjet_IDcleaner->process(event);
  topjet_cleaner->process(event);
  sort_by_pt<TopJet>(*event.topjets);

  topjet_puppi_IDcleaner->process(event);
  topjet_puppi_cleaner->process(event);
  sort_by_pt<TopJet>(*event.toppuppijets);
  fill_histograms(event, "TopjetCleaner");

  // 1st AK4 jet selection
  const bool pass_jet1 = jet1_sel->passes(event);
  if(!pass_jet1) return false;
  fill_histograms(event, "Jet1");

  // 2nd AK4 jet selection
  const bool pass_jet2 = jet2_sel->passes(event);
  if(!pass_jet2) return false;
  fill_histograms(event, "Jet2");

  // MET selection
  const bool pass_met = met_sel->passes(event);
  if(!pass_met) return false;
  fill_histograms(event, "MET");

  return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(ZprimePreselectionModule)
