/*
 * PigsStorage.cpp
 * Handles data storage
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include <PigsStorage.h>

PigsStorage::PigsStorage() {
    // Constructor with a default file name
    outf = new TFile("pigs_outfile.root","RECREATE");
    e = new PigsEvent();
    t = new TTree("t",Form("PigsDAQ measurements - %s","pigs_outfile.root"));
    t->Branch("e","PigsEvent",&e);
}

PigsStorage::PigsStorage(TString foutname) {
    // Constructor using file name as an argument
    outf = new TFile(foutname.Data(),"RECREATE");
    e = new PigsEvent();
    t = new TTree("t",Form("PigsDAQ measurements - %s",foutname.Data()));
    t->Branch("e","PigsEvent",&e);
}

PigsStorage::~PigsStorage() {
    // Destructor writes out the data and closes the output file
    if(e) delete e;
    if(outf) {
        outf->Write();
        outf->Close();
        delete outf;
    }
}

void PigsStorage::Print(Option_t *) const {
    // Prints information about the storage
    std::cout << "*** Printing information of " << this->GetName() <<" ***"<< std::endl;
    std::cout << "File name: " << this->getOutFileName<< std::endl;
    std::cout << "Recorded events: " << t->GetEntries() << std::endl;
}
