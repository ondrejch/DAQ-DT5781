/*
 * Name        : fpigs.cpp
 * Author      : Ondrej Chvala <ochvala@utk.edu>
 * Version     : 0.08 beta
 * Copyright   : GNU/GPL
 * Description : Main program for Four-channel Position Indicating Gamma Sensor (F-PIGS)
 */

#include <iostream>
#include <PigsDAQ.h>
#include <PigsGUI.h>

#include <TFile.h>
#include <TH1D.h>
#include <TThread.h>
#include <TEnv.h>
#include <TApplication.h>

int main(int argc, char *argv[]) {
    int ret = 0;
    std::cout << "Welcome to F-PIGS DAQ!" << std::endl;

//    gEnv->SetValue("Gui.Backend", "qt");
//    gEnv->SetValue("Gui.Factory", "qt");
    gEnv->SetValue("Gui.DefaultFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
    TApplication fpigsApp("F-PIGS", &argc, argv);
    new PigsGUI(gClient->GetRoot());     // Popup the GUI
    fpigsApp.Run();

    return ret;
}
