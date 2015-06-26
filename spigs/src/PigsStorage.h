/*
 * PigsStorage.h
 * Handles data storage
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#ifndef PIGSSTORAGE_H_
#define PIGSSTORAGE_H_

#include <PigsEvent.h>

#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TString.h>


class PigsStorage: public TObject {
public:
    PigsStorage();
    virtual ~PigsStorage();

private:
    TFile *outf;                // Filename to write the tree
    TString fFileName;          // Name of the output file
    TTree *t;                   // Tree with measurements
    PigsEvent *e;               // Measurements

    ClassDef(PigsStorage, 0);

};

#endif /* PIGSSTORAGE_H_ */
