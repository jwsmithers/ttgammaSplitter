//////////////////////////////
//Joshua.Wyatt.Smith@cern.ch//
//////////////////////////////
#include "ttgammaSplitter.h"
#include <iostream>
#include <memory>

using namespace std;


void m_add_branches(
  TChain *fChain_func,
  TEntryList *entryList,
  TTree *newtree_func,
  string filename,
  string source,
  string channel)
  {

  activateBranches(fChain_func); 

  int nentries = entryList->GetN();

///////
  TFile *original = new TFile(source.c_str(),"read"); 
  if ( original->IsOpen() ) std::cout << source << " file opened successfully" 
    << std::endl;
  string cutflow_str=channel+"_2015/cutflow_mc_pu_zvtx";
  TH1F* cutflow_hist = (TH1F*)original->Get(cutflow_str.c_str());
  Double_t cutflow_mc_pu_zvtx=cutflow_hist->GetBinContent(1);
///////

  EVENT_LUMI=36097.56;

  //For Herwig sample event_norm is:
  //xsec/cutflow_mc_pu_zvtx->GetBinContent(1)
  //
  //For ttgamma variations:
  //copy event_norm from nominal 410389 sample

  newtree_func->Branch("event_lumi",&EVENT_LUMI);   
  newtree_func->Branch("event_norm2",&EVENT_NORM2);   
  newtree_func->Branch("event_norm",&EVENT_NORM);   

  for (Long64_t event =0;event<nentries;event++) {
    int entryNumber = fChain_func->GetEntryNumber(event);
    if (entryNumber < 0) break;
       fChain_func->GetEntry(entryNumber);
    loadBar(event, nentries, 100, 50);

    if (mcChannelNumber==410405 || mcChannelNumber==410404){
      // based off sigma=3.356
      EVENT_NORM2=0.0008098;
      // based off sigma=4.623
      EVENT_NORM=0.0011155;

    } else if (mcChannelNumber==410395){
      EVENT_NORM2=3.35642/cutflow_mc_pu_zvtx;
      EVENT_NORM=4.6233/cutflow_mc_pu_zvtx;
    }


    newtree_func->Fill();

  }// end event loop
}// end add_nn loop

int main(int argc, char** argv)
{
  //gROOT->ProcessLine( "gErrorIgnoreLevel = kFatal;");
  std::cout << "Found " << argc-1 << " files to run over:" << std::endl;

  string path = "root://eosuser//eos/user/c/caudron2/TtGamma_PL/v010/";

  string channels[] ={"ejets","mujets","ee","emu","mumu"};
  //string channels[] ={"ee"};
  // Where we save to:
  // Remember to make the directory. I.e. mkdir ../SR1 ; cd ../SR1 ; mkdir emu mumu etc
  // I'm just too lazy.
  string outputPath = "root://eosatlas//eos/atlas/user/j/jwsmith/reprocessedNtuples/v010/particle_level/";


  for (int i = 1; i < argc; ++i) {
    for(const string &c : channels){

      TFile *newfile=nullptr;
      TFile *oldFile=nullptr;

      string filename = argv[i];
      string file = path+"/"+filename;
      std::cout<<c<<": Reading from "<<path<< std::endl;
      string newpath = outputPath + c+"/"+filename;
      std::cout<<c<<": "<< filename<< std::endl;
      std::cout<<c<<": Saving to "<<newpath<< std::endl;

      newfile = new TFile((newpath.c_str()), "recreate");
      oldFile = new TFile((file.c_str()), "read");

      TCut cut;
      //Channel specific selection:
      if (c.find("ejets") != std::string::npos) {
        cut="(ejets_2015 || ejets_2016)";
      }
      if (c.find("mujets") != std::string::npos) {
        cut="(mujets_2015 || mujets_2016)";
      }
      if (c.find("ee") != std::string::npos) {
        cut="(ee_2015 || ee_2016)";
      }
      if (c.find("emu") != std::string::npos) {
        cut="(emu_2015 || emu_2016)";
      }
      if (c.find("mumu") != std::string::npos) {
        cut="(mumu_2015 || mumu_2016)";
      }

      TChain *fChain=nullptr;
      TTree *newtree=nullptr;
 
      fChain = new TChain("nominal");
      fChain->Add((file).c_str());


      newfile->cd(); // Make sure we are in new file otherwise shit breaks.
  
      fChain->Draw(">>entrylist",cut,"entrylist");
      TEntryList *elist = (TEntryList*)gDirectory->Get("entrylist");
 
      fChain->SetEntryList(elist);
      newtree = fChain->CloneTree(0);
      if(fChain->GetEntries() == 0){
        std::cout<<"No events, is this an error? Skipping..."<<std::endl;
        delete newtree;
        delete fChain;
        continue;
      }
      m_add_branches(fChain,elist,newtree, filename,file,c);
      newfile->cd();
      newtree->Write();
      delete newtree;
      delete fChain;
      delete newfile;
      delete oldFile;
    } // end loop over channels
  }  // end loop over files

  return 0;
}
