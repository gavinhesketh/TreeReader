#pragma once

/*
TreeReader.cpp
Some simple code to read a root file and make some plots
Gavin Hesketh, gavin.hesketh@ucl.ac.uk
Started June 2025
*/

#include <boost/program_options.hpp>

#include <filesystem>
#include <iostream>

#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

using std::cout;
using std::endl;
using std::vector;


class TreeReader {

    public:
    TreeReader(TTree * chain, std::string output_name) : tree(chain), outputName(output_name), plotFile(0) {
        plots1D.clear();
        plots2D.clear();
        EnableBranches();
        DefinePlots();
        nentries_ = tree->GetEntriesFast();
    };
    ~TreeReader() {
        plotFile->Write();
        cout<< "Wrote plots to " << plotFile->GetName() << endl;
        plotFile->Close();
        delete plotFile;
        delete tree;
    };


    void EnableBranches();
    void DefinePlots();
    void Run();

private:
    //activate a root tree branch:
template <class VAR>
void Enable(TString name, VAR& var) {
    std::cout << "Activating branch " << name << std::endl;
    var = 0;
    tree->SetBranchStatus(name, 1);
    tree->SetBranchAddress(name, &var);
}
  
    inline void plot1D(TString name, TString title, int nbins, float xlow, float xhigh){
        plots1D.push_back(plot1DMaker(name, title, nbins, xlow, xhigh));
      }

    TH1 * plot1DMaker(TString name, TString title, int nbins, float xlow, float xhigh);
    inline void plot2D(TString name, TString title, int xnbins, float xlow, float xhigh, int ynbins, float ylow, float yhigh){
        plots2D.push_back(plot2DMaker(name, title, xnbins, xlow, xhigh, ynbins, ylow, yhigh));
    }
    TH2 * plot2DMaker(TString name, TString title, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh);

      void Fill1D(TString name, double val, double weight);
      void Fill2D(TString name, double xval, double yval, double weight);
    

      inline void ReadEvent(int jentry) {
        const int frac = 100*(jentry+1) / nentries_;
        if(frac%5 == 0) {
          std::cout<<"\r"<<"Processed "<<frac<<"% of "<<nentries_<<" "<<std::flush;
        }
        tree->LoadTree(jentry);
        tree->GetEntry(jentry);
      }
      
      int nentries_ = 0; //number of entries in the tree
      TTree * tree;
      std::string outputName;
      TFile * plotFile;
      std::vector<TH1*> plots1D;
      std::vector<TH2*> plots2D;
  


   // Declaration of leaf types
   ULong_t         frameId;
   UInt_t          runId;
   Int_t           Nhit;
   vector<unsigned int> *hit_pixelid;
   vector<unsigned int> *hit_timestamp;
   vector<unsigned long> *debug_si_data;
   vector<unsigned long> *debug_si_ts;
   Int_t           Nfibremppc;
   vector<unsigned int> *fibremppc_mppc;
   vector<int>     *fibremppc_col;
   vector<unsigned int> *fibremppc_timestamp;
   Int_t           Ntilehit;
   vector<unsigned int> *tilehit_tile;
   vector<double>  *tilehit_time;
   vector<double>  *tilehit_edep;
   vector<unsigned int> *tilehit_timestamp;


};


//=================================================



TH1 * TreeReader::plot1DMaker(TString name, TString title, int nbins, float xlow, float xhigh){
    TH1 * p = new TH1D(name, title, nbins, xlow, xhigh);
    p->Sumw2();
   // p->SetDirectory(0);
    p->GetXaxis()->SetLabelSize(0.04);
    p->GetYaxis()->SetLabelSize(0.04);
    p->GetXaxis()->SetTitleOffset(1.2);
    p->GetYaxis()->SetTitleOffset(1.2);
    p->GetXaxis()->SetLabelFont(42);
    p->GetYaxis()->SetLabelFont(42);
    p->GetXaxis()->SetTitleFont(42);
    p->GetYaxis()->SetTitleFont(42);
    //  p->SetTitle(0);
    p->SetLineWidth(2);
    p->SetMarkerSize(1.5);
    return p;
  }
  
  TH2 * TreeReader::plot2DMaker(TString name, TString title, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh){
    TH2 * p = new TH2D(name, title, nbinsx, xlow, xhigh, nbinsy, ylow, yhigh);
    p->Sumw2();
  //  p->SetDirectory(0);
    p->GetXaxis()->SetLabelSize(0.04);
    p->GetYaxis()->SetLabelSize(0.04);
    p->GetXaxis()->SetTitleOffset(1.2);
    p->GetYaxis()->SetTitleOffset(1.2);
    p->GetXaxis()->SetLabelFont(42);
    p->GetYaxis()->SetLabelFont(42);
    p->GetXaxis()->SetTitleFont(42);
    p->GetYaxis()->SetTitleFont(42);
    // p->SetTitle(0);
    p->SetLineWidth(2);
    p->SetMarkerSize(1.5);
    p->SetContour(99);
    return p;
  }
  
  
  void TreeReader::Fill1D(TString name, double val, double weight) {
    for(int i=0; i<plots1D.size(); i++){
      if(name.CompareTo(plots1D[i]->GetName())) continue;
      plots1D[i]->Fill(val, weight);
      return;
    }
    std::cout<<"Plot1D "<<name<<" not found!"<<std::endl;
  }
  
  
  void TreeReader::Fill2D(TString name, double xval, double yval, double weight) {
    for(int i=0; i<plots2D.size(); i++){
      if(name.CompareTo(plots2D[i]->GetName())) continue;
      plots2D[i]->Fill(xval, yval, weight);
      return;
    }
    std::cout<<"Plot2D "<<name<<" not found!"<<std::endl;
  }
  
  


//=================================================================

int main(int argc, char* argv[]) {

    bool dumpText = 0;
    std::string cutString, treeName, weightName, inputName, outputName = "";

    namespace po = boost::program_options;
    po::options_description opts("Options");
    opts.add_options()
        ("output,o", po::value(&outputName), "Output plot file name (default = inputname_plots.root)")
        ("tree,t", po::value(&treeName)->default_value("mu3e"), "Tree to read")
        ("help", "Print this help message");

    // define hidden args
    po::options_description opts_p("Hidden Options");
    opts_p.add_options()
        ("input,i", po::value(&inputName));
    po::positional_options_description p;
    p.add("input", 1); //name, max count

    po::options_description allopts;
    allopts.add(opts).add(opts_p);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
        .options(allopts).positional(p).run(), vm
    );
    po::notify(vm);

    if(vm.count("help") || inputName.empty()) {
        printf("Usage: mu3eSkim  <INPUT> [OPTIONS]\n\n");
        opts.print(std::cout);
        exit(EXIT_SUCCESS);
    }

    if(!vm.count("output")) {
        std::string outstem = std::filesystem::path(inputName).stem();
        outputName = (outstem + "_plots.root");
    }

    std::cout << "Reading " << inputName << ", " << treeName << " tree." << std::endl;
    std::cout << "Writing plots to " << outputName << std::endl;



    //Get input file
    TFile* inputFile = TFile::Open(inputName.c_str(), "READ");
    if(inputFile==nullptr || inputFile->IsZombie()) {
        std::cout << "Could not open file: " << inputName << std::endl;
        return 1;
    }

    // open the input tree:
    TTree* inputTree = nullptr;
    inputFile->GetObject(treeName.c_str(), inputTree);

    if(inputTree == nullptr) {
        std::cout << "Could not find tree: " << treeName << std::endl;
        return 1;
    }

    TreeReader * TR = new TreeReader(inputTree, outputName);

    TR->Run();

    delete TR;
    delete inputFile;

    return 1;

}
