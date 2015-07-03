/*
 * PigsGUI.cpp
 *
 *  Created on: Jun 19, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include "PigsGUI.h"

int PigsGUI::InitDAQ() {
    // Initialization of the PigsDAQ object, DPP library, DAQ config, storage
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int ret = 0;

    if (storage) delete storage;                        // Storage initialization
    fDateTime.Set();
    fDateTime.GetDate(0, 0, &year, &month, &day);
    fDateTime.GetTime(0, 0, &hour, &min,   &sec);
    storage = new PigsStorage(Form("out-%04d%02d%02d_%02d:%02d:%02d.root",
            year,month,day,hour,min,sec));              // unique file name by using the current date and time
    ev      = storage->getE();
    fDTinfo->AddLine(Form("Output file: %s",storage->getOutFileName()));

    fDTinfo->AddLine("*** Initializing the DAQ ***");   // DAQ initialization
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
        fStartDAQ->SetState(kButtonUp);             // Enable acquisition
        fAcqTimeEntry->SetState(1);                 // Enable changing of the acquisition time
        this->SetAcquisitionLoopTime();             // Set default acquisition time
        TGText tbuff; tbuff.LoadBuffer(daq->getBoardInfo());
        fDTinfo->AddText(&tbuff);
        fDTinfo->Update();
    }
    daq->setGUI(this);            // set GUI pointer
    return ret;
}

int PigsGUI::DisconnectDAQ() {
    // Ends connection to the DPP library
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int ret = 0;
    fDTinfo->AddLine("*** Disconnecting the DAQ ***");
    if (daq) ret = daq->EndLibrary();
    return ret;
}

int PigsGUI::RunAcquisition() {
    // Runs acquisition as a loop
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int ret=0;
    fStartDAQ->SetState(kButtonDown);
    keepAcquiring = kTRUE;
    while(keepAcquiring) {                      // Acquisition loop
        ret = daq->ConfigureChannel(0);         // TODO Ideally this should only be called if acquisition parameters changed
        ret = daq->AcquisitionSingleLoop();
        if(!ret) {
            daq->RefreshCurrHist();             // transfer data to TH1D
            cCurrHCanvas->cd();
            daq->getCurrHist()->Draw();         // plot latest TH1D
            cCurrHCanvas->Modified();
            cCurrHCanvas->Update();
            gSystem->ProcessEvents();
            ev->spectrum  = daq->getCurrHist();   // save current measurement
            ev->realTime  = daq->getRealTime();
            ev->deadTime  = daq->getDeadTime();
            ev->goodCounts= daq->getGoodCounts();
            ev->totCounts = daq->getTotCounts();
            ev->countsPerSecond = daq->getCountsPerSecond();
            storage->getTree()->Fill();
            UpdateHistory();                     // Updates the history & average tabs
        }
        fHCurrHProgressBar->SetPosition(1);
    }
    fStartDAQ->SetState(kButtonUp);
    fStopDAQ->SetState(kButtonUp);
    return ret;
}

int PigsGUI::RunSingleAcquisition() {
    // Runs one acquisition loop
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int ret=0;
    ret = daq->ConfigureChannel(0);
    fStartDAQ->SetState(kButtonDown);
    ret = daq->AcquisitionSingleLoop();
//    fAcqThread = new TThread("AcqThread",(void(*)(void *))daq->AcquisitionSingleLoop(), (void*) 0);
//    fAcqThread->Run();

    if(!ret) {
        daq->RefreshCurrHist();             // transfer data to TH1D
        cCurrHCanvas->cd();
        daq->getCurrHist()->Draw();         // plot latest TH1D
        cCurrHCanvas->Modified();
        cCurrHCanvas->Update();
        gSystem->ProcessEvents();

        ev->spectrum  = daq->getCurrHist();   // save current measurement
        ev->realTime  = daq->getRealTime();
        ev->deadTime  = daq->getDeadTime();
        ev->goodCounts= daq->getGoodCounts();
        ev->totCounts = daq->getTotCounts();
        ev->countsPerSecond = daq->getCountsPerSecond();
        storage->getTree()->Fill();
        UpdateHistory();                     // Updates the history & average tabs
    }
    fHCurrHProgressBar->SetPosition(1);
    fStartDAQ->SetState(kButtonUp);
    return ret;
}

void PigsGUI::SetAcquisitionLoopTime() {
    // Changes the acquisition time
    if(fVerbose) std::cout << __PRETTY_FUNCTION__ << " -- val: " <<
            fAcqTimeEntry->GetNumberEntry()->GetNumber() << std::endl;
    if(daq) {
        daq->SetAcquisitionLoopTime(fAcqTimeEntry->GetNumberEntry()->GetNumber());
    }
}

void PigsGUI::UpdateHistory() {
    // Updates the history and average tabs
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    Long64_t totalEntries = storage->getTree()->GetEntries();
    Long64_t currentEntry = -1;
    if(fNormAvgH) fNormAvgH->Delete();
    fNormAvgH = (TH1D*) daq->getCurrHist()->Clone();
    if(fVerbose>1) cout << __PRETTY_FUNCTION__ << "totalEntries: " << totalEntries << endl;
    for (int i=0; i<9; i++) {       // loop over subcanvases
        currentEntry = totalEntries - i;
        if(currentEntry>0) {
            if(fVerbose>2) cout << __PRETTY_FUNCTION__ << "i: " << i << " entry: " << currentEntry << endl;
            cLastNspectra->GetPad(i+1)->cd();
            storage->getTree()->GetEntry(currentEntry);
            storage->getE()->spectrum->Draw();
            cLastNspectra->Modified();
            cLastNspectra->Update();
            if(i>0) fNormAvgH->Add(storage->getE()->spectrum);
        }
    }
    fNormAvgH->Scale(1.0/fNormAvgH->Integral()); // Histogram normalization
    cSumSpectra->cd();
    fNormAvgH->Draw();
    cSumSpectra->Modified();
    cSumSpectra->Update();
}

int PigsGUI::HardStopAcquisition() {
    // Stops acquisition on channel 0
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int ret = 0;
    if (daq) ret = daq->StopAcquisition(0);
    return ret;
}

int PigsGUI::StopAcquisition() {
    // Stops the acquisition loop
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int ret = 0;
    fStopDAQ->SetState(kButtonDown);
    if (daq && keepAcquiring) keepAcquiring = kFALSE;
    return ret;
}

void PigsGUI::SetProgressBarPosition(Float_t fposition) {
    // set position of the progress bar
    fHCurrHProgressBar->SetPosition(fposition);
    gClient->NeedRedraw(fHCurrHProgressBar);
}

PigsGUI::PigsGUI(const TGWindow *p) : TGMainFrame(p, fGUIsizeX, fGUIsizeY)  {
    // Creates the GUI
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    year = month = day = hour = min = sec = 0;
    daq = 0; storage = 0; ev = 0;
    fAcqThread = 0;
    fNormAvgH = 0;
    keepAcquiring = kFALSE;
    fAboutMsg = (char*)
"       _____  _____  ______ _______\n"
"      |_____]   |   |  ____ |______\n"
"      |       __|__ |_____| ______|\n"
"\n"
"\n"
"*** Position Indicating Gamma Sensor   ***\n"
" * CAEN DT-5781 Data Acquisition System *\n"
"        Single Channel Version\n"
"\n"
"  by Ondrej Chvala <ochvala@utk.edu>\n"
"       version 0.05, June 2015\n"
"  https://github.com/ondrejch/DAQ-DT5781\n"
"                GNU/GPL";

    // *** Main GUI window ***
    fMainGUIFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
    fMainGUIFrame->SetName("fMainGUIFrame");
    fMainGUIFrame->SetWindowName("S-PIGS");      // GUI window name
    fMainGUIFrame->SetLayoutBroken(kTRUE);
    ufont = gClient->GetFont("-*-*-bold-r-*-*-16-*-*-*-*-*-*-*");
    // ufont = gClient->GetFont("-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-iso8859-1");
    // ufont = gClient->GetFont("-urw-nimbus sans l-bold-r-normal--0-0-0-0-p-0-iso8859-1");
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
//    fStartDAQ->Connect("Clicked()","PigsGUI",this,"RunSingleAcquisition()");
    fStartDAQ->Connect("Clicked()","PigsGUI",this,"RunAcquisition()");

    fStopDAQ = new TGTextButton(fMainGUIFrame, "Stop DAQ");        // stop DAQ
    fStopDAQ->SetTextJustify(36);
    fStopDAQ->SetMargins(0,0,0,0);
    fStopDAQ->Resize(90,25);
    fMainGUIFrame->AddFrame(fStopDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fStopDAQ->MoveResize(fGUIsizeX-50-90,fGUIsizeY-30,90,25);
    gClient->GetColorByName("red", fColor);
    fStopDAQ->ChangeBackground(fColor);
//    fStopDAQ->SetState(kButtonDisabled);
    fStopDAQ->Connect("Clicked()","PigsGUI",this,"StopAcquisition()");

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
    fLatestHistoCanvas = new TRootEmbeddedCanvas("CurrentHEC",fCurHistFrame,fGUIsizeX-10,fGUIsizeY-140);
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
    fHCurrHProgressBar->SetPosition(1);
    fCurHistFrame->AddFrame(fHCurrHProgressBar, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** container of "History" ***
    fTabHisto = fTabHolder->AddTab("History");
    fTabHisto->SetLayoutManager(new TGVerticalLayout(fTabHisto));
    // embedded canvas
    fLastNspectra = new TRootEmbeddedCanvas("HistoryHEC",fTabHisto,fGUIsizeX-10,fGUIsizeY-110);
    Int_t wfLastNspectra = fLastNspectra->GetCanvasWindowId();
    cLastNspectra = new TCanvas("cLastNspectra", 10, 10, wfLastNspectra);
    fLastNspectra->AdoptCanvas(cLastNspectra);
    cLastNspectra->Divide(3,3);
    fTabHisto->AddFrame(fLastNspectra, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** container of "Sum" ***
    fTabSum = fTabHolder->AddTab("Sum");
    fTabSum->SetLayoutManager(new TGVerticalLayout(fTabSum));
    // embedded canvas
    fSumSpectra = new TRootEmbeddedCanvas("SumHEC",fTabSum,fGUIsizeX-10,fGUIsizeY-110);
    Int_t wfSumSpectra = fSumSpectra->GetCanvasWindowId();
    cSumSpectra = new TCanvas("cSumSpectra", 10, 10, wfSumSpectra);
    cSumSpectra->SetLogx();
    cSumSpectra->SetLogy();
    fSumSpectra->AdoptCanvas(cSumSpectra);
    fTabSum->AddFrame(fSumSpectra, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** container of "Config" ***
    fTabConfig = fTabHolder->AddTab("Config");
    fTabConfig->SetLayoutManager(new TGVerticalLayout(fTabConfig));
    fControlFrame = new TGGroupFrame(fTabConfig, "Acquisition time [sec]");
    fControlFrame->SetTitlePos(TGGroupFrame::kCenter);
    fAcqTimeEntry = new TGNumberEntry(fControlFrame, (Double_t) 10.0 ,5,-1,(TGNumberFormat::EStyle) 1,
            (TGNumberFormat::EAttribute) 2,(TGNumberFormat::ELimit) 2, 0.1, 600);
    (fAcqTimeEntry->GetNumberEntry())->Connect("TextChanged(char*)", "PigsGUI", this,
            "SetAcquisitionLoopTime()");
    (fAcqTimeEntry->GetNumberEntry())->Connect("ReturnPressed()", "PigsGUI", this,
            "SetAcquisitionLoopTime()");
    fControlFrame->AddFrame(fAcqTimeEntry, new TGLayoutHints(kLHintsNormal, 5, 5, 5, 5));
    fTabConfig->AddFrame(fControlFrame, new TGLayoutHints(kLHintsNormal, 20, 20, 20, 20));
    fAcqTimeEntry->SetState(0);

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
    ufont = gClient->GetFont("-*-fixed-medium-r-*-*-12-*-*-*-*-*-*-*");
    fAboutText = new TGTextView(fTabAbout,1,1,"SPIGS",kSunkenFrame);
    fAboutText->SetFont(ufont->GetFontStruct());
    fTabAbout->AddFrame(fAboutText, new TGLayoutHints(kLHintsNormal));
    fAboutText->LoadBuffer(fAboutMsg);

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

    fAboutText->MoveResize(80,30,320,220);
}

PigsGUI::~PigsGUI() {
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    HardStopAcquisition();
    DisconnectDAQ();
    if(storage) delete storage;
    // Clean up all widgets, frames and layout hints that were used
    Cleanup();
    gApplication->Terminate(0);
}
