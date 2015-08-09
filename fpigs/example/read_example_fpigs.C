/*
 * read_example_fpigs.C
 * ROOT/C++ macro that shows how to read the events in the data file.
 *
 * To run the macro in ROOT:
 $ cd DAQ-DT5781/fpigs/example
 $ root -l read_example_fpigs.C
 *
 *  Created on: Jul 10, 2015
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
int nev, i, ch;

using namespace std;

void read_example_fpigs()
{
    gSystem->Load("../lib/fPigsDict.so");    // Load the library with class dictionary
    f = TFile::Open("out-test-fpigs.root");  // Open the data file
    e = 0;                                   // Object to access data in the tree
    t = (TTree*)f->FindObjectAny("t");       // Get the tree with stored measurements
    t->SetBranchAddress("e", &e);            // Associate the object with the tree
    nev = t->GetEntries();                   // Get total number of measurements

    // Table header
    cout << "Measurement\t|\t\tcounts\t\t|\tdetector response"<<endl;
    cout << "  number   \t|ch0\t ch1\t ch2 \tch3 \t|ch0\t ch1\t ch2 \tch3"<<endl;
    // Loop over all measurements and show measured counts and detector response
    for (i=0; i<nev; i++) {
        t->GetEntry(i);               // Sets "e" to this particular measurement
        cout <<"\t"<< i<< "\t"<<      // Print information about that measurement
                e->totCounts[0] <<"\t"<< e->totCounts[1] <<"\t"<<
                e->totCounts[2] <<"\t"<< e->totCounts[3] <<"\t"<<
                e->detectorResponse[0] <<"\t"<< e->detectorResponse[1] <<"\t"<<
                e->detectorResponse[2] <<"\t"<< e->detectorResponse[2] <<endl;
    }

    // Show spectrum of 7th measurement on channel 0
    ch = 0;                     // Channel 0
    t->GetEntry(7);             // 7th measurement
    c = new TCanvas("c","show histogram",800,600);  // Canvas for the histogram
    c->SetLogx();               // Log scale on X-axis
    c->SetGrid();               // Nice grid
    e->spectrum[ch]->Draw();    // Draw the spectrum histogram
    e->Print();                 // Print information about this measurement
}
