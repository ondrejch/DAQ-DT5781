/*
 * PigsStorage.h
 * Handles data storage
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#ifndef PIGSSTORAGE_H_
#define PIGSSTORAGE_H_

#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TString.h>

struct measurement {
    TH1D *spectrum;
    uint64_t realTime;
    uint64_t deadTime;
    uint32_t goodCounts;
    uint32_t totCounts;
    double countsPerSecond;
};

class PigsStorage: public TObject {
public:
    PigsStorage();
    virtual ~PigsStorage();

private:
    TTree *t;                   // Tree with measurements
    TFile *outf;                // Filename to write the tree
    TString fFileName;          // Name of the output file


    ClassDef(PigsStorage, 0);

};

#endif /* PIGSSTORAGE_H_ */
