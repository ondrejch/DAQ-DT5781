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
	daq = PigsDAQ::getInstance();
	if (!ret) ret = daq->InitDPPLib();
	if (!ret) ret = daq->AddBoardUSB();
	if (!ret) ret = daq->BasicInit();
	if (!ret) ret = daq->ConfigureBoard();
	if (!ret) ret = daq->ConfigureChannel(0);
	if (!ret) {
		daq->PrintBoardInfo();
		daq->PrintChannelParameters(0);
		fStartDAQ->SetState(kButtonUp);
	}
	daq->setGUI(this);			// set GUI pointer
	return ret;
}

int PigsGUI::ExitDAQ() {
	// Ends connection to the DPP library
	int ret = daq->EndLibrary();
	return ret;
}

int PigsGUI::StartAcqisition() {
	// TODO
	fStartDAQ->SetState(kButtonDown);

	int ret=0;
	return ret;
}

int PigsGUI::StopAcqisition() {
	// Stops acquisition on channel 0
	int ret = daq->StopAcquisition(0);
	return ret;
}

void PigsGUI::SetProgressBarPosition(Float_t fposition) {
	// set position of the progress bar
	fHCurrHProgressBar->SetPosition(fposition);
}

PigsGUI::PigsGUI() {
	daq = 0;

	// Main GUI window
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

	// tab widget
	// GCValues_t valTab;
	// gClient->GetColorByName("#000000",valTab.fForeground);
	// gClient->GetColorByName("#e0e0e0",valTab.fBackground);
	// ufont = gClient->GetFont("-urw-nimbus sans l-regular-r-normal--0-0-0-0-p-0-iso8859-1");
	// valTab.fFillStyle = kFillSolid;
	// valTab.fFont = ufont->GetFontHandle();
	// valTab.fGraphicsExposures = kFALSE;
	// uGC = gClient->GetGC(&valTab, kTRUE);
	fTabHolder = new TGTab(fMainGUIFrame,fGUIsizeX-4,fGUIsizeX-4);//,uGC->GetGC());

	// container of "CurrentHistogram"
	fCurHistFrame = fTabHolder->AddTab("CurrentHistogram");
	fCurHistFrame->SetLayoutManager(new TGVerticalLayout(fCurHistFrame));

	// embedded canvas
	fLatestHistoCanvas = new TRootEmbeddedCanvas(0,fCurHistFrame,fGUIsizeX-10,fGUIsizeY-140);
	Int_t wfLatestHistoCanvas = fLatestHistoCanvas->GetCanvasWindowId();
	cCurrHCanvas = new TCanvas("cCurrHCanvas", 10, 10, wfLatestHistoCanvas);
	cCurrHCanvas->SetEditable(kFALSE); 		//	to remove editing
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

	// container of "History"
	fTabHisto = fTabHolder->AddTab("History");
	fTabHisto->SetLayoutManager(new TGVerticalLayout(fTabHisto));
	// embedded canvas
	fLastNspectra = new TRootEmbeddedCanvas(0,fTabHisto,fGUIsizeX-10,fGUIsizeY-110);
	Int_t wfLastNspectra = fLastNspectra->GetCanvasWindowId();
	cLastNspectra = new TCanvas("cLastNspectra", 10, 10, wfLastNspectra);
	fLastNspectra->AdoptCanvas(cLastNspectra);
	fTabHisto->AddFrame(fLastNspectra, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

	// container of "Config"
	fTabConfig = fTabHolder->AddTab("Config");
	fTabConfig->SetLayoutManager(new TGVerticalLayout(fTabConfig));

	// container of "DT5781"
	fTABDT5781 = fTabHolder->AddTab("DT5781");
	fTABDT5781->SetLayoutManager(new TGVerticalLayout(fTABDT5781));

	// container of "About"
	fTabAbout = fTabHolder->AddTab("About");
	fTabAbout->SetLayoutManager(new TGVerticalLayout(fTabAbout));

	// Buttons
	fStartDAQ = new TGTextButton(fMainGUIFrame, "Start DAQ"); 	// start DAQ
	fStartDAQ->SetTextJustify(36);
	fStartDAQ->SetMargins(0,0,0,0);
	fStartDAQ->Resize(90,25);
	fMainGUIFrame->AddFrame(fStartDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	fStartDAQ->MoveResize(50,fGUIsizeY-30,90,25);
	gClient->GetColorByName("green", fColor);
	fStartDAQ->ChangeBackground(fColor);
	fStartDAQ->SetState(kButtonDisabled);

	fStopDAQ = new TGTextButton(fMainGUIFrame, "Stop DAQ");		// stop DAQ
	fStopDAQ->SetTextJustify(36);
	fStopDAQ->SetMargins(0,0,0,0);
	fStopDAQ->Resize(90,25);
	fMainGUIFrame->AddFrame(fStopDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	fStopDAQ->MoveResize(fGUIsizeX-50-90,fGUIsizeY-30,90,25);
	gClient->GetColorByName("red", fColor);
	fStopDAQ->ChangeBackground(fColor);
	fStopDAQ->SetState(kButtonDisabled);

	fExitDAQ = new TGTextButton(fMainGUIFrame, "Exit DAQ");		// exit DAQ
	fExitDAQ->SetTextJustify(36);
	fExitDAQ->SetMargins(0,0,0,0);
	fExitDAQ->Resize(90,25);
	fMainGUIFrame->AddFrame(fExitDAQ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	fExitDAQ->MoveResize(fGUIsizeX/2-45,fGUIsizeY-30,90,25);

	// display GUI
	fTabHolder->SetTab("CurrentHistogram");
	fTabHolder->Resize(fTabHolder->GetDefaultSize());
	fMainGUIFrame->AddFrame(fTabHolder, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	fTabHolder->MoveResize(0,32,fGUIsizeX-2,fGUIsizeY-80);

	fMainGUIFrame->SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
	fMainGUIFrame->MapSubwindows();

	fMainGUIFrame->Resize(fMainGUIFrame->GetDefaultSize());
	fMainGUIFrame->MapWindow();
	fMainGUIFrame->Resize(fGUIsizeX,fGUIsizeY);

}

PigsGUI::~PigsGUI() {
	// TODO KILL DAQ first!
	StopAcqisition();
	ExitDAQ();
	// Clean up all widgets, frames and layout hints that were used
	Cleanup();
}
