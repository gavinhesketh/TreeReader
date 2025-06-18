/*
TreeReader.cpp
Some simple code to read a root file and make some plots
Gavin Hesketh, gavin.hesketh@ucl.ac.uk
Started June 2025
*/

#include "mu3eSort.h"


void TreeReader::EnableBranches() {

    tree->SetBranchStatus("*", 0);

/*
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
*/

    Enable("fibremppc_timestamp", fibremppc_timestamp);
    Enable("tilehit_timestamp", tilehit_timestamp);
    Enable("hit_timestamp", hit_timestamp);

}

void TreeReader::DefinePlots() {
    plotFile = new TFile(outputName.c_str(), "RECREATE");
    plotFile->cd();


    plot1D("nfibrehit", " ; Number of fibre hits; counts", 100, -0.5, 99.5);
    plot1D("ntilehit", " ; Number of tile hits; counts", 100, -0.5, 99.5);
    plot1D("npixelhit", " ; Number of pixel hits; counts", 100, -0.5, 99.5);



    plot1D("fibremppc_timestamp", "; Timestamp [8 ns];Counts", 1000, 0, 1000000);
    plot1D("tilehit_timestamp", ";Timestamp [8 ns];Counts", 1000, 0, 17000000);
    plot1D("hit_timestamp", ";Timestamp [8 ns];Counts", 1000, 0, 17000000);

    plot1D("delta_tl_p", ";#Delta t(tile - pixel) [8 ns];Counts", 200, -100, 100);

}



void TreeReader::Run() {

 //nentries_ = 10000;//tree->GetEntriesFast();


//from Nik:
//mu3e->SetAlias("f_ts","fibremppc_timestamp/256+(fibremppc_timestamp%256)/160.0");
//mu3e->SetAlias("t_ts","tilehit_timestamp/256+(tilehit_timestamp%256)/160.0");
//mu3e->SetAlias("delta","f_ts-hit_timestamp%(1<<24)");

    cout<<"Looping over "<<nentries_<<" enries"<<endl;
    for(Long64_t jentry = 0; jentry < nentries_; jentry++) {
        ReadEvent(jentry);


    Fill1D("nfibrehit", fibremppc_timestamp->size(), 1.0);
    Fill1D("ntilehit", tilehit_timestamp->size(), 1.0);
    Fill1D("npixelhit", hit_timestamp->size(), 1.0);






        for(int i = 0; i < fibremppc_timestamp->size(); i++) {
            double f_ts = (*fibremppc_timestamp)[i]/256+((*fibremppc_timestamp)[i]%256)/160.0;
            Fill1D("fibremppc_timestamp", f_ts, 1.0);
        }


        for(int j = 0; j < tilehit_timestamp->size(); j++) {
            double t_ts = (*tilehit_timestamp)[j]/256+((*tilehit_timestamp)[j]%256)/160.0;
            Fill1D("tilehit_timestamp", t_ts, 1.0);
        }


        for(int k = 0; k < hit_timestamp->size(); k++) {
            double p_ts = (*hit_timestamp)[k]%(1<<24);
            Fill1D("hit_timestamp", p_ts, 1.0);

            for(int j = 0; j < tilehit_timestamp->size(); j++) {
                double t_ts = (*tilehit_timestamp)[j]/256+((*tilehit_timestamp)[j]%256)/160.0;

                double delta = t_ts - p_ts;
                Fill1D("delta_tl_p", delta, 1.0);
            }
    


        }





    }

    cout<<"Finished!"<<endl;

}
