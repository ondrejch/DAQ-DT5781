//============================================================================
// Name        : spigs.cpp
// Author      : Ondrej Chvala <ochvala@utk.edu>
// Version     :
// Copyright   : GNU/GPL
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <PigsDAQ.h>
#include <PigsGUI.h>

#include <TFile.h>
#include <TH1D.h>
#include <TThread.h>
#include <TApplication.h>

using namespace std;

int test1(void) {
    // Simple test subroutine
    int ret = 0;
    TThread *t;

    PigsDAQ *daq = PigsDAQ::getInstance();
    t = new TThread("AcqThread",(void(*)(void *))daq->AcquisitionSingleLoop(), (void*) 0);
    if (!ret) ret = daq->InitDPPLib();
    if (!ret) ret = daq->AddBoardUSB();
    if (!ret) ret = daq->BasicInit();
    if (!ret) ret = daq->ConfigureBoard();
    if (!ret) ret = daq->ConfigureChannel(0);
    if (!ret) {
        daq->PrintBoardInfo();
        daq->PrintChannelParameters(0);
        //ret = daq->AcquisitionSingleLoop();
        //ret = daq->ThreadAcqSingleLoop();
        t->Run();
    }
    t->Join();
    if (!ret) {
        daq->PrintAcquisotionInfo();
        ret = daq->RefreshCurrHist();
    }
    if (!ret) {
        TFile *f = new TFile("AFile.root", "RECREATE", "Example");
        TH1D *h = daq->getCurrHist();
        h->Write();
        f->Write();
        f->Close();
    }
    ret = daq->EndLibrary();
    return ret;
}

int main(int argc, char *argv[]) {
    int ret = 0;
    cout << "Welcome to SPIGS DAQ!" << endl;
    // ret = test1(); // run simple non-GUI tests

    TApplication spigsApp("SPIGS", &argc, argv);
    new PigsGUI(gClient->GetRoot());     // Popup the GUI...
    spigsApp.Run();

    return ret;
}
