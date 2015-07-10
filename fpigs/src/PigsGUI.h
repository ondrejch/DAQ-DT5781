/*
 * PigsGUI.h
 * Four channel version
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
#include <TH1F.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <TThread.h>
#include <TString.h>
#include <TTimeStamp.h>
#include <TColor.h>
#include <TArrow.h>
#include <TImage.h>
#include <TApplication.h>
#include <Riostream.h>

// PIGS
#include <PigsEvent.h>
#include <PigsStorage.h>
#include <PigsDAQ.h>

class PigsDAQ;
class PigsScalerInput;
class PigsIntLimInput;

class PigsGUI : public TGMainFrame  {
public:
    PigsGUI(const TGWindow *p);         // Creates the GUI
    ~PigsGUI();                         // Ends DAQ, DPP, GUI
    int32_t InitDAQ();                  // Initialization of the PigsDAQ object, DPP library, DAQ config
    int32_t DisconnectDAQ();            // Ends connection to the DPP library
    int32_t RunAcquisition();           // Loop acquisition
    int32_t StopAcquisition();          // Stops acquisition loop
    int32_t HardStopAcquisition();      // Stops acquisition on channel 0
    void SetAcquisitionLoopTime();                     // Changes acquisition time using GUI
    void SetProgressBarPosition(Float_t fposition);    // Set the position of the progress bar
    Float_t CalcResponseV1(int32_t ch); // Simple response function that returns scaled number of counts
    Float_t CalcResponseV2(int32_t ch); // Detector response function using energy integrals
    void SetGainScalerCh0();            // Changes scaler gain using GUI
    void SetGainScalerCh1();            // Changes scaler gain using GUI
    void SetGainScalerCh2();            // Changes scaler gain using GUI
    void SetGainScalerCh3();            // Changes scaler gain using GUI
    void SetIntegralLimitMin();         // Changes lower limit for energy integration using GUI
    void SetIntegralLimitMax();         // Changes upper limit for energy integration using GUI
    void ToggleUseIntegration();        // Use count sum or energy integration as detector response, set from GUI

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
    TRootEmbeddedCanvas *fLastMeas;
    TMultiGraph *fMG;
    TGraph *fGraph[4];
    TCanvas *cLastMeas;
    TGCompositeFrame *fTabSum;          // container of "Sum"
    TRootEmbeddedCanvas *fSumSpectra;
    TCanvas *cSumSpectra;
    TGCompositeFrame *fTabArrow;        // container of "Arrow"
    TRootEmbeddedCanvas *fArrowECanvas;
    TCanvas *cArrowCanvas;
    TGCompositeFrame *fTabConfig;       // container of "Config"
    TGGroupFrame *fControlFrame;
    TGNumberEntry *fAcqTimeEntry;
    TGGroupFrame *fScalerFrame;
    PigsScalerInput *fScalerInput[4];
    TGGroupFrame *fIntLimFrame;
    TGCheckButton *fUseIntegration;
    PigsIntLimInput *fIntLimInputMin;
    PigsIntLimInput *fIntLimInputMax;

    TGCompositeFrame *fTabDT5781;       // container of "DT5781"
    TGTextView *fDTinfo;
    TGTextButton *fInitDAQ, *fDisconnectDAQ;  // buttons
    TGCompositeFrame *fTabAbout;        // container of "About"
    TGTextView *fAboutText;

    TGFont *ufont;                      // Will reflect user font changes
    ULong_t fColor;                     // Color helper
    TGGC   *uGC;                        // Will reflect user GC changes
    GCValues_t valTitle;                // graphics context changes
    TThread *fAcqThread;                // Acquisition thread
    PigsDAQ *daq;                       // DAQ pointer
    PigsStorage *storage;               // Data storage
    PigsEvent *ev;                      // Event buffer used to fill the storage
    TH1F *fNormAvgH[4];                 // Normalized average of last 9 measurements
    Bool_t keepAcquiring;               // Flag if we should continue loop
    Bool_t useIntegration;              // Flag if integration is used for detector response

    TTimeStamp fDateTime;               // Current date for file name
    UInt_t year, month, day, hour, min, sec;

    Float_t fScaleFactor[4];            // Scaling of the detector response
    int32_t fIntegralMin, fIntegralMax; // Bin limits for integration used by CalcResponseV2

    static const int32_t fHistColors[4];        // Colors for history plot
    static const int32_t fGUIsizeX    = 1200;   // GUI size in pixels
    static const int32_t fGUIsizeY    = 1000;
    static const int32_t fVerbose = 1;  // Verbosity level settings
    const char *fAboutMsg;              // "About" tab text

    ClassDef(PigsGUI, 0)
};

//----------------------------------------------------------------------------

class PigsScalerInput : public TGHorizontalFrame {
// Auxiliary class for scaler value input
protected:
   TGNumberEntry *fEntry;
   static const Float_t fScalerMin = 0.5;
   static const Float_t fScalerMax = 2.0;

public:
   PigsScalerInput(const TGWindow *p, const char *name) : TGHorizontalFrame(p)  {
      fEntry = new TGNumberEntry(this, 1.0, 5, -1, TGNumberFormat::kNESRealThree,
              TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMinMax, fScalerMin, fScalerMax);
      AddFrame(fEntry, new TGLayoutHints(kLHintsLeft));
      TGLabel *label = new TGLabel(this, name);
      AddFrame(label, new TGLayoutHints(kLHintsLeft, 10));
   }
   TGNumberEntryField *GetEntry() const { return fEntry->GetNumberEntry(); }

   ClassDef(PigsScalerInput, 0)
};


class PigsIntLimInput : public TGHorizontalFrame {
// Auxiliary class for integral limit value input
protected:
   TGNumberEntry *fEntry;

public:
   PigsIntLimInput(const TGWindow *p, const char *name) : TGHorizontalFrame(p)  {
      fEntry = new TGNumberEntry(this, 1, 5, -1, TGNumberFormat::kNESInteger,
              TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMinMax, 1, 16384);
      AddFrame(fEntry, new TGLayoutHints(kLHintsLeft));
      TGLabel *label = new TGLabel(this, name);
      AddFrame(label, new TGLayoutHints(kLHintsLeft, 10));
   }
   TGNumberEntryField *GetEntry() const { return fEntry->GetNumberEntry(); }

   ClassDef(PigsIntLimInput, 0)
};
#endif /* PIGSGUI_H_ */
