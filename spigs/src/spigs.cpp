//============================================================================
// Name        : spigs.cpp
// Author      : Ondrej Chvala <ochvala@utk.edu>
// Version     :
// Copyright   : GNU/GPL
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <PigsDAQ.h>

#include <TFile.h>
#include <TH1D.h>

using namespace std;

int test1(void) {
	// Simple test subroutine
	int ret = 0;

	PigsDAQ *daq = PigsDAQ::getInstance();
	if (!ret) ret = daq->InitDPPLib();
	if (!ret) ret = daq->AddBoardUSB();
	if (!ret) ret = daq->BasicInit();
	if (!ret) ret = daq->ConfigureBoard();
	if (!ret) ret = daq->ConfigureChannel(0);
	if (!ret) {
		daq->PrintBoardInfo();
		daq->PrintChannelParameters(0);
		ret = daq->AcquisitionSingleLoop();
	}
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

int main(void) {
	int ret = 0;
	cout << "Welcome to SPIGS DAQ!" << endl;
	ret = test1();
	return ret;
}
