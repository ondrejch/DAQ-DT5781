/******************************************************************************
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
*
* \file     Functions.h
* \brief    CAEN - Defintion of CAENDPPLibrary Demo Utility's API
* \author   Originally: Francesco Pepe (support.computing@caen.it)
*           Modified by Ondrej Chvala <ochvala@utk.edu>, do not blame Francesco!
*
******************************************************************************/

#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

// Redfine posix deprecated
#define _popen popen
// define function max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
// define function min
//#define min(a,b)            (((a) > (b)) ? (b) : (a))

#include <CAENDPPLib.h>
#include <oDAQ.h>

/// \fn      uint64_t getTimeMs();
/// \brief   Returns the current time in ms
///
/// \return  the time in ms
uint64_t getTimeMs();

/// \fn      int32_t isPow2(uint32_t val);
/// \brief   Gets if val is a power of 2
///
/// \param   [IN]  val     : The value to check
///
/// \return  0 = false; 1 = true
int32_t isPow2(uint32_t val);

/// \fn      int32_t saveHistogram(char *filename, uint32_t *EHisto, int32_t nbins);
/// \brief   Saves a Histogram to file
///
/// \param   [IN]  filename     : The name of the file to write to
/// \param   [IN]  EHisto       : The Pointer to the Histogram Data
/// \param   [IN]  nbins        : The number of bins of the histogram
/// \param   [IN]  RT_ns        : The Real Time in ns (if 0 it is not dumped)
/// \param   [IN]  DT_ns        : The Dead Time in ns (if 0 it is not dumped)
///
/// \return  0 = Success; negative numbers are error codes
int32_t saveHistogram(char *filename, uint32_t *EHisto, int32_t nbins, uint64_t RT_ns, uint64_t DT_ns);

/// \fn      int32_t InitDgtzParams(CAENDPP_DgtzParams_t *Params);
/// \brief   Initialize the configuration parameters to their default value
///
/// \param   [OUT]  Params      : Pointer to the parameters structure to fill
///
/// \return  0 = Success; negative numbers are error codes
int32_t InitDgtzParams(CAENDPP_DgtzParams_t *Params);

// \fn      int32_t SetDAQParams(CAENDPP_DgtzParams_t *Params);
/// \brief   Configures DAQ parameters for oDAQ purposes
///
/// \param   [OUT]  Params      : Pointer to the parameters structure
///
/// \return  0 = Success; negative numbers are error codes
int32_t SetDAQParams(CAENDPP_DgtzParams_t *Params);


/// \fn      int32_t PrintChannelParameters(int32_t ch, CAENDPP_DgtzParams_t *Params);
/// \brief   Prints Channel Parameters
///
/// \param   [IN]   ch      : The channel to modify
/// \param   [I/O]  Params  : Pointer to the parameters structure to print
///
/// \return  0 = Success; negative numbers are error codes
int32_t PrintChannelParameters(int32_t ch, CAENDPP_DgtzParams_t *Params);


/// \fn      char *decodeError(char *dest, int32_t ret);
/// \brief   Decodes the given error code into a message
///
/// \param   [IN] dest   : the string to be filled
/// \param   [IN] ret    : the error code
///
/// \return  a pointer to the string if success, otherwise NULL
char *decodeError(char *dest, int32_t code);

/// \fn      int32_t getBrdRange(int32_t *f, int32_t *l);
/// \brief   Gets the board range depending on 'currBrd' value.
///          This function gives the complete board range if 'currBrd' < 0
///          or the single board range otherwise. The resulting
///          range is aimed to be used inside a 'for' loop
///
/// \param   [OUT] f    : Index of the First board in range
/// \param   [OUT] l    : Index of the Last board in range
///
/// \return  0 = Success; negative numbers are error codes
int32_t getBrdRange(int32_t *f, int32_t *l);


/// \fn      int32_t getChRange(int32_t *f, int32_t *l) {
/// \brief   Gets the channel range depending on 'currCh' value.
///          This function gives the complete channel range if 'currCh' < 0
///          or the single channel range otherwise. The resulting
///          range is aimed to be used inside a 'for' loop
///
/// \param   [OUT] f    : Index of the First channel in range
/// \param   [OUT] l    : Index of the Last channel in range
///
/// \return  0 = Success; negative numbers are error codes
int32_t getChRange(int32_t *f, int32_t *l);


/// \fn      int32_t isChannelDisabled(int32_t handle, int32_t ch);
/// \brief   Get if a channel is Enabled
///
/// \param   [IN] handle    : Handle to the Library
/// \param   [IN] ch        : Channel index to check
///
/// \return  0 if the channel is enabled, 1 if it is disabled
int32_t isChannelDisabled(int32_t handle, int32_t ch);


/// \fn      int32_t plotHisto(char *title, uint32_t *EHisto, int32_t nbins, uint32_t counts, double RT_s, double DT_s);
/// \brief   Plots an Histogram
///
/// \param   [IN] title     : Title of the histogram
/// \param   [IN] EHisto    : Histogram Datas
/// \param   [IN] nbins     : Histogram Number of Bins
/// \param   [IN] counts    : Histogram Total Counts
/// \param   [IN] RT_s      : Histogram Real Time
/// \param   [IN] DT_s      : Histogram Dead Time
///
/// \return  0 = Success; negative numbers are error codes
int32_t plotHisto(char *title, uint32_t *EHisto, int32_t nbins, uint32_t counts, double RT_s, double DT_s);


#endif
