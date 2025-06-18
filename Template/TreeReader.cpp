/*
TreeReader.cpp
Some simple code to read a root file and make some plots
Gavin Hesketh, gavin.hesketh@ucl.ac.uk
Started June 2025
*/

#include "TreeReader.h"


void TreeReader::EnableBranches() {

    tree->SetBranchStatus("*", 0);


    Enable("p", p);

}

void TreeReader::DefinePlots() {
    plotFile = new TFile(outputName.c_str(), "RECREATE");
    plotFile->cd();




}



void TreeReader::Run() {

   // nentries_ = 100;//tree->GetEntriesFast();

    cout<<"Looping over "<<nentries_<<" enries"<<endl;
    for(Long64_t jentry = 0; jentry < nentries_; jentry++) {
        ReadEvent(jentry);
    }

    cout<<"Finished!"<<endl;

}
