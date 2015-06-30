/*
 * Position Indicating Gamma Sensing system
 * Currently assumes single channel operation on channel 0
 *
 * PigsDAQ.h - Main/Helper singleton class
 *
 *
 *  Created on: Jun 12, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#ifndef PIGSDAQ_H_
#define PIGSDAQ_H_

#include <stdio.h>          // printf, scanf, NULL */
#include <stdlib.h>         // calloc, exit, free
#include <iostream>         // std::cerr
#include <typeinfo>         // operator typeid
#include <exception>        // std::exception
#include <string.h>         // strcpy

// CAEN
#include <CAENDPPLib.h>

// ROOT
#include <TROOT.h>
#include <TH1D.h>
#include <TTimeStamp.h>
#include <TString.h>
#include <TThread.h>

// PIGS
#include <PigsGUI.h>

// The maximum number of board's channels.
// NOTE: MAX_NUMCHB is defined in 'CAENDPPLibTypes.h'
#define MAX_BOARD_CHNUM MAX_NUMCHB

// The maximum number of board's channels.
// NOTE: MAX_NUMB is defined in 'CAENDPPLibTypes.h'
#define MAX_BOARD_NUMBER MAX_NUMB

// The maximum number of bits for the histograms
// 14 bits ->from bin 0 to bin 16383 (2^14-1)
#define MAX_HISTOBITS (14)

// The maximum number of bins of the histograms
// (depending on their maximum number of bits)
#define MAX_HISTO_NBINS (0x1 << (MAX_HISTOBITS-0))

// Other defines
#define MAX_STATUSMSG_LEN   (200)   // Maximum length in chars of the status string
#define MAX_ERRMSG_LEN      (100)   // Maximum length in chars of the error message string
#define MAX_HVNAME_LEN      (29)    // Maximum Length of Active HV Channel Name
#define MAX_TRACENAME_LEN   (50)    // Maximum Length of Traces Name
#define MAX_HISTOFNAME_LEN  (100)   // Maximum Length of Histograms File Name

class PigsGUI;

class PigsDAQ {
public:
    static PigsDAQ * getInstance(); // Get unique pointer to the class
    int32_t BasicInit();            // Does all basic initialization
    int32_t InitDgtzParams();       // Initialize the digitizer parameters to their default value
    int32_t SetDgtzParams();        // Set custom digitizer parameters
    int32_t SetDAQParams();         // Set custom DAQ parameters
    int32_t InitDPPLib();           // Initialize the DPP library
    int32_t AddBoardUSB();          // Adds board 0 on USB bus to the DPP library
    void PrintBoardInfo();          // Prints board information
    int32_t ConfigureBoard();       // Configures the board with digitizer and DAQ parameters
    int32_t EndLibrary();           // Cleanup: call this before the program ends!

    int32_t isChannelDisabled(int32_t ch);          // 0 if enabled, 1 if disabled
    void PrintChannelParameters(int32_t ch);        // Prints Channel Parameters
    char * decodeError(char *dest, int32_t code);   // Decodes the given error code into a message

    int32_t ConfigureChannel(int32_t ch);   // Sets channel parameters specified in Init & Set calls
    int32_t StopAcquisition(int32_t ch);    // Stops acquisition for channel ch
    int32_t AcquisitionSingleLoop();        // Runs acquisition
    int32_t ThreadAcqSingleLoop();          // Runs acquisition in a separate thread
    void PrintAcquisotionInfo();            // Prints real/dead time, cps, ...
    void SetAcquisitionLoopTime(float sec); // Set acquisition loop time in seconds

    int32_t RefreshCurrHist();              // Transfers h1 into currHist

    TH1D *getCurrHist() const;
    void setCurrHist(TH1D *currHist);
    double getCountsPerSecond() const;
    uint32_t getGoodCounts() const;
    uint64_t getRealTime() const;
    uint64_t getDeadTime() const;
    uint32_t getTotCounts() const;
    CAENDPP_StopCriteria_t getStopCriteria() const;
    uint64_t getStopCriteriaValue() const;
    void setStopCriteria(CAENDPP_StopCriteria_t stopCriteria);
    void setStopCriteriaValue(uint64_t stopCriteriaValue);
    PigsGUI *getGUI() const;
    void setGUI(PigsGUI *gui);
    const char *getBoardInfo() const;

protected:
    static PigsDAQ * instance;
    static bool instantiated;
    PigsDAQ();
    ~PigsDAQ(){};

private:
    int32_t ch;         // ch is the variable used to identify a board's channel inside DPPLibrary.
    int32_t handle;     // Handler to the Library
    int32_t brd;        // Board Identifier - this code assumes we only have one board
    CAENDPP_ConnectionParams_t connParam;   // Connection Parameters - Used to connect to the board.
    CAENDPP_Info_t info;                    // Board Infos Structure
    TString fBoardInfo;
    CAENDPP_DgtzParams_t dgtzParams;        // Acquisition Parameters - Used to set the acquisition properties.
    CAENDPP_AcqMode_t acqMode;              // Acquisition Mode (Histogram or Waveforms)
    CAENDPP_InputRange_t iputLevel;         // Channel input range
    CAENDPP_StopCriteria_t StopCriteria;    // Stop Criteria definition
    uint64_t StopCriteriaValue;             // Stop Criteria value
    CAENDPP_AcqStatus_t isAcquiring;        // 1 yes, 0 no
    uint32_t usecSleepPollDAQ;              // sleep in microseconds to poll DAQ acquisition status
    uint64_t realTime;
    uint64_t deadTime;
    uint32_t goodCounts;
    uint32_t totCounts;
    double countsPerSecond;
    uint32_t *h1;                           // histogram used for DPP dump
    int32_t  h1NBins;                       // number of bins in the histogram

    char codeStr[MAX_ERRMSG_LEN + 1];
    char histoFName[MAX_HISTOFNAME_LEN];

    static const int32_t fVerbose = 1;      // verbosity level settings
    int32_t fErrCode;       // error code from DPP calls
    TH1D *fCurrHist;        // Current histogram
    TTimeStamp fDt;         // Current date for histogram time
    TString fAcqDate;       // Acquisition date
    UInt_t year, month, day, hour, min, sec;
//    void *CleanAcqThread(void* arg);
    PigsGUI *gui;           // Associated GUI

};

#endif /* PIGSDAQ_H_ */
