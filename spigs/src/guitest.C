/*
 * guitest.C
 *
 *  Created on: Jun 19, 2015
 *      Author: o
 */

// Mainframe macro generated from application: /usr/bin/root.exe
// By ROOT version 5.34/19 on 2015-06-12 21:22:34

#include "TGDockableFrame.h"
#include "TGMenu.h"
#include "TGMdiDecorFrame.h"
#include "TG3DLine.h"
#include "TGMdiFrame.h"
#include "TGMdiMainFrame.h"
#include "TGMdiMenu.h"
#include "TGColorDialog.h"
#include "TGColorSelect.h"
#include "TGListBox.h"
#include "TGNumberEntry.h"
#include "TGScrollBar.h"
#include "TGComboBox.h"
#include "TGFrame.h"
#include "TGFileDialog.h"
#include "TGShutter.h"
#include "TGButtonGroup.h"
#include "TGCanvas.h"
#include "TGFSContainer.h"
#include "TGFontDialog.h"
#include "TGButton.h"
#include "TGFSComboBox.h"
#include "TGLabel.h"
#include "TGProgressBar.h"
#include "TGMsgBox.h"
#include "TGTab.h"
#include "TGListView.h"
#include "TGSplitter.h"
#include "TGStatusBar.h"
#include "TGListTree.h"
#include "TGToolTip.h"
#include "TGToolBar.h"
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TGInputDialog.h"

#include "Riostream.h"

void guitest()
{

   // main frame
   TGMainFrame *fMainGUIFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMainGUIFrame->SetName("fMainGUIFrame");
   fMainGUIFrame->SetLayoutBroken(kTRUE);

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-*-luxi_sans-bold-r-*-*-0-*-*-*-*-*-*-*");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t valTitle;
   valTitle.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000066",valTitle.fForeground);
   gClient->GetColorByName("#e0e0e0",valTitle.fBackground);
   valTitle.fFillStyle = kFillSolid;
   valTitle.fFont = ufont->GetFontHandle();
   valTitle.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valTitle, kTRUE);
   TGLabel *fMainTitle = new TGLabel(fMainGUIFrame,"Single-channel Position Identifying Gamma Sensor",uGC->GetGC(),ufont->GetFontStruct());
   fMainTitle->SetTextJustify(36);
   fMainTitle->SetMargins(0,0,0,0);
   fMainTitle->SetWrapLength(-1);
   fMainGUIFrame->AddFrame(fMainTitle, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fMainTitle->MoveResize(0,0,488,32);

   // tab widget
   TGTab *fTabHolder = new TGTab(fMainGUIFrame,488,336);

   // container of "CurrentHistogram"
   TGCompositeFrame *fCurHistFrame;
   fCurHistFrame = fTabHolder->AddTab("CurrentHistogram");
   fCurHistFrame->SetLayoutManager(new TGVerticalLayout(fCurHistFrame));

   // embedded canvas
   TRootEmbeddedCanvas *fLatestHistoCanvas = new TRootEmbeddedCanvas(0,fCurHistFrame,478,280);
   Int_t wfLatestHistoCanvas = fLatestHistoCanvas->GetCanvasWindowId();
   TCanvas *fCurrHCanvas = new TCanvas("fCurrHCanvas", 10, 10, wfLatestHistoCanvas);
   fCurrHCanvas->SetEditable(kFALSE); 		//	to remove editing
   fLatestHistoCanvas->AdoptCanvas(fCurrHCanvas);
   fCurHistFrame->AddFrame(fLatestHistoCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   TGHProgressBar *fHCurrHProgressBar = new TGHProgressBar(fCurHistFrame,472);
   fHCurrHProgressBar->SetFillType(TGProgressBar::kBlockFill);
   fHCurrHProgressBar->ChangeOptions(kSunkenFrame | kDoubleBorder | kOwnBackground);

   ULong_t ucolor;        // will reflect user color changes
   gClient->GetColorByName("#ffffff",ucolor);
   fHCurrHProgressBar->SetBackgroundColor(ucolor);
   fHCurrHProgressBar->SetPosition(25);
   fCurHistFrame->AddFrame(fHCurrHProgressBar, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));


   // container of "History"
   TGCompositeFrame *fTabHisto;
   fTabHisto = fTabHolder->AddTab("History");
   fTabHisto->SetLayoutManager(new TGVerticalLayout(fTabHisto));

   // embedded canvas
   TRootEmbeddedCanvas *fLastNspectra = new TRootEmbeddedCanvas(0,fTabHisto,478,309);
   Int_t wfLastNspectra = fLastNspectra->GetCanvasWindowId();
   TCanvas *c125 = new TCanvas("c125", 10, 10, wfLastNspectra);
   fLastNspectra->AdoptCanvas(c125);
   fTabHisto->AddFrame(fLastNspectra, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));


   // container of "Config"
   TGCompositeFrame *fTabConfig;
   fTabConfig = fTabHolder->AddTab("Config");
   fTabConfig->SetLayoutManager(new TGVerticalLayout(fTabConfig));

   // container of "DT5781"
   TGCompositeFrame *fTABDT5781;
   fTABDT5781 = fTabHolder->AddTab("DT5781");
   fTABDT5781->SetLayoutManager(new TGVerticalLayout(fTABDT5781));


   // container of "About"
   TGCompositeFrame *fTabAbout;
   fTabAbout = fTabHolder->AddTab("About");
   fTabAbout->SetLayoutManager(new TGVerticalLayout(fTabAbout));


   fTabHolder->SetTab(4);

   fTabHolder->Resize(fTabHolder->GetDefaultSize());
   fMainGUIFrame->AddFrame(fTabHolder, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTabHolder->MoveResize(0,32,488,336);

   fMainGUIFrame->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   fMainGUIFrame->MapSubwindows();

   fMainGUIFrame->Resize(fMainGUIFrame->GetDefaultSize());
   fMainGUIFrame->MapWindow();
   fMainGUIFrame->Resize(490,372);
}


