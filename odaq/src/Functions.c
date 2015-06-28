/******************************************************************************
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
*
* \file     Functions.c
* \brief    CAEN - Utility's source of Routines used in CAENDPPLibrary Demo
* \author   Originally: Francesco Pepe (support.computing@caen.it)
*           Modified by Ondrej Chvala <ochvala@utk.edu>, do not blame Francesco!
*
******************************************************************************/

#include <Functions.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <string.h>


// Definition of the commands  chain
// connected to specific chars
typedef struct timedCmd_t {
    char c;
    uint64_t timeOutMs;
    struct timedCmd_t *next;
    struct timedCmd_t *prev;
    uint64_t startTime;
} timedCmd_t;

//static timedCmd_t *WNC_COMMANDS = NULL;

/// \fn      uint64_t getTimeMs();
/// \brief   Returns the current time in ms
///
/// \return  the time in ms
uint64_t getTimeMs() {
    uint64_t res;
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = (uint64_t)(start.tv_sec * 1000) + (uint64_t)(start.tv_nsec * 0.000001);
    return res;
}

/// \fn      int32_t isPow2(uint32_t val);
/// \brief   Gets if val is a power of 2
///
/// \param   [IN]  val     : The value to check
///
/// \return  0 = false; 1 = true
int32_t isPow2(uint32_t val) {
    int res = (val & (val >> 1)) == 0 ? 1 : 0;
    if (val == 0)
        res = 1;
    return res;
}

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
int32_t saveHistogram(char *filename, uint32_t *EHisto, int32_t nbins, uint64_t RT_ns, uint64_t DT_ns) {
    FILE *fh;
    int32_t i;
    long long unsigned int u64;
    fh = fopen(filename, "w");
    if (fh == NULL)
        return - 1;
    if (RT_ns > 0) {
        u64 = RT_ns;
        fprintf(fh, "RealTime %llu\n", u64);
    }
    if (DT_ns > 0) {
        u64 = DT_ns;
        fprintf(fh, "DeadTime %llu\n", u64);
    }
    for (i = 0; i < nbins; i++) {
        fprintf(fh, "%u\n", EHisto[i]);
    }
    fclose(fh);

    return 0;
}

/// \fn      char *decodeError(char *dest, int32_t ret);
/// \brief   Decodes the given error code into a message
///
/// \param   [IN] dest   : the string to be filled
/// \param   [IN] ret    : the error code
///
/// \return  a pointer to the string if success, otherwise NULL
char *decodeError(char *dest, int32_t code) {
    char codeStr[10];
    size_t nc = MAX_ERRMSG_LEN;
    sprintf(codeStr, " [%d]", code);
    nc -= strlen(codeStr);
    switch(code) {
    case CAENDPP_RetCode_Ok: strncpy(dest, "Success", nc); break;
    case CAENDPP_RetCode_GenericError: strncpy(dest, "Generic Error", nc); break;
    case CAENDPP_RetCode_TooManyInstances: strncpy(dest, "Too Many Instances", nc); break;
    case CAENDPP_RetCode_ProcessFail: strncpy(dest, "Process Fail", nc); break;
    case CAENDPP_RetCode_ReadFail: strncpy(dest, "Read Fail", nc); break;
    case CAENDPP_RetCode_WriteFail: strncpy(dest, "Write Fail", nc); break;
    case CAENDPP_RetCode_BadMessage: strncpy(dest, "Bad Message", nc); break;
    case CAENDPP_RetCode_InvalidHandle: strncpy(dest, "Invalid Handle", nc); break;
    case CAENDPP_RetCode_ConfigError: strncpy(dest, "Configuration Error", nc); break;
    case CAENDPP_RetCode_BoardInitFail: strncpy(dest, "Board Initialization Failed", nc); break;
    case CAENDPP_RetCode_TimeoutError: strncpy(dest, "Timeout Error", nc); break;
    case CAENDPP_RetCode_InvalidParameter: strncpy(dest, "Invalid Parameter", nc); break;
    case CAENDPP_RetCode_NotInWaveMode: strncpy(dest, "Not in Waveform Mode", nc); break;
    case CAENDPP_RetCode_NotInHistoMode: strncpy(dest, "Not in Histogram Mode", nc); break;
    case CAENDPP_RetCode_NotInListMode: strncpy(dest, "Not in List Mode", nc); break;
    case CAENDPP_RetCode_NotYetImplemented: strncpy(dest, "Not Yet Implemented", nc); break;
    case CAENDPP_RetCode_BoardNotConfigured: strncpy(dest, "Board not Configured", nc); break;
    case CAENDPP_RetCode_InvalidBoardIndex: strncpy(dest, "Invalid Board Index", nc); break;
    case CAENDPP_RetCode_InvalidChannelIndex: strncpy(dest, "Invalid Channel Index", nc); break;
    case CAENDPP_RetCode_UnsupportedFirmware: strncpy(dest, "Unsupported Firmware", nc); break;
    case CAENDPP_RetCode_NoBoardsAdded: strncpy(dest, "No Boards Added", nc); break;
    case CAENDPP_RetCode_AcquisitionRunning: strncpy(dest, "Acquisition Running", nc); break;
    case CAENDPP_RetCode_OutOfMemory: strncpy(dest, "Out of Memory", nc); break;
    case CAENDPP_RetCode_BoardChannelIndex: strncpy(dest, "Invalid Board Channel Index", nc); break;
    case CAENDPP_RetCode_HistoAlloc: strncpy(dest, "Histogram Allocation Failed", nc); break;
    case CAENDPP_RetCode_OpenDumper: strncpy(dest, "Dumper Open Failed", nc); break;
    case CAENDPP_RetCode_BoardStart: strncpy(dest, "Board Start Failed", nc); break;
    case CAENDPP_RetCode_ChannelEnable: strncpy(dest, "Channel not Enabled", nc); break;
    case CAENDPP_RetCode_InvalidCommand: strncpy(dest, "Invalid Command", nc); break;
    case CAENDPP_RetCode_NumBins: strncpy(dest, "Invalid Number of Bins", nc); break;
    case CAENDPP_RetCode_HistoIndex: strncpy(dest, "Invalid Histogram Index", nc); break;
    case CAENDPP_RetCode_UnsupportedFeature: strncpy(dest, "Unsupported Feature", nc); break;
    case CAENDPP_RetCode_BadHistoState: strncpy(dest, "Incompatible Histogram Status", nc); break;
    case CAENDPP_RetCode_NoMoreHistograms: strncpy(dest, "No more Histograms Available", nc); break;
    case CAENDPP_RetCode_NotHVBoard: strncpy(dest, "Not an HV Board", nc); break;
    case CAENDPP_RetCode_InvalidHVChannel: strncpy(dest, "Invalid HV Channel", nc); break;
    case CAENDPP_RetCode_SocketSend: strncpy(dest, "Socket Send Error", nc); break;
    case CAENDPP_RetCode_SocketReceive: strncpy(dest, "Socket Receive Error", nc); break;
    case CAENDPP_RetCode_BoardThread: strncpy(dest, "Board Acquisition Thread Error", nc); break;
    case CAENDPP_RetCode_DecodeWaveform: strncpy(dest, "Cannot Decode Waveform from Buffer", nc); break;
    case CAENDPP_RetCode_OpenDigitizer: strncpy(dest, "Error Opening the digitizer", nc); break;
    case CAENDPP_RetCode_BoardModel: strncpy(dest, "Unsupported Board Model", nc); break;
    case CAENDPP_RetCode_AutosetStatus: strncpy(dest, "Incompatible Autoset Status", nc); break;
    case CAENDPP_RetCode_Autoset: strncpy(dest, "Autoset Error", nc); break;
    default: strncpy(dest, "UNKNOWN Error", nc);
    }
    strcat(dest, codeStr);
    return dest;
}


/// \fn      int32_t InitDgtzParams(CAENDPP_DgtzParams_t *Params);
/// \brief   Initialize the configuration parameters to their default value
///
/// \param   [OUT]  Params      : Pointer to the parameters structure to fill
///
/// \return  0 = Success; negative numbers are error codes
int32_t InitDgtzParams(CAENDPP_DgtzParams_t *Params) {
    int32_t ch;

    // set listMode default parameters
    Params->ListParams.enabled = FALSE;
    Params->ListParams.saveMode = CAENDPP_ListSaveMode_FileBinary;
    strcpy(Params->ListParams.fileName, "UNNAMED");
    Params->ListParams.maxBuffNumEvents = 0;
    Params->ListParams.saveMask = 0xF;

    // default board parameters
    Params->ChannelMask = 0; // it will be filled later
    Params->EventAggr = 0;
    Params->IOlev = CAENDPP_IOLevel_NIM;

    // Generic Writes to Registers
    Params->GWn = 0;                                    // Number of Generic Writes
    memset(Params->GWaddr, 0, MAX_GW*sizeof(uint32_t)); // List of addresses (length = 'GWn')
    memset(Params->GWdata, 0, MAX_GW*sizeof(uint32_t)); // List of datas (length = 'GWn')
    memset(Params->GWmask, 0, MAX_GW*sizeof(uint32_t)); // List of masks (length = 'GWn')

    // Waveform parameters default settings
    Params->WFParams.dualTraceMode = 1;
    Params->WFParams.vp1 = CAENDPP_PHA_VIRTUALPROBE1_Input;
    Params->WFParams.vp2 = CAENDPP_PHA_VIRTUALPROBE2_TrapBLCorr;
    Params->WFParams.dp1 = CAENDPP_PHA_DigitalProbe1_Armed;
    Params->WFParams.dp2 = CAENDPP_PHA_DigitalProbe2_Trigger;
    Params->WFParams.recordLength = (int32_t)(8192);
    Params->WFParams.preTrigger = (int32_t)(100);
    Params->WFParams.probeSelfTriggerVal = 150;
    Params->WFParams.probeTrigger = CAENDPP_PHA_PROBETRIGGER_MainTrig;

    // Channel parameters
    for (ch = 0; ch < MAX_BOARD_CHNUM; ch++) {
        // Channel parameters
        Params->DCoffset[ch] = 7864;
        Params->PulsePolarity[ch] = CAENDPP_PulsePolarityPositive;

        // Coicidence parameters between channels
        Params->CoincParams[ch].CoincChMask = 0x0;
        Params->CoincParams[ch].CoincLogic = CAENDPP_CoincLogic_None;
        Params->CoincParams[ch].CoincOp = CAENDPP_CoincOp_OR;
        Params->CoincParams[ch].MajLevel = 0;
        Params->CoincParams[ch].TrgWin = 0;

        // DPP Parameters
        Params->DPPParams.M[ch] = 47500;        // Signal Decay Time Constant
        Params->DPPParams.m[ch] = 5000;         // Trapezoid Flat Top
        Params->DPPParams.k[ch] = 5000;         // Trapezoid Rise Time
        Params->DPPParams.ftd[ch] = 2500;       // Flat Top Delay
        Params->DPPParams.a[ch] = 4;            // Trigger Filter smoothing factor
        Params->DPPParams.b[ch] = 200;          // Input Signal Rise time
        Params->DPPParams.thr[ch] = 1500;       // Trigger Threshold
        Params->DPPParams.nsbl[ch] = 3;         // Number of Samples for Baseline Mean
        Params->DPPParams.nspk[ch] = 0;         // Number of Samples for Peak Mean Calculation
        Params->DPPParams.pkho[ch] = 0;         // Peak Hold Off
        Params->DPPParams.blho[ch] = 1000;      // Base Line Hold Off
        Params->DPPParams.dgain[ch] = 0;        // Digital Probe Gain
        Params->DPPParams.enf[ch] = 1.0;        // Energy Normalization Factor
        Params->DPPParams.decimation[ch] = 0;   // Decimation of Input Signal
        Params->DPPParams.trgho[ch] = 100;      // Trigger Hold Off
        Params->DPPParams.twwdt[ch] = 0;        //
        Params->DPPParams.trgwin[ch] = 0;       //

        // Parameters for X770
        Params->ChannelExtraParameters[ch].analogPath = 0;
		Params->ChannelExtraParameters[ch].CRgain = 0;
		Params->ChannelExtraParameters[ch].InputImpedance = 0;
		Params->ChannelExtraParameters[ch].PRDSgain = 0;
		Params->ChannelExtraParameters[ch].ResetDetector.EnableResetDetector = 0;
        Params->ChannelExtraParameters[ch].ResetDetector.thrhold = 100;
		Params->ChannelExtraParameters[ch].ResetDetector.reslenmin = 2;
		Params->ChannelExtraParameters[ch].ResetDetector.reslenpulse = 2000;
		Params->ChannelExtraParameters[ch].ResetDetector.thrhold = -10000;
		Params->ChannelExtraParameters[ch].SaturationHoldoff = 0;

        Params->DPPParams.X770_extraparameters[ch].decK2 = 0;
		Params->DPPParams.X770_extraparameters[ch].decK3 = 0;
		Params->DPPParams.X770_extraparameters[ch].deconvolutormode = 0;
		Params->DPPParams.X770_extraparameters[ch].energyFilterMode = 0;
		Params->DPPParams.X770_extraparameters[ch].PD_thrshld1 = 500;
		Params->DPPParams.X770_extraparameters[ch].PD_thrshld2 = 450;
		Params->DPPParams.X770_extraparameters[ch].PD_winlen = 1000;
		Params->DPPParams.X770_extraparameters[ch].trigK = 30;
		Params->DPPParams.X770_extraparameters[ch].trigm = 10;
		Params->DPPParams.X770_extraparameters[ch].trigMODE = 0;

        Params->SpectrumControl[ch].SpectrumMode = CAENDPP_SpectrumMode_Energy;
        Params->SpectrumControl[ch].TimeScale = 1;
    }
    return 0;
}

// \fn      int32_t SetDAQParams(CAENDPP_DgtzParams_t *Params);
/// \brief   Configures DAQ parameters for oDAQ purposes
///
/// \param   [OUT]  Params      : Pointer to the parameters structure
///
/// \return  0 = Success; negative numbers are error codes
int32_t SetDAQParams(CAENDPP_DgtzParams_t *Params) {
    int32_t ch;
    // Channel parameters
    for (ch = 0; ch < MAX_BOARD_CHNUM; ch++) {
        // Channel parameters
        Params->DCoffset[ch] = 58950;
//        Params->PulsePolarity[ch] = CAENDPP_PulsePolarityNegative;
        Params->PulsePolarity[ch] = CAENDPP_PulsePolarityPositive;

        // DPP Parameters
        Params->DPPParams.M[ch] = 250;          // Signal Decay Time Constant [ns]
        Params->DPPParams.m[ch] = 1500;         // Trapezoid Flat Top [ns]
        Params->DPPParams.k[ch] = 8000;         // Trapezoid Rise Time [ns]
        Params->DPPParams.ftd[ch] = 1200;       // Flat Top Delay [ns]
        Params->DPPParams.a[ch] = 4;            // Trigger Filter smoothing factor
        Params->DPPParams.b[ch] = 170;          // Input Signal Rise time [ns]
        Params->DPPParams.thr[ch] = 50;         // Trigger Threshold
        Params->DPPParams.nsbl[ch] = 1;         // Number of Samples for Baseline Mean
        Params->DPPParams.nspk[ch] = 3;         // Number of Samples for Peak Mean Calculation
        Params->DPPParams.pkho[ch] = 20000;     // Peak Hold Off [ns]
        Params->DPPParams.blho[ch] = 500;       // Base Line Hold Off [ns]
        Params->DPPParams.dgain[ch] = 0;        // Digital Probe Gain
        Params->DPPParams.enf[ch] = 1.0;        // Energy Normalization Factor
        Params->DPPParams.decimation[ch] = 0;   // Decimation of Input Signal
        Params->DPPParams.trgho[ch] = 2000;     // Trigger Hold Off
    }
    return 0;
}


/// \fn      int32_t PrintChannelParameters(int32_t ch, CAENDPP_DgtzParams_t *Params);
/// \brief   Prints the given Channel Parameters
///
/// \param   [IN]   ch      : The channel to modify
/// \param   [I/O]  Params  : Pointer to the parameters structure to print
///
/// \return  0 = Success; negative numbers are user abort
int32_t PrintChannelParameters(int32_t ch, CAENDPP_DgtzParams_t *Params) {
        // Print the Configuration
        printf("\nChannel %d configuration:\n",ch);
        printf(" Decay Time\t\t\t= %.2f us\n", (float)Params->DPPParams.M[ch] / 1000.0);
        printf(" Flat Top\t\t\t= %.2f us\n", (float)Params->DPPParams.m[ch] / 1000.0);
        printf(" Trapezoid Rise Time\t\t= %.2f us\n", (float)Params->DPPParams.k[ch] / 1000.0);
        printf(" Peaking Delay\t\t\t= %.2f us\n", (float)Params->DPPParams.ftd[ch] / 1000.0);
        printf(" Smoothing Factor\t\t= %d\n", Params->DPPParams.a[ch]);
        printf(" Input Rise time\t\t= %.2f us\n", (float)Params->DPPParams.b[ch] / 1000.0);
        printf(" Threshold\t\t\t= %d\n", Params->DPPParams.thr[ch]);
        printf(" Baseline Mean\t\t\t= %d\n", Params->DPPParams.nsbl[ch]);
        printf(" Peak Mean\t\t\t= %d\n", Params->DPPParams.nspk[ch]);
        printf(" Peak Hold Off\t\t\t= %.2f us\n", (float)Params->DPPParams.pkho[ch] / 1000.0);
        printf(" Baseline Hold Off\t\t= %.2f us\n", (float)Params->DPPParams.blho[ch] / 1000.0);
        printf(" Trigger Hold Off\t\t= %.2f us\n", (float)Params->DPPParams.trgho[ch] / 1000.0);
        printf(" Digital Gain\t\t\t= %d\n", Params->DPPParams.dgain[ch]);
        printf(" Energy Normalization Factor\t= %.2f\n", Params->DPPParams.enf[ch]);
        printf(" Decimation\t\t\t= %d\n", Params->DPPParams.decimation[ch]);
        printf(" Polarity\t\t\t= %s\n", Params->PulsePolarity[ch] == CAENDPP_PulsePolarityPositive ? "POS" : "NEG");
        printf(" DCOffset\t\t\t= %.2f\n", ((float)(Params->DCoffset[ch]) / 655.35) - 50.0);
        printf("\n");
    return 0;
}

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
int32_t getBrdRange(int32_t *f, int32_t *l) {
    int32_t ret = 0;
    if (currBrd < 0) {
        *f = 0;
        *l = connectedBrdNum;
    }
    else if (currBrd < connectedBrdNum) {
        *f = currBrd;
        *l = currBrd + 1;
    }
    else { // Invalid currBrd value
        ret = - 1;
        *f = 0;
        *l = 0;
    }
    return ret;
}

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
int32_t getChRange(int32_t *f, int32_t *l) {
    int32_t ret = 0;
    if (currCh < 0) {
        *f = 0;
        *l = connectedChNum;
    }
    else if (currCh < connectedChNum) {
        *f = currCh;
        *l = currCh + 1;
    }
    else { // Invalid currCh value
        ret = - 1;
        *f = 0;
        *l = 0;
    }
    return ret;
}

/// \fn      int32_t isChannelDisabled(int32_t handle, int32_t ch);
/// \brief   Get if a channel is Enabled
///
/// \param   [IN] handle    : Handle to the Library
/// \param   [IN] ch        : Channel index to check
///
/// \return  0 if the channel is enabled, 1 if it is disabled
int32_t isChannelDisabled(int32_t handle, int32_t ch) {
    int32_t enabled;
    int32_t ret;
    ret = CAENDPP_IsChannelEnabled(handle, ch, &enabled);
    if (ret != CAENDPP_RetCode_Ok)
        return ret;
    return !enabled;
}

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
int32_t plotHisto(char *title, uint32_t *EHisto, int32_t nbins, uint32_t counts, double RT_s, double DT_s) {
    char *pltDataName = "PlotData.txt";
    if (gnuplot == NULL) {
        gnuplot = _popen(GNUPLOT_CMD, "w");
        if (gnuplot == NULL) {
//            setStatusMessage("Can't open GNUPLOT");
            return - 1;
        }
    }
    if (saveHistogram(pltDataName, EHisto, nbins, 0, 0)) { // Avoid RealTime & DeadTime dumping
//        setStatusMessage("Can't save Plot Data to file.");
        return - 2;
    }

    fprintf(gnuplot, "set ylabel 'Counts'\n");
    fprintf(gnuplot, "set xlabel 'ADC Channels'\n");
    fprintf(gnuplot, "plot '%s' title '%s: ### C = %u (RT/DT) = (%.2f/%.2f)s ###' w step linecolor 1\n", pltDataName, title, counts, RT_s, DT_s);

    fflush(gnuplot);
    return 0;
}
