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

#include <PigsEvent.h>


class PigsStorage: public TObject {
public:
    PigsStorage(TString foutname);  // Use this constructor
    PigsStorage();
    virtual ~PigsStorage();

    const PigsEvent * getE()    const { return e;   }
    const TFile * getOutFile()  const { return outf;}
    const TTree * getT()        const { return t;   }


private:
    TFile *outf;                // Filename to write the tree
    TString fFileName;          // Name of the output file
    TTree *t;                   // Tree with measurements
    PigsEvent *e;               // Measurement object

    ClassDef(PigsStorage, 0);
};

#endif /* PIGSSTORAGE_H_ */
