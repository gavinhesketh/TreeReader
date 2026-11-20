#pragma once

#ifdef HAVE_MU3EUTIL
#include <mu3e/tools/RootFile.h>
using AnaFile = mu3e::RootFile;
inline AnaFile* OpenInputFile(const std::string& name) { return new mu3e::RootFile(name); }
inline AnaFile* OpenOutputFile(const std::string& name, AnaFile* parent) { return new mu3e::RootFile(name, parent); }
#else
#include "TFile.h"
using AnaFile = TFile;
inline AnaFile* OpenInputFile(const std::string& name) { return TFile::Open(name.c_str(), "READ"); }
inline AnaFile* OpenOutputFile(const std::string& name, AnaFile*) { return TFile::Open(name.c_str(), "RECREATE"); }
#endif

#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <filesystem>
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"


using std::vector;

class TreeReader {
public :
  TreeReader(bool MC, std::string filename, std::string output_file_name) : _MC(MC), fChain(0), nentries_(0) {

//auto-set tree about ouput name here based in input file


    std::cout<<"Reading "<<filename<< " and tree "<<TreeName()<<std::endl;
    AnaFile *f = OpenInputFile(filename);
    if (!f) return;
    f->GetObject(TreeName(), fChain);
    if (!fChain) return;

    fChain->SetMakeClass(1);
    fChain->SetBranchStatus("*",0);

    nentries_ = fChain->GetEntriesFast();
    std::cout<<"Tree contains "<<nentries_<<" entries."<<std::endl;

    TH1::SetDefaultSumw2();
    plots1D.clear();
    plots2D.clear();
    std::filesystem::create_directory("plot_files");
    output_file = OpenOutputFile("plot_files/"+output_file_name+".root", f);
    output_file->cd();
    InitHistos();
    Init();
  };

  ~TreeReader(){
    EndJob();
    if (!fChain) return;
    delete fChain->GetCurrentFile();
    output_file->Write();
    std::cout<<"Plots written to "<<output_file->GetName()<<std::endl;
    output_file->Close();
    delete output_file;
  };

  void     InitHistos();
  void     Init();
  void     Loop();
  void     ReadEvent(int jentry);
  void     EndJob() {};
  TString TreeName();

  template<class VAR> void Enable(TString name, VAR &var);

// some histogram functions:
  void plot1D(TString name, TString title, int nbins, float xlow, float xhigh){
     plots1D[name] = new TH1D(name, title, nbins, xlow, xhigh);
  };
  void Fill1D(TString name, double val, double weight=1) {
   plots1D[name]->Fill(val, weight);
  }
  inline void Fill(TString name, double val, double weight=1) {
    Fill1D(name, val, weight);
  }

  void plot2D(TString name, TString title, int xnbins, float xlow, float xhigh, int ynbins, float ylow, float yhigh){
    plots2D[name] = new TH2D(name, title, xnbins, xlow, xhigh, ynbins, ylow, yhigh);
  };
  void Fill2D(TString name, double xval, double yval, double weight=1){
    plots2D[name]->Fill(xval, yval, weight);
  }

 // void profile(TString name, TString title, int nbins, float xlow, float xhigh);
//  void FillProfile(TString name, double val, double weight=1);

private :
  bool _MC;

  std::map<TString, TH1D*> plots1D;
  std::map<TString, TH2D*> plots2D;

  AnaFile *output_file;
  TTree *fChain;
  int nentries_;
  int progress_counter=0;

  //variables to read from tree
   vector<double>  *vx;
   vector<double>  *vy;
   vector<double>  *vz;
   vector<double>  *mmass;
   vector<double>  *pxm;
   vector<double>  *pym;
   vector<double>  *pzm;
   vector<double>  *pm;




};

template<class VAR>
void TreeReader::Enable(TString name, VAR &var) {
    std::cout<<"Activating branch "<<name<<std::endl;
    var=0;
    fChain->SetBranchStatus(name,1);
    fChain->SetBranchAddress(name, &var);
}


TString TreeReader::TreeName() { return "vertex";}

void TreeReader::Init() {

  //enable branches you want to use
  //declare the variable in the class definition above first!
  Enable("vx", vx);
  Enable("vy", vy);
  Enable("vz", vz);
  Enable("mmass", mmass);
  Enable("pxm", pxm);
  Enable("pym", pym);
  Enable("pzm", pzm);
  Enable("pm", pm);

}


//============================================


void TreeReader::ReadEvent(int jentry) {
  int frac = 100*(jentry+1) / nentries_;
 // std::cout<<" "<<frac << " "<<progress_counter <<std::endl;
  if(frac==progress_counter) {
    std::cout<<"\r"<<"Processed "<<frac<<"% of "<<nentries_<<" "<<std::flush;
    progress_counter+=5;
  }
  fChain->LoadTree(jentry);
  fChain->GetEntry(jentry);

}




//============================================================================



namespace {

const std::string kDefaultInput  = "/scratch/Mu3e/RunTrirec/FramesAna_all.root";
const std::string kDefaultOutput = "plots";

void PrintUsage(const char* prog) {
  std::cout
    << "Usage: " << prog << " [options] [input.root]\n"
    << "\n"
    << "Options:\n"
    << "  -i, --input <file>    Input ROOT file (default: " << kDefaultInput << ")\n"
    << "  -o, --output <name>   Output name; writes plot_files/<name>.root (default: " << kDefaultOutput << ")\n"
    << "      --mc              Input is Monte Carlo (default)\n"
    << "      --data            Input is data (disables MC truth)\n"
    << "  -h, --help            Show this help and exit\n"
    << "\n"
    << "The input file can also be given as a single positional argument.\n";
}

} // namespace



int main(int argc, char *argv[]){

  std::string fileName = kDefaultInput;
  std::string outputFileName = kDefaultOutput;
  bool MC = true;
  bool inputSet = false;

  auto setInput = [&](const std::string& name) {
    if (inputSet) {
      std::cerr << "Error: input file given more than once" << std::endl;
      return false;
    }
    fileName = name;
    inputSet = true;
    return true;
  };

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    // Options that take a value: accept "-i file", "--input file" and "--input=file".
    auto takeValue = [&](const std::string& shortOpt, const std::string& longOpt, std::string& value) {
      if (arg == shortOpt || arg == longOpt) {
        if (i + 1 >= argc) {
          std::cerr << "Error: " << arg << " requires a value" << std::endl;
          std::exit(1);
        }
        value = argv[++i];
        return true;
      }
      if (arg.rfind(longOpt + "=", 0) == 0) {
        value = arg.substr(longOpt.size() + 1);
        return true;
      }
      return false;
    };

    std::string value;
    if (arg == "-h" || arg == "--help") {
      PrintUsage(argv[0]);
      return 0;
    }
    else if (arg == "--mc") {
      MC = true;
    }
    else if (arg == "--data") {
      MC = false;
    }
    else if (takeValue("-i", "--input", value)) {
      if (!setInput(value)) return 1;
    }
    else if (takeValue("-o", "--output", value)) {
      outputFileName = value;
    }
    else if (!arg.empty() && arg[0] == '-') {
      std::cerr << "Error: unknown option " << arg << "\n" << std::endl;
      PrintUsage(argv[0]);
      return 1;
    }
    else {
      if (!setInput(arg)) return 1;
    }
  }

  if (outputFileName.empty()) {
    std::cerr << "Error: output name must not be empty" << std::endl;
    return 1;
  }

  // Only check local files; leave remote URLs (root://, http://, ...) to ROOT.
  if (fileName.find("://") == std::string::npos && !std::filesystem::exists(fileName)) {
    std::cerr << "Error: input file not found: " << fileName << std::endl;
    return 1;
  }

  std::cout << "Input:  " << fileName << (inputSet ? "" : " (default)") << "\n"
            << "Output: plot_files/" << outputFileName << ".root\n"
            << "MC:     " << (MC ? "yes" : "no") << std::endl;

  TreeReader * a = new TreeReader(MC, fileName, outputFileName);
  a->Loop();
  delete a;

  return 0;
}
