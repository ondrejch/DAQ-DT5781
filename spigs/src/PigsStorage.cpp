/*
 * PigsStorage.cpp
 * Handles data storage
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include <PigsStorage.h>

PigsStorage::PigsStorage() {
    t=0; outf=0; e=0;
    fFileName = "outfile.root";
}

PigsStorage::PigsStorage(TString foutname) {
    outf = new TFile(foutname.Data(),"RECREATE");
    e = new PigsEvent();
    t = new TTree("t",Form("PigsDAQ measurements - %s",foutname.Data()));
    t->Branch("e","PigsEvent",&e);
}

PigsStorage::~PigsStorage() {
    if(outf) {
        outf->Write();
        outf->Close();
        delete outf;
    }
    if(t) delete t;
}

