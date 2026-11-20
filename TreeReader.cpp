#include "TreeReader.hpp"
#include <iomanip>
#include "TAxis.h"

using std::cout;
using std::endl;


//======================================================================

void TreeReader::InitHistos() {

  //Nplot1D("h_lam01", "; #lambda ; entries", 100, -1,1);

  plot1D("h_vertexMass", "; mass [MeV] ; entries", 150, 0,150);
 
}



//======================================================================
//======================================================================
//======================================================================


void TreeReader::Loop() {
   if (fChain == 0) return;

  //here we loop over all the frames in the file:
   for (Long64_t jentry=0; jentry<nentries_;jentry++) {
     ReadEvent(jentry);
    //cout<<jentry<<" "<<p->size()<<endl;


    for (uint i=0; i<mmass->size(); i++){

      Fill1D("h_vertexMass", mmass->at(i) );
      

      } 
    }
   }





