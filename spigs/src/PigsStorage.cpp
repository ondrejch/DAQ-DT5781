/*
 * PigsStorage.cpp
 * Handles data storage
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include <PigsStorage.h>

PigsStorage::PigsStorage() {
    t=0; outf=0;
    fFileName = "outfile.root";
}

PigsStorage::~PigsStorage() {
    if(outf) {
        outf->Write();
        outf->Close();
        delete outf;
    }
    if(t) delete t;
}

