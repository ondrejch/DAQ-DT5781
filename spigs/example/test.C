/*
 * test.C
 * ROOT/C++ macro that shows how to read the events in the data file.
 *
 *  Created on: June 28, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>

class PigsEvent;

TCanvas   *c;       // It is useful to declare the variables outside the macro.
TFile     *f;       // Then these can be used in the interactive session after
TTree     *t;       // the macro finishes.
PigsEvent *e;
int nev, i;

void test() {
    gSystem->Load("./sPigsDict.so");
    f = TFile::Open("out-test.root");
    t = (TTree*)f->FindObjectAny("t");
    e = 0;
    t->SetBranchAddress("e",&e);
    nev = t->GetEntries();

    for (i=0;i<nev;i++) {
        t->GetEntry(i);
        std::cout << "Measurement " << i << " \t counts: " << e->totCounts << std::endl;
    }

    t->GetEntry(7);
    c = new TCanvas("c","show histogram",800,600);
    e->spectrum->Draw();
    //  e->Dump();
}
