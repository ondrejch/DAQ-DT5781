/*******************************************************************************
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
*
* \file     oDAQ.c
* \brief    Ondrej's DAQ v 0.010
*           Based on CAEN's DPPLibDemo
*           4-channel Position Indicating Gamma Sensing system (4PIGS)
* \author   Ondrej Chvala <ochvala@utk.edu>
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <CAENDPPLib.h>
#include <oDAQ.h>
#include <Functions.h>

/// \fn      int32_t main();
/// \brief   Main Program
int32_t main() {
  int32_t ret = 0;
  int32_t ch;  			//  ch is the variable used to identify a board's channel inside DPPLibrary.
  int32_t handle; 		// Handler to the Library
  int32_t bId[MAX_BOARD_NUMBER], brd; 		// Board Identifier
  CAENDPP_ConnectionParams_t connParam; 	// Connection Parameters - Used to connect to the board.
  CAENDPP_Info_t info[MAX_BOARD_NUMBER]; 	// Board Infos Structure
  CAENDPP_DgtzParams_t dgtzParams; 			// Acquisition Parameters - Used to set the acquisition properties.
  CAENDPP_AcqMode_t acqMode; 				// Acquisition Mode (Histogram or Waveforms)
  CAENDPP_InputRange_t iputLevel = CAENDPP_InputRange_1_0Vpp; // Input range
  CAENDPP_StopCriteria_t StopCriteria;      // Stop Criteria definition
  uint64_t StopCriteriaValue; 				// Stop Criteria value
  CAENDPP_AcqStatus_t isAcquiring;          // 1 yes, 0 no
  const uint32_t usecSleepDAQ = 50000;      // sleep in microseconds
// Channel settings


  // Other Variable definition
  uint32_t *h1;
  //uint32_t MIN_REQ_EVTS = 100000;
  //uint32_t nEvts = 0;
  int16_t *AT1 = NULL, *AT2 = NULL;
  uint8_t *DT1 = NULL, *DT2 = NULL;
  uint16_t u16;
  uint32_t u32;
  uint64_t realTime, deadTime;
  int32_t SWTriggerEnabled = 0;
  int32_t bFirst, bLast;
  int32_t chFirst, chLast;
  int32_t histoNBins;
  char codeStr[MAX_ERRMSG_LEN + 1];
  char histoFName[MAX_HISTOFNAME_LEN];

  // Init global variables
  currBrd = - 1;
  currCh = - 1;
  currHisto = - 1;
  connectedBrdNum = 0;
  connectedChNum = 0;
  gnuplot = NULL;

  // Init digitizer parameters to default value
  //
  // NOTE: those parameters are probably NOT GOOD for you!
  // Refer to the DPP - PHA and CAENDigitizer.dll manuals
  // for details. You can also try to use the AutoSet functionality
  // or to change the parameters run-time
  ret = InitDgtzParams(&dgtzParams);
  // Custom digitizer parameters
  ret = SetDAQParams(&dgtzParams);
  /*for (ch = 0; ch < MAX_BOARD_CHNUM; ch++) {
    dgtzParams.DPPParams.thr[ch] = 10;					// Trigger Threshold
    //dgtzParams.PulsePolarity[ch] = CAENDPP_PulsePolarityPositive;
    dgtzParams.PulsePolarity[ch] = CAENDPP_PulsePolarityNegative;
    //dgtzParams.DCoffset[ch] = 7864;						// default 7864
    //dgtzParams.WFParams.preTrigger = (int32_t)(10);		// Pre-trigger samples
    //dgtzParams.WFParams.probeSelfTriggerVal = 150;
  }*/

  // The channel mask is used to set the enabled Acquisition Input Channels
  dgtzParams.ChannelMask = 0x1;

  // Set the Acquisition Mode
  acqMode = CAENDPP_AcqMode_Histogram;    // For Histogram mode (no waveforms)
  //acqMode = CAENDPP_AcqMode_Waveform;   // For Oscilloscope mode (waves + histogram)

  // Alloc the histogram to the given number of bins.
  h1 = (uint32_t *)calloc(MAX_HISTO_NBINS, sizeof(uint32_t));
  if (h1 == NULL) {
      ret = - 1; return ret; //break;
  //   goto QuitProgram;
  }

 // Init the library
  ret = CAENDPP_InitLibrary(&handle); // The handle will be used to command the library
  printf("System Init: %d\n", ret);

  // ***  Add board 0  ***
  // The following is for direct USB connection
  connParam.LinkType = CAENDPP_USB;
  connParam.LinkNum = 0;              // This defines the USB port to use and must increase with board number; ex: for board 3 must be 3
  connParam.ConetNode = 0;            // This MUST be 0
  connParam.VMEBaseAddress = 0x0;     // For direct connection the address must be 0


  // Add board to the Library
  ret = CAENDPP_AddBoard(handle, connParam, &bId[connectedBrdNum]);
  if (ret != CAENDPP_RetCode_Ok) {
    printf("Error adding board with selected connection parameters: %s\n", decodeError(codeStr, ret));
    return ret; //break;
  }

  // Get Board Info
  ret = CAENDPP_GetDPPInfo(handle, bId[connectedBrdNum], &info[connectedBrdNum]);
  if (ret != CAENDPP_RetCode_Ok) {
     printf("Error getting board info: %s\n", decodeError(codeStr, ret));
     return ret; //break;
  }

  printf("Board %s_%u added succesfully\n", info[connectedBrdNum].ModelName, info[connectedBrdNum].SerialNumber);

  // Increase globals
  connectedChNum += info[connectedBrdNum].Channels;
  connectedBrdNum++;
  // *** board 0 added ****

  // DPPLibrary API used in DPPDemo_Cmd_Configure:
  //   - CAENDPP_SetBoardConfiguration(int32_t handle, int32_t boardId, CAENDPP_AcqMode_t acqMode, CAENDPP_DgtzParams_t dgtz_params);
  // Configure Boards
  // Write the configuration to the boards
  ret = CAENDPP_RetCode_Ok;
  getBrdRange(&bFirst, &bLast);
  if (bFirst == bLast) { // No board Added
      printf("Can't configure boards: No board Added\n");
      return ret; // break;
  }
  for (brd = bFirst; brd < bLast; brd++) {
      printf(" - Configuring board %d\n",brd);
      ret = CAENDPP_SetBoardConfiguration(handle, bId[brd], acqMode, dgtzParams);
      if (ret != CAENDPP_RetCode_Ok)
          printf("Can't configure board %s_%u: %s\n", info[brd].ModelName, info[brd].SerialNumber, decodeError(codeStr, ret));
  }

  if (ret == CAENDPP_RetCode_Ok)  {
      printf("Boards Configured\n");
  } else {
     printf("Error in board configuration\n");
     return ret;
  }

/*  // Reset all histograms on four channels
  getChRange(&chFirst, &chLast);
  for (ch = chFirst; ch < chLast; ch++) {
    ret = CAENDPP_ResetAllHistograms(handle, ch);
    if (ret != CAENDPP_RetCode_Ok) {
      printf("Error resetting all histograms: %s\n", decodeError(codeStr, ret));
      return ret;
    }
    printf(" - channel %d reset ok\n", ch);
  }*/

   ch = currCh = 0;

  // Check if the channel is enabled
  CAENDPP_AcqStatus_t acqStatus;
  if (isChannelDisabled(handle, ch)) {
    printf("Warning: Channel %d disabled\n",ch);
  }

// Set input range for channel 0
  ret = CAENDPP_SetInputRange(handle, ch, iputLevel);
  if (ret == CAENDPP_RetCode_Ok)  {
      printf("Input range for channel %d set to %d\n", ch, iputLevel);
  } else {
     printf("Error setting input range\n");
     return ret;
  }

  // Print channel 0 pars
  PrintChannelParameters(ch, &dgtzParams);

 // Set stop criteria
  StopCriteria = CAENDPP_StopCriteria_RealTime;
//StopCriteriaValue = 600L*1000000000L;      // [ns] Run for 600 seconds
//StopCriteriaValue =  10L*1000000000L;      // [ns] Run for  10 seconds
  StopCriteriaValue =   1000000000;// 1000*1000*1000; // 1 sec [ns] Run for 600 seconds

  ret = CAENDPP_SetStopCriteria(handle, currCh, StopCriteria, StopCriteriaValue);
  if (ret != CAENDPP_RetCode_Ok)
    printf("Can't set Stop Criteria for channel %d: %s\n", currCh, decodeError(codeStr, ret));
            else
    printf("Stop Criteria successfully set: %d, %llu\n", StopCriteria, StopCriteriaValue);

int cycles=10;
int ic;
for (ic=0; ic<cycles; ic++) {

   ret = CAENDPP_ClearHistogram(handle, currCh, 0);
  if (ret != CAENDPP_RetCode_Ok) {
    printf("Error clearing histogram\n");
    return ret;
  } else {
    printf("Histogram on channel %d cleaned.", currCh);
  }

  // Start Acquisition for channel 0
  ret = CAENDPP_StartAcquisition(handle, currCh);
  if (ret != CAENDPP_RetCode_Ok) {
    printf("Error Starting Acquisition for channel %d: %s\n", currCh, decodeError(codeStr, ret));
    return ret;
  } else {
    printf("Acquisition Started for channel %d\n", currCh);
  }


  // wait
 //sleep(600); // 172800s = 6h
   do {
     usleep(usecSleepDAQ);
     CAENDPP_IsChannelAcquiring(handle, ch, &isAcquiring );
  // debug   printf("  -- ch %d acq status %d\n", ch, isAcquiring);
   } while (isAcquiring);


  // Get The Histogram
  ret = CAENDPP_GetCurrentHistogram(handle, ch, h1, &u32, &realTime, &deadTime, &acqStatus);
  if (ret != CAENDPP_RetCode_Ok) {
      printf("ERROR: Can't get current Histogram for ch %d: %s\n", ch, decodeError(codeStr, ret));
      //continue;
  }
  // Now you can do what you want with that histogram. We just print it to file
  ret = CAENDPP_GetHistogramSize(handle, ch, HISTO_IDX_CURRENT, &histoNBins);
  if (ret != CAENDPP_RetCode_Ok) {
      printf("ERROR: Can't get current Histogram size for ch %d: %s\n", ch, decodeError(codeStr, ret));
   //   continue;
  }
  sprintf(histoFName, "histo_ch%d_%d.txt", ch, ic);
  saveHistogram(histoFName, h1, histoNBins, realTime, deadTime);
  printf("CH%d\n", ch);
  printf(" RT\t= %.3f s\n", realTime / 1000000000.0);
  printf(" DT\t= %.3f ms\n", deadTime / 1000000.0);
  printf(" Good\t= %u counts\n", u32);
  printf(" Tot.\t= %u counts\n", (uint32_t)(u32 * (1.0 + (double)deadTime / (double)(realTime))));
  printf(" ICR\t= %.3f counts/s\n", (realTime - deadTime) > 0 ? 
		(double)u32 / (double)((realTime - deadTime) / 1000000000.0) : 0);

 if (ret == CAENDPP_RetCode_Ok)
   printf("All Histograms saved successfully.\n");


  // Stop Acquisition for channel 0
  ret = CAENDPP_StopAcquisition(handle, currCh);
  if (ret != CAENDPP_RetCode_Ok) {
    printf ("Error Stopping Acquisition for channel %d: %s\n", currCh, decodeError(codeStr, ret));
  } else {
    printf("Acquisition Stopped for channel %d\n", currCh);
  }
}

// end
  printf("Program End\n");
  CAENDPP_EndLibrary(handle);

  // Dealloc histogram and probes
  if (h1 != NULL) free(h1);
  if (AT1 != NULL) free(AT1);
  if (AT2 != NULL) free(AT2);
  if (DT1 != NULL) free(DT1);
  if (DT2 != NULL) free(DT2);

  return ret;

}

