/*
 * PigsGUI.cpp
 * Four channel version
 *
 *  Created on: Jun 19, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include "PigsGUI.h"

int32_t PigsGUI::InitDAQ() {
    // Initialization of the PigsDAQ object, DPP library, DAQ configuration, storage
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int32_t ret = 0;
    int32_t ch;

    if (storage) delete storage;                        // Storage initialization
    fDateTime.Set();
    fDateTime.GetDate(0, 0, &year, &month, &day);
    fDateTime.GetTime(0, 0, &hour, &min,   &sec);
    storage = new PigsStorage(Form("out-%04d%02d%02d_%02d:%02d:%02d.root",
            year,month,day,hour,min,sec));              // Unique file name by using the current date and time
    ev      = storage->getE();
    fDTinfo->AddLine(Form("Output file: %s",storage->getOutFileName()));

    fDTinfo->AddLine("*** Initializing the DAQ ***");   // DAQ initialization
    daq = PigsDAQ::getInstance();
    if (!ret) { ret = daq->InitDPPLib();  fDTinfo->AddLineFast("DPP firmware instantiated"); }
    if (!ret) { ret = daq->AddBoardUSB(); fDTinfo->AddLineFast("Board added"); }
    if (!ret) ret = daq->BasicInit();
    if (!ret) ret = daq->ConfigureBoard();
    for (ch=0; ch<4;ch++) if (!ret) ret = daq->ConfigureChannel(ch);
    if (!ret) {
        daq->PrintBoardInfo();
        if(fVerbose) for (ch=0; ch<4;ch++) daq->PrintChannelParameters(ch);
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

int32_t PigsGUI::DisconnectDAQ() {
    // Ends connection to the DPP library
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int32_t ret = 0;
    fDTinfo->AddLine("*** Disconnecting the DAQ ***");
    if (daq) ret = daq->EndLibrary();
    return ret;
}

int32_t PigsGUI::RunAcquisition() {
    // Runs acquisition as a loop
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int32_t ret=0;
    int32_t ch;
    Float_t currentAcqTime  = -2.0;     // used to check if we need to reconfigure channels
    Float_t previousAcqTime = -1.0;
    fStartDAQ->SetState(kButtonDown);
    fStopDAQ->SetState(kButtonUp);
    fUseIntegration->SetState(kButtonDisabled);
    keepAcquiring = kTRUE;
    while(keepAcquiring) {                      // Acquisition loop
        currentAcqTime  = daq->GetAcquisitionLoopTime();
        if(currentAcqTime != previousAcqTime) { // Acquisition time changed, reconfigure channels
            for (ch=0; ch<4;ch++) ret += daq->ConfigureChannel(ch);
            previousAcqTime = currentAcqTime;
        }
        ret += daq->AcquisitionSingleLoop();    // Run data acquisition
        if(!ret) {
            daq->RefreshCurrHist();             // transfer data to TH1F
            for (ch=0; ch<4;ch++) {
                cCurrHCanvas->GetPad(ch+1)->cd();
                daq->getCurrHist(ch)->Draw();         // plot latest TH1F
                cCurrHCanvas->GetPad(ch+1)->Update();
                ev->spectrum[ch]        = daq->getCurrHist(ch); // save current measurement
                ev->realTime[ch]        = daq->getRealTime(ch);
                ev->deadTime[ch]        = daq->getDeadTime(ch);
                ev->goodCounts[ch]      = daq->getGoodCounts(ch);
                ev->totCounts[ch]       = daq->getTotCounts(ch);
                ev->scaleFactor[ch]     = fScaleFactor[ch];
                ev->countsPerSecond[ch] = daq->getCountsPerSecond(ch);
                if (useIntegration)                             // detector response
                    ev->detectorResponse[ch]= this->CalcResponseV2(ch);
                else
                    ev->detectorResponse[ch]= this->CalcResponseV1(ch);
            }
            ev->acqTime = daq->GetAcquisitionLoopTime();
            NormalizeFuzzyInputs();
            ev->arrowAngle = UpdateArrow();      // Updates the arrow tab, calculates the arrow angle
            gSystem->ProcessEvents();
            cCurrHCanvas->Modified();
            storage->getTree()->Fill();
            UpdateHistory();                     // Updates the history & average tabs
        }
        fHCurrHProgressBar->SetPosition(1);
    }
    storage->getTree()->Write();                 // This may be excessive, but we have SSD for storage :)
    fStartDAQ->SetState(kButtonUp);
    fStopDAQ->SetState(kButtonDisabled);
    fUseIntegration->SetState(kButtonUp);
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

// Channel gain settings - one may write this more neatly has we have more time...
void PigsGUI::SetGainScalerCh0() {
    // Changes the scaler gain for channel 0
    if(fVerbose) std::cout << __PRETTY_FUNCTION__ << " -- gain: " <<
            fScalerInput[0]->GetEntry()->GetNumber() << std::endl;
    fScaleFactor[0] = fScalerInput[0]->GetEntry()->GetNumber();
}

void PigsGUI::SetGainScalerCh1() {
    // Changes the scaler gain for channel 1
    if(fVerbose) std::cout << __PRETTY_FUNCTION__ << " -- gain: " <<
            fScalerInput[1]->GetEntry()->GetNumber() << std::endl;
    fScaleFactor[1] = fScalerInput[1]->GetEntry()->GetNumber();
}

void PigsGUI::SetGainScalerCh2() {
    // Changes the scaler gain for channel 2
    if(fVerbose) std::cout << __PRETTY_FUNCTION__ << " -- gain: " <<
            fScalerInput[2]->GetEntry()->GetNumber() << std::endl;
    fScaleFactor[2] = fScalerInput[2]->GetEntry()->GetNumber();
}

void PigsGUI::SetGainScalerCh3() {
    // Changes the scaler gain for channel 3
    if(fVerbose) std::cout << __PRETTY_FUNCTION__ << " -- gain: " <<
            fScalerInput[3]->GetEntry()->GetNumber() << std::endl;
    fScaleFactor[3] = fScalerInput[3]->GetEntry()->GetNumber();
}

void PigsGUI::SetIntegralLimitMin() {
    // Changes lower ACD limit for energy integration
    if(fVerbose) std::cout << __PRETTY_FUNCTION__ << " -- lower ACD limit: " <<
            fIntLimInputMin->GetEntry()->GetNumber() << std::endl;
    fIntegralMin = fIntLimInputMin->GetEntry()->GetNumber();
}

void PigsGUI::SetIntegralLimitMax() {
    // Changes upper ACD limit for energy integration
    if(fVerbose) std::cout << __PRETTY_FUNCTION__ << " -- upper ACD limit: " <<
            fIntLimInputMax->GetEntry()->GetNumber() << std::endl;
    fIntegralMax = fIntLimInputMax->GetEntry()->GetNumber();
}

Float_t PigsGUI::CalcResponseV1(int32_t ch) {
    // Simple detector response using just number of recored counts
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    return fScaleFactor[ch] * daq->getGoodCounts(ch);
}

Float_t PigsGUI::CalcResponseV2(int32_t ch) {
    // Detector response which integrates captured energy
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int32_t ibin;
    Float_t energyIntegral = 0;
    for (ibin = fIntegralMin; ibin <= fIntegralMax; ibin++) {
        energyIntegral += ev->spectrum[ch]->GetBinContent(ibin)*(Float_t)ibin;
    }
    return  fScaleFactor[ch] * energyIntegral;
}

void PigsGUI::ToggleUseIntegration() {
    // Switch energy integration method
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    useIntegration = fUseIntegration->IsOn();
    fIntLimInputMin->GetEntry()->SetEnabled(useIntegration);
    fIntLimInputMax->GetEntry()->SetEnabled(useIntegration);
}

void PigsGUI::UpdateHistory() {
    // Updates the history tab
    int32_t ch;
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    Long64_t totalEntries = storage->getTree()->GetEntries();

    for(ch=0; ch<4; ch++) {                 // Clean the multigraph - so we refresh the view
        fMG->RecursiveRemove(fGraph[ch]);
    }
    for(ch=0; ch<4; ch++) {
        fGraph[ch]->SetPoint(fGraph[ch]->GetN(),daq->getTimeStamp().GetSec(), ev->detectorResponse[ch]);
        fMG->Add(fGraph[ch]);
    }
    if(fGraph[0]->GetN() >= 2) {            // After we have the first two data points
        cLastMeas->cd();
        fMG->Draw("AQ");
        fMG->GetXaxis()->SetTimeDisplay(1);
        fMG->GetXaxis()->SetTimeOffset(1);
        fMG->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M:%S}");
        fMG->GetXaxis()->SetLabelSize(0.02);
        fMG->GetXaxis()->SetLabelOffset(0.03);
        fMG->GetYaxis()->SetLabelSize(0.025);
        fMG->GetYaxis()->SetTitleOffset(1.25);
        fMG->GetYaxis()->SetTitle("Detector response");
        fMG->Draw("APC");
        cLastMeas->Update();
        cLastMeas->Modified();
    }

    // Update the average tab
    Long64_t currentEntry = -1;
    for(ch=0; ch<4; ch++) {               // averages
        if(fNormAvgH[ch]) fNormAvgH[ch]->Delete();
        fNormAvgH[ch] = (TH1F*) daq->getCurrHist(ch)->Clone();
    }
    if(fVerbose>1) cout << __PRETTY_FUNCTION__ << "totalEntries: " << totalEntries << endl;
    for (int32_t i=0; i<10; i++) {       // Loop over last 10 measurements
        currentEntry = totalEntries - i;
        if(currentEntry >=0 ) {
            if(fVerbose>2) cout << __PRETTY_FUNCTION__ << "i: " << i << " entry: " << currentEntry << endl;
            storage->getTree()->GetEntry(currentEntry);
            for(ch=0;ch<4;ch++) if(i>0) fNormAvgH[ch]->Add(storage->getE()->spectrum[ch]);
        }
    }
    for(ch=0; ch<4; ch++) {               // plot the normalized average
        fNormAvgH[ch]->Scale(1.0/fNormAvgH[ch]->Integral()); // Histogram normalization
        cSumSpectra->GetPad(ch+1)->cd();
        fNormAvgH[ch]->Draw();
        cSumSpectra->GetPad(ch+1)->Update();
    }
    cSumSpectra->Modified();
}

int32_t PigsGUI::HardStopAcquisition() {
    // Stops acquisition on all channels
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int32_t ret = 0;
    if (daq) ret = daq->StopAcquisition(-1); // -1 = all channels
    return ret;
}

int32_t PigsGUI::StopAcquisition() {
    // Stops the acquisition loop
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int32_t ret = 0;
    fStopDAQ->SetState(kButtonDown);
    if (daq && keepAcquiring) keepAcquiring = kFALSE;
    return ret;
}

void PigsGUI::SetProgressBarPosition(Float_t fposition) {
    // Set position of the progress bar
    fHCurrHProgressBar->SetPosition(fposition);
    gClient->NeedRedraw(fHCurrHProgressBar);
}


float PigsGUI::UpdateArrow() {
    // Update the arrow tab
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

    int32_t i;                          // Helper variable
    float fuzz_angle;                   // Arrow angle to be calculated
    float ox, oy;                       // Plotting variables for coordinates
    float comp_x1, comp_y1;
    float comp_x2, comp_y2;

    double **tmpNormalized;             // Ugly hack to communicate with fismain
    tmpNormalized = new double*[4];     // We have double[4] and need double**
    for(i=0; i<4; i++) {
        tmpNormalized[i] = new double[1];
        tmpNormalized[i][0] = Normalized[i];
    }

    // Get the arrow angle, calls the routine in fismain
    fuzz_angle =  get_fuzzy(tmpNormalized, 4);

    cArrowCanvas->cd();
    // Define where origin is
    ox = 0.5;
    oy = 0.5;
    // Create fake fuzzy output between 0-16
        //      fake_fuzzy = 16.0*((float) rand()) / (float) RAND_MAX;
        //    fuzz_angle =  fake_fuzzy*22.5;
        //    if(fVerbose) std::cout << fake_fuzzy << "////";
    // Create x,y around circle
    if(fVerbose) std::cout << "** Arrow Angle = " << fuzz_angle << std::endl;
    comp_x2 = 0.5 + 0.2*cos(fuzz_angle*M_PI/180.0);
    comp_y2 = 0.5 + 0.2*sin(fuzz_angle*M_PI/180.0);
    comp_x1 = -comp_x2 + 2.0*ox;
    comp_y1 = -comp_y2 + 2.0*oy;
    if(fVerbose) std::cout << comp_x1 <<"////"<< comp_x2 <<"////"<< comp_y1 <<"////"<< comp_y2 << std::endl;
    ar1->SetX1(comp_x1);
    ar1->SetY1(comp_y1);
    ar1->SetX2(comp_x2);
    ar1->SetY2(comp_y2);

    cArrowCanvas->Modified();
    cArrowCanvas->Update();
    
    return fuzz_angle;
}

PigsGUI::PigsGUI(const TGWindow *p) : TGMainFrame(p, fGUIsizeX, fGUIsizeY)  {
    // Creates the GUI
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    daq = 0; storage = 0; ev = 0;               // Initialize local variables
    year = month = day = hour = min = sec = 0;
    fAcqThread = 0;
    keepAcquiring  = kFALSE;
    useIntegration = kTRUE;
    const int32_t fHistColors[] = { kMagenta+1, kGreen+1, kBlue+1, kRed+1 };
    fAboutMsg = (char*)
"\n"
"\n"
"        _____  _____  ______ _______\n"
"       |_____]   |   |  ____ |______\n"
"       |       __|__ |_____| ______|\n"
"\n"
"\n"
" *** Position Indicating Gamma Sensor   ***\n"
"  * CAEN DT-5781 Data Acquisition System *\n"
"         Four Channel Version\n"
"\n"
"   by Ondrej Chvala <ochvala@utk.edu>\n"
"        version 0.097, July 2015\n"
"   https://github.com/ondrejch/DAQ-DT5781\n"
"                 GNU/GPL";
    int32_t i = 0; // helper variable
    for (i=0; i<4; i++) {
        fScaleFactor[i] = 1.0;
        fNormAvgH[i] = 0;
    }
    fIntegralMin = 1;
    fIntegralMax = 16384;

    // *** Main GUI window ***
    fMainGUIFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
    #include "fpigsicon.xpm"
    TImage *tmpicon = TImage::Create();
    tmpicon->SetImageBuffer((char**)fpigsicon_xpm, TImage::kXpm);
    gVirtualX->SetIconPixmap(fMainGUIFrame->GetId(),tmpicon->GetPixmap());
    delete tmpicon;
    fMainGUIFrame->SetName("fMainGUIFrame");
    fMainGUIFrame->SetWindowName("F-PIGS");      // GUI window name
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
    fMainTitle = new TGLabel(fMainGUIFrame,"Four-channel Position Identifying Gamma Sensor (F-PIGS)",
            uGC->GetGC(),ufont->GetFontStruct());
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
    fStartDAQ->Connect("Clicked()","PigsGUI",this,"RunAcquisition()");

    fStopDAQ = new TGTextButton(fMainGUIFrame, "Stop DAQ");        // stop DAQ
    fStopDAQ->SetTextJustify(36);
    fStopDAQ->SetMargins(0,0,0,0);
    fStopDAQ->Resize(90,25);
    fMainGUIFrame->AddFrame(fStopDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fStopDAQ->MoveResize(fGUIsizeX-50-90,fGUIsizeY-30,90,25);
    gClient->GetColorByName("red", fColor);
    fStopDAQ->ChangeBackground(fColor);
    fStopDAQ->SetState(kButtonDisabled);
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

    // *** Container of "CurrentHistogram" ***
    fCurHistFrame = fTabHolder->AddTab("CurrentHistogram");
    fCurHistFrame->SetLayoutManager(new TGVerticalLayout(fCurHistFrame));
    // embedded canvas
    fLatestHistoCanvas = new TRootEmbeddedCanvas("CurrentHEC",fCurHistFrame,fGUIsizeX-10,fGUIsizeY-140);
    Int_t wfLatestHistoCanvas = fLatestHistoCanvas->GetCanvasWindowId();
    cCurrHCanvas = new TCanvas("cCurrHCanvas", 10, 10, wfLatestHistoCanvas);
    fLatestHistoCanvas->AdoptCanvas(cCurrHCanvas);
    cCurrHCanvas->Divide(2,2);
    fCurHistFrame->AddFrame(fLatestHistoCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fHCurrHProgressBar = new TGHProgressBar(fCurHistFrame,fGUIsizeX-5);
    fHCurrHProgressBar->SetFillType(TGProgressBar::kBlockFill);
    fHCurrHProgressBar->ChangeOptions(kSunkenFrame | kDoubleBorder | kOwnBackground);

    // will reflect user color changes
    gClient->GetColorByName("#ffffff",fColor);
    fHCurrHProgressBar->SetBackgroundColor(fColor);
    fHCurrHProgressBar->SetPosition(1);
    fCurHistFrame->AddFrame(fHCurrHProgressBar, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** Container of "History" ***
    fTabHisto = fTabHolder->AddTab("History");
    fTabHisto->SetLayoutManager(new TGVerticalLayout(fTabHisto));
    // embedded canvas
    fLastMeas = new TRootEmbeddedCanvas("HistoryHEC",fTabHisto,fGUIsizeX-10,fGUIsizeY-110);
    Int_t wfLastNspectra = fLastMeas->GetCanvasWindowId();
    cLastMeas = new TCanvas("cLastMeas", 10, 10, wfLastNspectra);
    fLastMeas->AdoptCanvas(cLastMeas);
    fMG = new TMultiGraph("fMG","");
    for (i=0; i<4; i++) {
        fGraph[i] = new TGraph();
        fGraph[i]->SetName(Form("gCh%d",i));
        fGraph[i]->SetDrawOption("AP");
        fGraph[i]->SetMarkerColor(fHistColors[i]);
        fGraph[i]->SetMarkerStyle(21);
        fGraph[i]->SetMarkerSize(2.0);
        fGraph[i]->SetLineWidth(0.5);
        fGraph[i]->SetLineColor(i+12);
        fGraph[i]->SetFillStyle(0);
        fMG->Add(fGraph[i]);
    }
    fTabHisto->AddFrame(fLastMeas, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** Container of "Sum" ***
    fTabSum = fTabHolder->AddTab("Sum");
    fTabSum->SetLayoutManager(new TGVerticalLayout(fTabSum));
    // embedded canvas
    fSumSpectra = new TRootEmbeddedCanvas("SumHEC",fTabSum,fGUIsizeX-10,fGUIsizeY-110);
    Int_t wfSumSpectra = fSumSpectra->GetCanvasWindowId();
    cSumSpectra = new TCanvas("cSumSpectra", 10, 10, wfSumSpectra);
    fSumSpectra->AdoptCanvas(cSumSpectra);
    cSumSpectra->Divide(2,2);
    for (i=1; i<5; i++) {
        cSumSpectra->GetPad(i)->SetLogx();
        cSumSpectra->GetPad(i)->SetLogy();
    }
    fTabSum->AddFrame(fSumSpectra, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** Container of "Arrow" ***
    fTabArrow = fTabHolder->AddTab("Arrow");
    fTabArrow->SetLayoutManager(new TGVerticalLayout(fTabArrow));
    // embedded canvas
    fArrowECanvas = new TRootEmbeddedCanvas("ArrowHEC",fTabArrow,fGUIsizeX-10,fGUIsizeY-110);
    Int_t wfArrowECanvas = fArrowECanvas->GetCanvasWindowId();
    cArrowCanvas = new TCanvas("cArrowCanvas", 10, 10, wfArrowECanvas);
    fArrowECanvas->AdoptCanvas(cArrowCanvas);
    fTabArrow->AddFrame(fSumSpectra, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    // *** Container of "Config" ***
    fTabConfig = fTabHolder->AddTab("Config");
    fTabConfig->SetLayoutManager(new TGVerticalLayout(fTabConfig));
    // Acquisition time settings
    fControlFrame = new TGGroupFrame(fTabConfig, "Acquisition time [sec]");
    fControlFrame->SetTitlePos(TGGroupFrame::kCenter);
    fAcqTimeEntry = new TGNumberEntry(fControlFrame, (Double_t) 10.0 ,5,-1, TGNumberFormat::kNESRealOne,
            TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMinMax, 0.1, 600);
    fAcqTimeEntry->GetNumberEntry()->SetToolTipText("Time for one DAQ loop in seconds.");
    fAcqTimeEntry->GetNumberEntry()->Connect("TextChanged(char*)", "PigsGUI", this,
            "SetAcquisitionLoopTime()");
    fAcqTimeEntry->GetNumberEntry()->Connect("ReturnPressed()", "PigsGUI", this,
            "SetAcquisitionLoopTimeNumberEntry()");
    
    fAcqTimeFrame->AddFrame(fAcqTimeEntry, new TGLayoutHints(kLHintsNormal, 5, 5, 5, 5));
    //fControlFrame->AddFrame(fAcqTimeEntry, new TGLayoutHints(kLHintsNormal, 5, 5, 5, 5));
    
    // Disable acquisition time adjustment until DAQ initialized
    fAcqTimeEntry->SetState(kFALSE);
    fAcqTimeSlider->SetState(kFALSE);
/*
    fAcqTimeLabelFrame = new TGCompositeFrame(fTabConfig, 100, 10, kHorizontalFrame); 
    fAcqTimeLabelText = new TGLabel(fAcqTimeLabelFrame,"Acquire time:",
            uGC->GetGC(),ufont->GetFontStruct());
    fAcqTimeLabelText->SetTextJustify(kTextLeft);
    fAcqTimeLabelText->SetWrapLength(-1);
    fAcqTimeLabel = new TGLabel(fAcqTimeLabelFrame,"    ",
            uGC->GetGC(),ufont->GetFontStruct());
    fAcqTimeLabel->SetTextColor(0x0066ff);
    fAcqTimeLabel->SetTextJustify(kTextCenterX);
    fAcqTimeLabel->SetWrapLength(-1);
    fAcqTimeLabel->SetMinWidth(3);
    fAcqTimeLabelFrame->AddFrame(fAcqTimeLabelText, new TGLayoutHints(kLHintsLeft, 10, 5, 10, 10));
    fAcqTimeLabelFrame->AddFrame(fAcqTimeLabel, new TGLayoutHints(kLHintsRight, 0, 10, 10, 10 ));
  */ 
    fControlFrame->AddFrame(fAcqTimeFrame, new TGLayoutHints(kLHintsTop, 10, 10, 5, 5));
   // fControlFrame->AddFrame(fAcqTimeLabelFrame, new TGLayoutHints(kLHintsBottom, 10, 10, 5, 5));
    fTabConfig->AddFrame(fControlFrame, new TGLayoutHints(kLHintsNormal, 10, 10, 10, 10));    

    // Scale Factor setting
    fScalerFrame = new TGGroupFrame(fTabConfig, "Channel gain compensation");
    fScalerFrame->SetTitlePos(TGGroupFrame::kCenter);
    for (i=0; i<4; i++){
        fScalerInput[i] = new PigsScalerInput(fScalerFrame, Form("ch %d scaling", i));
        fScalerInput[i]->GetEntry()->Connect("TextChanged(char*)", "PigsGUI", this,
                Form("SetGainScalerCh%d()",i));
        fScalerInput[i]->GetEntry()->SetToolTipText(
                "Channel gain is a multiplicative factor used in detector response calculation.");
        fScalerFrame->AddFrame(fScalerInput[i], new TGLayoutHints(kLHintsNormal, 0, 0, 2, 2));
    }
    fTabConfig->AddFrame(fScalerFrame, new TGLayoutHints(kLHintsNormal, 10, 10, 10, 10));
    // Integration Limits
    fIntLimFrame = new TGGroupFrame(fTabConfig, "ADC window for integration");
    fIntLimFrame->SetTitlePos(TGGroupFrame::kCenter);
    fUseIntegration = new TGCheckButton(fIntLimFrame, "Energy integration On/Off");
    fUseIntegration->SetOn(kFALSE);         // Start with regular counts
    fUseIntegration->SetToolTipText("If enabled, the detector response is calculated by integrating "
            "the energy deposited in ADC bins within the limits specidied below.\n"
            "If disabled, the hit count is used as a detector response.");
    fUseIntegration->Connect("Toggled(Bool_t)", "PigsGUI", this, "ToggleUseIntegration()");
    fIntLimFrame->AddFrame(fUseIntegration, new TGLayoutHints(kLHintsNormal, 0, 0, 2, 2));
    fIntLimInputMin = new PigsIntLimInput(fIntLimFrame, "Lower limit");
    fIntLimInputMin->GetEntry()->SetIntNumber(fIntegralMin);
    fIntLimInputMin->GetEntry()->Connect("TextChanged(char*)", "PigsGUI", this, "SetIntegralLimitMin()");
    fIntLimFrame->AddFrame(fIntLimInputMin, new TGLayoutHints(kLHintsNormal, 0, 0, 2, 2));
    fIntLimInputMax = new PigsIntLimInput(fIntLimFrame, "Upper limit");
    fIntLimInputMax->GetEntry()->SetIntNumber(fIntegralMax);
    fIntLimInputMax->GetEntry()->Connect("TextChanged(char*)", "PigsGUI", this, "SetIntegralLimitMax()");
    fIntLimFrame->AddFrame(fIntLimInputMax, new TGLayoutHints(kLHintsNormal, 0, 0, 2, 2));
    fTabConfig->AddFrame(fIntLimFrame, new TGLayoutHints(kLHintsNormal, 10, 10, 10, 10));
    this->ToggleUseIntegration();           // Updates the integration entries' status

    // *** Container of "DT5781" ***
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

    // *** Container of "About" ***
    fTabAbout = fTabHolder->AddTab("About");
    fTabAbout->SetLayoutManager(new TGVerticalLayout(fTabAbout));
    ufont = gClient->GetFont("-*-fixed-medium-r-*-*-15-*-*-*-*-*-*-*");
    fAboutText = new TGTextView(fTabAbout,1,1,"SPIGS",kSunkenFrame);
    fAboutText->SetFont(ufont->GetFontStruct());
    fTabAbout->AddFrame(fAboutText, new TGLayoutHints(kLHintsNormal));
    fAboutText->LoadBuffer(fAboutMsg);

    //-------------------------------------------------------------------------

    // Change to the starting tab
    fTabHolder->SetTab("DT5781");

    // Display the GUI
    fTabHolder->Resize(fTabHolder->GetDefaultSize());
    fMainGUIFrame->AddFrame(fTabHolder, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fTabHolder->MoveResize(0,32,fGUIsizeX-2,fGUIsizeY-80);

    fMainGUIFrame->SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
    fMainGUIFrame->MapSubwindows();
    fMainGUIFrame->Resize(fMainGUIFrame->GetDefaultSize());
    fMainGUIFrame->MapWindow();
    fMainGUIFrame->Resize(fGUIsizeX,fGUIsizeY);

    static const int32_t tmpw = 410;        // Constants for About window placement
    static const int32_t tmph = 260;
    fAboutText->MoveResize((fGUIsizeX-tmpw)/2,(fGUIsizeY-tmph)/3,tmpw,tmph);
}

void PigsGUI::NormalizeFuzzyInputs() {
    // Normalize counts for fuzzy input
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    int i, min, max;
    if(fVerbose) std::cout << "[";
    for(i=0; i<4; i++){
        RawFuzzArray[i] = ev->goodCounts[i];
        if(fVerbose) std::cout << RawFuzzArray[i] << ",";
    }    
    min = *std::min_element(RawFuzzArray, RawFuzzArray+4);
    max = *std::max_element(RawFuzzArray, RawFuzzArray+4);
    if(fVerbose) {
        std::cout << "]""\n";
        std::cout << "The smallest element is " << *std::min_element(RawFuzzArray,RawFuzzArray+4) << std::endl;
        std::cout << "The largest  element is " << *std::max_element(RawFuzzArray,RawFuzzArray+4) << std::endl;
    }
    for(i=0; i<4; i++){
        Normalized[i] = 100.0*(RawFuzzArray[i]-min)/(max-min);
        if(fVerbose) std::cout << Normalized[i] << ",";
    }        
    if(fVerbose) std::cout << std::endl;
}                        

PigsGUI::~PigsGUI() {
    if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
    HardStopAcquisition();
    DisconnectDAQ();
    if(storage) delete storage;     // Save data
    Cleanup();                      // Clean up all widgets, frames and layout hints that were used
    gApplication->Terminate(0);
}

