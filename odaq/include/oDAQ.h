/*******************************************************************************
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
*
* \file     oDAQ.h
* \brief    Ondrej's DAQ plain c version
*           Based on CAEN's DPPLibDemo
* \author   Ondrej Chvala <ochvala@utk.edu>
*
*******************************************************************************/

#ifndef __ODAQ_H
#define __ODAQ_H

#include <stdlib.h>
#include <stdio.h>

// GNUPlot Command
#define GNUPLOT_CMD "gnuplot"

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
#define PLOT_PERIOD_MS      (1000)  // Continuous plot interval (ms)
#define MAX_STATUSMSG_LEN   (200)   // Maximum length in chars of the status string
#define MAX_ERRMSG_LEN      (100)   // Maximum length in chars of the error message string
#define MAX_HVNAME_LEN      (29)    // Maximum Length of Active HV Channel Name
#define MAX_TRACENAME_LEN   (50)    // Maximum Length of Traces Name
#define MAX_HISTOFNAME_LEN  (100)   // Maximum Length of Histograms File Name

// Some globals used to manage number of bins, active board/channel, plotting, etc.
int32_t currBrd;                                // Current board index
int32_t currCh;                                 // Current channel index
int32_t currHisto;                              // Current histogram index
int32_t connectedBrdNum;                        // Number of connected boards
int32_t connectedChNum;                         // Number of connected channels
FILE *gnuplot;                                  // Pipe to the plotter

#endif
