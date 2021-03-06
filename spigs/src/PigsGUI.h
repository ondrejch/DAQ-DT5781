/*
 * PigsGUI.h
 * Single channel version
 *
 *  Created on: Jun 19, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#ifndef PIGSGUI_H_
#define PIGSGUI_H_

// ROOT
#include <TObject.h>
#include <TRootEmbeddedCanvas.h>
#include <TGTab.h>
#include <TGLabel.h>
#include <TGProgressBar.h>
#include <TGButton.h>
#include <TCanvas.h>
#include <TGClient.h>
#include <TGTextView.h>
#include <TGNumberEntry.h>
#include <TH1D.h>
#include <TThread.h>
#include <TString.h>
#include <TTimeStamp.h>
#include <TApplication.h>
#include <Riostream.h>

// PIGS
#include <PigsDAQ.h>
#include <PigsStorage.h>
#include <PigsEvent.h>

class PigsDAQ;
class PigsStorage;
class PigsEvent;

class PigsGUI : public TGMainFrame  {
public:
    PigsGUI(const TGWindow *p); // Creates the GUI
    ~PigsGUI();                         // Ends DAQ, DPP, GUI
    int InitDAQ();                      // Initialization of the PigsDAQ object, DPP library, DAQ config
    int DisconnectDAQ();                // Ends connection to the DPP library
    int RunSingleAcquisition();         // Runs one acquisition loop
    int RunAcquisition();               // Loop acquisition
    int StopAcquisition();              // Stops acquisition loop
    int HardStopAcquisition();          // Stops acquisition on channel 0
    void SetAcquisitionLoopTime();                     // Changes acquisition time using GUI
    void SetProgressBarPosition(Float_t fposition);    // Set the position of the progress bar

private:
    void UpdateHistory();
    TGMainFrame *fMainGUIFrame;         // Main GUI window
    TGLabel *fMainTitle;                //
    TGTextButton *fStartDAQ, *fStopDAQ, *fExitDAQ; // buttons
    TGTab *fTabHolder;                  // tab widget
    TGCompositeFrame *fCurHistFrame;    // container of "CurrentHistogram"
    TRootEmbeddedCanvas *fLatestHistoCanvas;
    TCanvas *cCurrHCanvas;
    TGHProgressBar *fHCurrHProgressBar;
    TGCompositeFrame *fTabHisto;        // container of "History"
    TRootEmbeddedCanvas *fLastNspectra;
    TCanvas *cLastNspectra;
    TGCompositeFrame *fTabSum;          // container of "Sum"
    TRootEmbeddedCanvas *fSumSpectra;
    TCanvas *cSumSpectra;
    TGCompositeFrame *fTabConfig;       // container of "Config"
    TGGroupFrame *fControlFrame;
    TGNumberEntry *fAcqTimeEntry;
    TGCompositeFrame *fTabDT5781;       // container of "DT5781"
    TGTextView *fDTinfo;
    TGTextButton *fInitDAQ, *fDisconnectDAQ;  // buttons
    TGCompositeFrame *fTabAbout;        // container of "About"
    TGTextView *fAboutText;

    TGFont *ufont;                      // will reflect user font changes
    ULong_t fColor;                     // Color helper
    TGGC   *uGC;                        // will reflect user GC changes
    GCValues_t valTitle;                // graphics context changes
    TThread *fAcqThread;                // Acquisition thread
    PigsDAQ *daq;                       // DAQ pointer
    PigsStorage *storage;               // Data storage
    PigsEvent *ev;                      // Event buffer used to fill the storage
    TH1D *fNormAvgH;                    // Normalized average of last 9 measurements
    Bool_t keepAcquiring;               // Flag if we should continue loop

    TTimeStamp fDateTime;               // Current date for file name
    UInt_t year, month, day, hour, min, sec;

    static const int fGUIsizeX    = 600;
    static const int fGUIsizeY    = 500;
    static const int32_t fVerbose = 0;  // verbosity level settings
    const char *fAboutMsg;

    ClassDef(PigsGUI, 0);
};

#endif /* PIGSGUI_H_ */
