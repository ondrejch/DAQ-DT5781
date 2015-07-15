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

class PigsEvent;

class PigsStorage : public TObject {
public:
    PigsStorage(TString foutname);  // Use this constructor
    PigsStorage();
    virtual ~PigsStorage();
    virtual void Print(Option_t *option="") const;  // Prints the storage information to stdout

    PigsEvent * getE()     { return e;   }
    TFile * getOutFile()   { return outf;}
    TTree * getTree()      { return t;   }
    const char *getOutFileName() const { return outf->GetName(); }

private:
    TFile *outf;                // Filename to write the tree
    TTree *t;                   // Tree with measurements
    PigsEvent *e;               // Measurement object

    ClassDef(PigsStorage, 1)
};

#endif /* PIGSSTORAGE_H_ */
