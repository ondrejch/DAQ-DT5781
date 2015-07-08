//============================================================================
// Name        : fpigs.cpp
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
#include <TEnv.h>
#include <TApplication.h>

using namespace std;

int main(int argc, char *argv[]) {
    int ret = 0;
    cout << "Welcome to FPIGS DAQ!" << endl;

//    gEnv->SetValue("Gui.Backend", "qt");
//    gEnv->SetValue("Gui.Factory", "qt");
    gEnv->SetValue("Gui.DefaultFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
    TApplication fpigsApp("F-PIGS", &argc, argv);
    new PigsGUI(gClient->GetRoot());     // Popup the GUI
    fpigsApp.Run();

    return ret;
}
