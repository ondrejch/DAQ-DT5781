/*
 * Position Indicating Gamma Sensing system
 * Four channel version
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
#include <string.h>         // strcpy
#include <iostream>         // std::cerr
#include <typeinfo>         // operator typeid
#include <exception>        // std::exception

// CAEN
#include <CAENDPPLib.h>

// ROOT
#include <TROOT.h>
#include <TH1F.h>
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

    int32_t ConfigureChannel(int32_t ch);       // Sets channel parameters specified in Init & Set calls
    int32_t StopAcquisition(int32_t ch);        // Stops acquisition for channel ch
    int32_t AcquisitionSingleLoop();            // Runs acquisition
    int32_t ThreadAcqSingleLoop();              // Runs acquisition in a separate thread
    void PrintAcquisotionInfo(int32_t ch);      // Prints real/dead time, cps, ...
    void SetAcquisitionLoopTime(Float_t sec);   // Set acquisition loop time in seconds
    Float_t GetAcquisitionLoopTime() const;

    int32_t RefreshCurrHist();                  // Transfers h1 into currHist

    TH1F *getCurrHist(int32_t ch) const;        // Getters & setters
    Float_t  getCountsPerSecond(int32_t ch) const;
    uint32_t getGoodCounts(int32_t ch) const;
    uint64_t getRealTime(int32_t ch) const;
    uint64_t getDeadTime(int32_t ch) const;
    uint32_t getTotCounts(int32_t ch) const;
    CAENDPP_StopCriteria_t getStopCriteria() const;
    uint64_t getStopCriteriaValue() const;
    void setStopCriteria(CAENDPP_StopCriteria_t stopCriteria);
    void setStopCriteriaValue(uint64_t stopCriteriaValue);
    PigsGUI *getGUI() const;
    void setGUI(PigsGUI *gui);
    const char *getBoardInfo() const;
    const TTimeStamp& getTimeStamp() const;

protected:
    static PigsDAQ * instance;
    static bool instantiated;
    PigsDAQ();
    ~PigsDAQ(){};

private:
    int32_t  ch;        // ch is the variable used to identify a board's channel inside DPPLibrary.
    int32_t handle;     // Handler to the Library
    int32_t brd;        // Board Identifier - this code assumes we only have one board
    CAENDPP_ConnectionParams_t connParam;   // Connection Parameters - Used to connect to the board.
    CAENDPP_Info_t info;                    // Board Infos Structure
    TString fBoardInfo;                     // String that holds board information
    CAENDPP_DgtzParams_t dgtzParams;        // Acquisition Parameters - Used to set the acquisition properties.
    CAENDPP_AcqMode_t acqMode;              // Acquisition Mode (Histogram or Waveforms)
    CAENDPP_InputRange_t iputLevel;         // Channel input range
    CAENDPP_StopCriteria_t StopCriteria;    // Stop Criteria definition
    uint64_t StopCriteriaValue;             // Stop Criteria value
    CAENDPP_AcqStatus_t isAcquiring;        // 1 yes, 0 no
    CAENDPP_AcqStatus_t checkAcquiring[4];  // 1 yes, 0 no, for each channel
    uint32_t usecSleepPollDAQ;              // Sleep in microseconds to poll DAQ acquisition status
    uint64_t realTime[4];                   // Real time from DAQ [ns]
    uint64_t deadTime[4];                   // Dead time from DAQ [ns]
    uint32_t goodCounts[4];                 // Counts as measured by DAQ
    uint32_t totCounts[4];                  // DAQ counts scaled up by (1+ deadTime)/realTime
    Float_t  countsPerSecond[4];            // goodCounts/(realTime-deadTime)
    uint32_t *h[4];                 // histograms used for DPP dump
    int32_t  hNBins[4];             // number of bins in the histograms

    char codeStr[MAX_ERRMSG_LEN + 1];
    char histoFName[MAX_HISTOFNAME_LEN];

    static const int32_t fVerbose = 1;      // verbosity level settings
    int32_t fErrCode;       // Error code from DPP calls
    TH1F *fCurrHist[4];     // Current histograms
    TTimeStamp fDt;         // Current date for histogram time
    TString fAcqDate;       // Acquisition date
    UInt_t year, month, day, hour, min, sec;    // Helpers to form strings from time stamps
    PigsGUI *gui;           // Associated GUI
};

#endif /* PIGSDAQ_H_ */
