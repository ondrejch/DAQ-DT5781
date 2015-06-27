/*
 * PigsGUI.cpp
 *
 *  Created on: Jun 19, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include "PigsGUI.h"

int PigsGUI::InitDAQ() {
    // Initialization of the PigsDAQ object, DPP library, DAQ config
    int ret = 0;
    fDTinfo->AddLine("*** Initializing the DAQ ***");
    daq = PigsDAQ::getInstance();
    if (!ret) { ret = daq->InitDPPLib();  fDTinfo->AddLineFast("DPP firmware instantiated"); }
    if (!ret) { ret = daq->AddBoardUSB(); fDTinfo->AddLineFast("Board added"); }
    if (!ret) ret = daq->BasicInit();
    if (!ret) ret = daq->ConfigureBoard();
    if (!ret) ret = daq->ConfigureChannel(0);
    if (!ret) {
        daq->PrintBoardInfo();
        daq->PrintChannelParameters(0);
        fDTinfo->AddLineFast("Board configured");
        fStartDAQ->SetState(kButtonUp);
        TGText tbuff; tbuff.LoadBuffer(daq->getBoardInfo());
        fDTinfo->AddText(&tbuff);
        fDTinfo->Update();
        cout<<&tbuff<<endl;
    }
    daq->setGUI(this);            // set GUI pointer
    return ret;
}

int PigsGUI::DisconnectDAQ() {
    // Ends connection to the DPP library
    int ret = 0;
    fDTinfo->AddLine("*** Disconnecting the DAQ ***");
    if (daq) ret = daq->EndLibrary();
    return ret;
}

int PigsGUI::RunSingleAcquisition() {
    // Runs one acquisition loop
    int ret=0;
    fStartDAQ->SetState(kButtonDown);
    ret = daq->AcquisitionSingleLoop();
//    fAcqThread = new TThread("AcqThread",(void(*)(void *))daq->AcquisitionSingleLoop(), (void*) 0);
//    fAcqThread->Run();

    if(!ret) {
        daq->RefreshCurrHist();
        cCurrHCanvas->cd();
        daq->getCurrHist()->DrawClone();
        cCurrHCanvas->Modified();
        cCurrHCanvas->Update();
    }
    fStartDAQ->SetState(kButtonUp);
    return ret;
}

int PigsGUI::StopAcquisition() {
    // Stops acquisition on channel 0
    int ret = 0;
    if (daq) ret = daq->StopAcquisition(0);
    return ret;
}

void PigsGUI::SetProgressBarPosition(Float_t fposition) {
    // set position of the progress bar
    fHCurrHProgressBar->SetPosition(fposition);
}

PigsGUI::PigsGUI(const TGWindow *p) : TGMainFrame(p, fGUIsizeX, fGUIsizeY)  {
    daq = 0;
    fAcqThread=0;

    // *** Main GUI window ***
    fMainGUIFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
    fMainGUIFrame->SetName("fMainGUIFrame");
    fMainGUIFrame->SetLayoutBroken(kTRUE);
    // ufont = gClient->GetFont("-*-luxi_sans-bold-r-*-*-16-*-*-*-*-*-*-*");
    // ufont = gClient->GetFont("-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-iso8859-1");
    ufont = gClient->GetFont("-urw-nimbus sans l-bold-r-normal--0-0-0-0-p-0-iso8859-1");
    valTitle.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
    gClient->GetColorByName("#0000FF",valTitle.fForeground);
    gClient->GetColorByName("#e0e0e0",valTitle.fBackground);
    valTitle.fFillStyle = kFillSolid;
    valTitle.fFont = ufont->GetFontHandle();
    valTitle.fGraphicsExposures = kFALSE;
    uGC = gClient->GetGC(&valTitle, kTRUE);
    fMainTitle = new TGLabel(fMainGUIFrame,"Single-channel Position Identifying Gamma Sensor",uGC->GetGC(),ufont->GetFontStruct());
    fMainTitle->SetTextJustify(36);
    fMainTitle->SetMargins(0,0,0,0);
    fMainTitle->SetWrapLength(-1);
    fMainGUIFrame->AddFrame(fMainTitle, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fMainTitle->MoveResize(0,0,fGUIsizeX-4,32);
    fMainGUIFrame->Connect("CloseWindow()", "PigsGUI", this, "~PigsGUI()"); // call class destructor on alt+f4
    fMainGUIFrame->DontCallClose();

    // Buttons for main GUI
    fStartDAQ = new TGTextButton(fMainGUIFrame, "Start DAQ");     // start DAQ
    fStartDAQ->SetTextJustify(36);
    fStartDAQ->SetMargins(0,0,0,0);
    fStartDAQ->Resize(90,25);
    fMainGUIFrame->AddFrame(fStartDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fStartDAQ->MoveResize(50,fGUIsizeY-30,90,25);
    gClient->GetColorByName("green", fColor);
    fStartDAQ->ChangeBackground(fColor);
    fStartDAQ->SetState(kButtonDisabled);
    fStartDAQ->Connect("Clicked()","PigsGUI",this,"RunSingleAcquisition()");

    fStopDAQ = new TGTextButton(fMainGUIFrame, "Stop DAQ");        // stop DAQ
    fStopDAQ->SetTextJustify(36);
    fStopDAQ->SetMargins(0,0,0,0);
    fStopDAQ->Resize(90,25);
    fMainGUIFrame->AddFrame(fStopDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fStopDAQ->MoveResize(fGUIsizeX-50-90,fGUIsizeY-30,90,25);
    gClient->GetColorByName("red", fColor);
    fStopDAQ->ChangeBackground(fColor);
    fStopDAQ->SetState(kButtonDisabled);

    fExitDAQ = new TGTextButton(fMainGUIFrame, "Exit DAQ");        // exit DAQ
    fExitDAQ->SetTextJustify(36);
    fExitDAQ->SetMargins(0,0,0,0);
    fExitDAQ->Resize(90,25);
    fMainGUIFrame->AddFrame(fExitDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fExitDAQ->MoveResize(fGUIsizeX/2-45,fGUIsizeY-30,90,25);
    fExitDAQ->Connect("Clicked()","PigsGUI",this,"~PigsGUI()");

    // *** Tab widget ****
    fTabHolder = new TGTab(fMainGUIFrame,fGUIsizeX-4,fGUIsizeX-4);//,uGC->GetGC());

    // *** container of "CurrentHistogram" ***
    fCurHistFrame = fTabHolder->AddTab("CurrentHistogram");
    fCurHistFrame->SetLayoutManager(new TGVerticalLayout(fCurHistFrame));
    // embedded canvas
    fLatestHistoCanvas = new TRootEmbeddedCanvas(0,fCurHistFrame,fGUIsizeX-10,fGUIsizeY-140);
    Int_t wfLatestHistoCanvas = fLatestHistoCanvas->GetCanvasWindowId();
    cCurrHCanvas = new TCanvas("cCurrHCanvas", 10, 10, wfLatestHistoCanvas);
    fLatestHistoCanvas->AdoptCanvas(cCurrHCanvas);
    fCurHistFrame->AddFrame(fLatestHistoCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fHCurrHProgressBar = new TGHProgressBar(fCurHistFrame,fGUIsizeX-5);
    fHCurrHProgressBar->SetFillType(TGProgressBar::kBlockFill);
    fHCurrHProgressBar->ChangeOptions(kSunkenFrame | kDoubleBorder | kOwnBackground);

    // will reflect user color changes
    gClient->GetColorByName("#ffffff",fColor);
    fHCurrHProgressBar->SetBackgroundColor(fColor);
    fHCurrHProgressBar->SetPosition(10);
    fCurHistFrame->AddFrame(fHCurrHProgressBar, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** container of "History" ***
    fTabHisto = fTabHolder->AddTab("History");
    fTabHisto->SetLayoutManager(new TGVerticalLayout(fTabHisto));
    // embedded canvas
    fLastNspectra = new TRootEmbeddedCanvas(0,fTabHisto,fGUIsizeX-10,fGUIsizeY-110);
    Int_t wfLastNspectra = fLastNspectra->GetCanvasWindowId();
    cLastNspectra = new TCanvas("cLastNspectra", 10, 10, wfLastNspectra);
    fLastNspectra->AdoptCanvas(cLastNspectra);
    fTabHisto->AddFrame(fLastNspectra, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** container of "Config" ***
    fTabConfig = fTabHolder->AddTab("Config");
    fTabConfig->SetLayoutManager(new TGVerticalLayout(fTabConfig));

    // *** container of "DT5781" ***
    fTabDT5781 = fTabHolder->AddTab("DT5781");
    fTabDT5781->SetLayoutManager(new TGVerticalLayout(fTabDT5781));
    gClient->GetColorByName("white", fColor);
    fDTinfo = new TGTextView(fTabDT5781,fGUIsizeX-120,fGUIsizeY-150,"DAQ not initialized.",kSunkenFrame,fColor);
    fTabDT5781->AddFrame(fDTinfo, new TGLayoutHints(kLHintsNormal));
    fDTinfo->MoveResize(10,50,fGUIsizeX-120,fGUIsizeY-150);

    fInitDAQ = new TGTextButton(fTabDT5781, "Init DAQ");            // button InitDAQ
    fInitDAQ->SetTextJustify(36);
    fInitDAQ->SetMargins(0,0,0,0);
    fInitDAQ->Resize(90,25);
    fMainGUIFrame->AddFrame(fInitDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    gClient->GetColorByName("light blue", fColor);
    fInitDAQ->ChangeBackground(fColor);
    fInitDAQ->MoveResize(fGUIsizeX-50-60,50,90,25);
    fInitDAQ->Connect("Clicked()","PigsGUI",this,"InitDAQ()");
    fDisconnectDAQ = new TGTextButton(fTabDT5781, "Disconnect DAQ");  // buttons DisconnectDAQ
    fDisconnectDAQ->SetTextJustify(36);
    fDisconnectDAQ->SetMargins(0,0,0,0);
    fDisconnectDAQ->Resize(90,25);
    fMainGUIFrame->AddFrame(fDisconnectDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    gClient->GetColorByName("light blue", fColor);
    fDisconnectDAQ->MoveResize(fGUIsizeX-50-60,150,90,25);
    fDisconnectDAQ->ChangeBackground(fColor);
    fDisconnectDAQ->Connect("Clicked()","PigsGUI",this,"DisconnectDAQ()");

    // *** container of "About" ***
    fTabAbout = fTabHolder->AddTab("About");
    fTabAbout->SetLayoutManager(new TGVerticalLayout(fTabAbout));

    //-------------------------------------------------------------------------

    // change to the starting tab
    //fTabHolder->SetTab("CurrentHistogram");
    fTabHolder->SetTab("DT5781");

    // display GUI
    fTabHolder->Resize(fTabHolder->GetDefaultSize());
    fMainGUIFrame->AddFrame(fTabHolder, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fTabHolder->MoveResize(0,32,fGUIsizeX-2,fGUIsizeY-80);

    fMainGUIFrame->SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
    fMainGUIFrame->MapSubwindows();

    fMainGUIFrame->Resize(fMainGUIFrame->GetDefaultSize());
    fMainGUIFrame->MapWindow();
    fMainGUIFrame->Resize(fGUIsizeX,fGUIsizeY);
//    fDTinfo->DataChanged();

}

PigsGUI::~PigsGUI() {
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    StopAcquisition();
    DisconnectDAQ();
    // Clean up all widgets, frames and layout hints that were used
    Cleanup();
    gApplication->Terminate(0);
}
