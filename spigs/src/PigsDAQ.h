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

#include <CAENDPPLib.h>
#include <TH1I.h>

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
	PigsDAQ();

public:
	int32_t PigsDAQ::InitDgtzParams(); 	// Initialize the digitizer parameters to their default value
	int32_t PigsDAQ::SetDgtzParams(); 	// Set custom digitizer parameters
	int32_t PigsDAQ::SetDAQParams();	// Set custom DAQ parameters
	int32_t PigsDAQ::InitDPPLib();		// Initialize the DPP library
	int32_t PigsDAQ::AddBoardUSB();		// Adds board 0 on USB bus to the DPP library
//	int32_t PigsDAQ::PrintBoardInfo();  // Prints board info
	int32_t PigsDAQ::ConfigureBoard();	// Configures the board with digitizer and DAQ parameters

	int32_t PigsDAQ::isChannelDisabled(int32_t ch);
	void PigsDAQ::PrintChannelParameters(int32_t ch); //Prints Channel Parameters
	char * PigsDAQ::decodeError(char *dest, int32_t code); // Decodes the given error code into a message

	~PigsDAQ();
	static PigsDAQ* PigsDAQ::getInstance();
	//	static PigsDAQ * getInstance(){
	//		if(!instantiated) cout<<"<PigsDAQ * getInstance> Please instantiate correct Helper fist"<<endl;
	//		return instance;
	//	}

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
	const uint32_t usecSleepPollDAQ;      // sleep in microseconds to poll DAQ acquisition status
	uint64_t realTime, deadTime;
    uint32_t *h1;						  // histogram used for DPP dump

	char codeStr[MAX_ERRMSG_LEN + 1];
	char histoFName[MAX_HISTOFNAME_LEN];

	int32_t fVerbose;	// verbosity level settings
	int32_t fErrCode;	// error code from DPP calls
	TH1I *fCurrHist;	// Current histogram

};

#endif /* PIGSDAQ_H_ */
