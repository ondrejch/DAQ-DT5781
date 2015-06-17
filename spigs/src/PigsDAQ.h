/*
 * Position Indicating Gamma Sensing system
 *
 * PigsDAQ.h - Main/Helper singleton class
 *
 *
 *  Created on: Jun 12, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#ifndef PIGSDAQ_H_
#define PIGSDAQ_H_

#include <stdio.h>      	// printf, scanf, NULL */
#include <stdlib.h>     	// calloc, exit, free
#include <iostream>       	// std::cerr
#include <typeinfo>       	// operator typeid
#include <exception>      	// std::exception
#include <string.h>			// strcpy

// CAEN
#include <CAENDPPLib.h>

// ROOT
//#include <TROOT.h>
//#include <TH1I.h>

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

class PigsDAQ {
protected:
	static PigsDAQ * instance;
	static bool instantiated;
	PigsDAQ(){};
	~PigsDAQ(){};

public:
	int32_t BasicInit();	    // Does all basic initialization
	int32_t InitDgtzParams(); 	// Initialize the digitizer parameters to their default value
	int32_t SetDgtzParams(); 	// Set custom digitizer parameters
	int32_t SetDAQParams();	    // Set custom DAQ parameters
	int32_t InitDPPLib();		// Initialize the DPP library
	int32_t AddBoardUSB();		// Adds board 0 on USB bus to the DPP library
//	int32_t PigsDAQ::PrintBoardInfo();  // Prints board info
	int32_t ConfigureBoard();	// Configures the board with digitizer and DAQ parameters

	int32_t isChannelDisabled(int32_t ch);
	void PrintChannelParameters(int32_t ch); //Prints Channel Parameters
	char * decodeError(char *dest, int32_t code); // Decodes the given error code into a message

	static PigsDAQ * getInstance();

private:
	int32_t ch;  		//  ch is the variable used to identify a board's channel inside DPPLibrary.
	int32_t handle; 	// Handler to the Library
	int32_t brd; 		// Board Identifier - this code assumes we only have one board
	CAENDPP_ConnectionParams_t connParam; // Connection Parameters - Used to connect to the board.
	CAENDPP_Info_t info; 				  // Board Infos Structure
	CAENDPP_DgtzParams_t dgtzParams;      // Acquisition Parameters - Used to set the acquisition properties.
	CAENDPP_AcqMode_t acqMode; 			  // Acquisition Mode (Histogram or Waveforms)
	CAENDPP_InputRange_t iputLevel; 	  // Channel input range
	CAENDPP_StopCriteria_t StopCriteria;  // Stop Criteria definition
	uint64_t StopCriteriaValue; 		  // Stop Criteria value
	CAENDPP_AcqStatus_t isAcquiring;      // 1 yes, 0 no
	uint32_t usecSleepPollDAQ;            // sleep in microseconds to poll DAQ acquisition status
	uint64_t realTime, deadTime;
    uint32_t *h1;						  // histogram used for DPP dump

	char codeStr[MAX_ERRMSG_LEN + 1];
	char histoFName[MAX_HISTOFNAME_LEN];

	static const int32_t fVerbose;	// verbosity level settings
	int32_t fErrCode;	// error code from DPP calls
//	TH1I *fCurrHist;	// Current histogram

};

#endif /* PIGSDAQ_H_ */
