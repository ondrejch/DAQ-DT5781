/*
 * Position Indicating Gamma Sensing system
 * Currently assumes single channel operation on channel 0
 *
 *
 * PigsDAQ.cpp
 *
 *  Created on: Jun 12, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 *
 */

#include "PigsDAQ.h"

bool PigsDAQ::instantiated   = false;
PigsDAQ * PigsDAQ::instance  = 0;
const int32_t PigsDAQ::fVerbose = 1;

//------------------------------------------------
PigsDAQ * PigsDAQ::getInstance(){
	if(!instantiated) {
		instance = new PigsDAQ();
		instantiated = true;
		std::cout<<"<PigsDAQ> Instantiated" << std::endl;
	}
	return instance ;
}

//PigsDAQ::PigsDAQ(){
//ch=handle=brd=0;
//connParam=info=dgtzParams=acqMode=iputLevel=StopCriteria=StopCriteriaValue=0;
//}

int32_t PigsDAQ::BasicInit() {
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
	fErrCode = 0;
	// Setting the digitizer and DAQ parameters is safe, no need to error check.
	// Init digitizer parameters to default value
	fErrCode = instance->InitDgtzParams();
	// Custom digitizer parameters
	fErrCode += instance->SetDgtzParams();
	// Custom DAQ parameters
	fErrCode += instance->SetDAQParams();
	// Time keeping
	fDt = 0;
	// No histogram yet
	fCurrHist = 0;
	// reset counters
	totCounts = realTime = deadTime = 0;
	countsPerSecond = 0;
	// Allocate the histogram to the given number of bins.
	h1 = (uint32_t *)calloc(MAX_HISTO_NBINS, sizeof(uint32_t));
	if (h1 == 0) {
		std::cerr<<__PRETTY_FUNCTION__<<" Cannot allocate h1"<<std::endl;
		throw std::bad_alloc(); //std::exception();
	}
	return fErrCode;
}

int32_t PigsDAQ::SetDAQParams(){
	/// \fn      int32_t SetDAQParams();
	/// \brief   Configures DAQ parameters for PIGS purposes.
	///			 Sets everything which is not in the dgtzParams structure
	///
	/// \param   No parameters yet, it could be implemented better.
	///
	/// \return  0 = Success; negative numbers are error codes
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

	// Set the Acquisition Mode
	acqMode = CAENDPP_AcqMode_Histogram;    // For Histogram mode (no waveforms)
	//acqMode = CAENDPP_AcqMode_Waveform;   // For Oscilloscope mode (waves + histogram)
	iputLevel = CAENDPP_InputRange_1_0Vpp;  // Channel input level - could be per channel
	usecSleepPollDAQ = 50000;				// [ns] DAQ acquisition poll
	// Set stop criteria
	StopCriteria = CAENDPP_StopCriteria_RealTime; 	// Elapsed real time
	//StopCriteriaValue = 600L*1000000000L;      	// [ns] Run for 600 seconds
	StopCriteriaValue =  10L*1000000000L;      		// [ns] Run for  10 seconds
	//StopCriteriaValue =   1000000000;// 1000*1000*1000; // 1 sec [ns] Run for 600 seconds
	return 0;
}

int32_t PigsDAQ::InitDPPLib() {
	// Initialize the DPP library
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
	fErrCode = CAENDPP_InitLibrary(&handle); // The handle will be used to command the library
	if(fErrCode != CAENDPP_RetCode_Ok)
		std::cerr<<"Problem initializing the library: "<< decodeError(codeStr,fErrCode) << std::endl;
	return fErrCode;
}

int32_t PigsDAQ::AddBoardUSB() {
	// Adds board 0 & prints board information
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
	// The following is for direct USB connection
	connParam.LinkType = CAENDPP_USB;
	connParam.LinkNum = 0;              // This defines the USB port to use and must increase with board number; ex: for board 3 must be 3
	connParam.ConetNode = 0;            // This MUST be 0
	connParam.VMEBaseAddress = 0x0;     // For direct connection the address must be 0
	brd = 0;							// We only have one board

	// Add board to the Library
	fErrCode = CAENDPP_AddBoard(handle, connParam, &brd);
	if(fErrCode != CAENDPP_RetCode_Ok) {
		std::cerr<<"Error adding board with selected connection parameters: "<< decodeError(codeStr,fErrCode) << std::endl;
		return fErrCode;
	}

	// Get Board Info
	fErrCode  = CAENDPP_GetDPPInfo(handle,	brd, &info);
	if(fErrCode != CAENDPP_RetCode_Ok) {
		std::cerr<<"Error getting board info: "<< decodeError(codeStr,fErrCode) << std::endl;
		return fErrCode;
	}
	PrintBoardInfo();

	return fErrCode;
}


void PigsDAQ::PrintBoardInfo() {
	std::cout << "Board "<< info.ModelName << ", Serial#: " << info.SerialNumber << std::endl;
	std::cout << "  Channels: " << info.Channels << ", License: " << info.License << std::endl;
	std::cout << "  Firmware AMC: " << info.AMC_FirmwareRel << ", ROC" << info.ROC_FirmwareRel << std::endl;
}

int32_t PigsDAQ::ConfigureBoard() {
	// Configures board 0
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

	fErrCode  = CAENDPP_SetBoardConfiguration(handle, 0, acqMode, dgtzParams);
	if(fErrCode != CAENDPP_RetCode_Ok) {
		std::cerr<<"Can't configure board "<<info.ModelName << ", Serial#: " << info.SerialNumber
				<< ": " << decodeError(codeStr,fErrCode) << std::endl;
		return fErrCode;
	}
	if(fVerbose && fErrCode == CAENDPP_RetCode_Ok) {
		std::cout << "Board "<< info.ModelName << ", Serial#: " << info.SerialNumber <<" configured."<< std::endl;
	}
	return fErrCode;
}

void PigsDAQ::PrintChannelParameters(int32_t ch) {
	/// \fn      void PrintChannelParameters(int32_t ch);
	/// \brief   Prints the given Channel Parameters
	///
	/// \param   [IN]   ch      : The channel to modify
	///
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

	// Print the Configuration
	printf("\nChannel %d configuration:\n",ch);
	printf(" Decay Time\t\t\t= %.2f us\n", (float)dgtzParams.DPPParams.M[ch] / 1000.0);
	printf(" Flat Top\t\t\t= %.2f us\n", (float)dgtzParams.DPPParams.m[ch] / 1000.0);
	printf(" Trapezoid Rise Time\t\t= %.2f us\n", (float)dgtzParams.DPPParams.k[ch] / 1000.0);
	printf(" Peaking Delay\t\t\t= %.2f us\n", (float)dgtzParams.DPPParams.ftd[ch] / 1000.0);
	printf(" Smoothing Factor\t\t= %d\n", dgtzParams.DPPParams.a[ch]);
	printf(" Input Rise time\t\t= %.2f us\n", (float)dgtzParams.DPPParams.b[ch] / 1000.0);
	printf(" Threshold\t\t\t= %d\n", dgtzParams.DPPParams.thr[ch]);
	printf(" Baseline Mean\t\t\t= %d\n", dgtzParams.DPPParams.nsbl[ch]);
	printf(" Peak Mean\t\t\t= %d\n", dgtzParams.DPPParams.nspk[ch]);
	printf(" Peak Hold Off\t\t\t= %.2f us\n", (float)dgtzParams.DPPParams.pkho[ch] / 1000.0);
	printf(" Baseline Hold Off\t\t= %.2f us\n", (float)dgtzParams.DPPParams.blho[ch] / 1000.0);
	printf(" Trigger Hold Off\t\t= %.2f us\n", (float)dgtzParams.DPPParams.trgho[ch] / 1000.0);
	printf(" Digital Gain\t\t\t= %d\n", dgtzParams.DPPParams.dgain[ch]);
	printf(" Energy Normalization Factor\t= %.2f\n", dgtzParams.DPPParams.enf[ch]);
	printf(" Decimation\t\t\t= %d\n", dgtzParams.DPPParams.decimation[ch]);
	printf(" Polarity\t\t\t= %s\n", dgtzParams.PulsePolarity[ch] == CAENDPP_PulsePolarityPositive ? "POS" : "NEG");
	printf(" DCOffset\t\t\t= %.2f\n", ((float)(dgtzParams.DCoffset[ch]) / 655.35) - 50.0);
	printf("\n");
}


int32_t PigsDAQ::StopAcquisition(int32_t ch) {
	fErrCode = CAENDPP_StartAcquisition(handle, ch);
	// Stop Acquisition for channel 0
	fErrCode = CAENDPP_StopAcquisition(handle, ch);
	if (fErrCode != CAENDPP_RetCode_Ok) {
		printf ("Error Stopping Acquisition for channel %d: %s\n", ch, decodeError(codeStr, fErrCode));
	} else {
		printf("Acquisition Stopped for channel %d\n", ch);
	}
	return fErrCode;
}

int32_t PigsDAQ::AcquisitionLoop() {
	// Currently does only one cycle in the loop...
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

	// reset counters
	totCounts = realTime = deadTime = 0;
	countsPerSecond = 0;

	// Set starting date time
	if(fDt) delete fDt;
	fDt = new TDatime();

	// Start Acquisition for channel 0
	ch = 0;
	fErrCode = CAENDPP_StartAcquisition(handle, ch);
	if (fErrCode != CAENDPP_RetCode_Ok) {
		printf("Error Starting Acquisition for channel %d: %s\n", ch, decodeError(codeStr, fErrCode));
		return fErrCode;
	} else {
		if(fVerbose) printf("Acquisition Started for channel %d\n", ch);
	}

	do {
		usleep(usecSleepPollDAQ);		// waits to poll DT5781
		CAENDPP_IsChannelAcquiring(handle, ch, &isAcquiring );
		if(fVerbose>9) printf("  -- ch %d acq status %d\n", ch, isAcquiring);
	} while (isAcquiring);

	// Get The Histogram
	fErrCode = CAENDPP_GetCurrentHistogram(handle, ch, h1, &goodCounts, &realTime, &deadTime, &isAcquiring);
	if (fErrCode != CAENDPP_RetCode_Ok) {
		printf("ERROR: Can't get current Histogram for ch %d: %s\n", ch, decodeError(codeStr, fErrCode));
		return fErrCode;
	}
	// Get the histogram bin number
	fErrCode = CAENDPP_GetHistogramSize(handle, ch, HISTO_IDX_CURRENT, &h1NBins);
	if (fErrCode != CAENDPP_RetCode_Ok) {
		printf("ERROR: Can't get current Histogram size for ch %d: %s\n", ch, decodeError(codeStr, fErrCode));
		return fErrCode;
	}
	totCounts       = (uint32_t)(goodCounts * (1.0 + (double)deadTime / (double)(realTime)));
	countsPerSecond = (realTime - deadTime) > 0 ? (double)goodCounts / (double)((realTime - deadTime) / 1000000000.0) : 0;
	return fErrCode;
}

int32_t PigsDAQ::RefreshCurrHist() {
	// creates TH1D from h1
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

	fAcqDate=Form("%06d %02d:%02d:%02d",fDt->GetDate(),fDt->GetHour(),fDt->GetMinute(),fDt->GetSecond());
	if(fCurrHist) delete fCurrHist;
	fCurrHist = new TH1D("fCurrHist", Form("ch0: %s",fAcqDate.Data()), h1NBins, 0, h1NBins-1);
	fCurrHist->SetXTitle("ADC channel");
	fCurrHist->SetYTitle("Counts");
	if(fCurrHist) {
		for (int32_t i=0; i < h1NBins; i++) { // copy over data from h1
			fCurrHist->SetBinContent(i+1,h1[i]);
		}
		return 0;
	} else {
		std::cerr << "Error creating fCurrHist!" << std::endl;
	}
	return 999;
}

void PigsDAQ::PrintAcquisotionInfo() {
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
	printf("CH%d\n", ch);
	printf(" RT\t= %.3f s\n", realTime / 1000000000.0);
	printf(" DT\t= %.3f ms\n", deadTime / 1000000.0);
	printf(" Good\t= %u counts\n", goodCounts);
	printf(" Tot.\t= %u counts\n", totCounts);
	printf(" ICR\t= %.3f counts/s\n", countsPerSecond);
}

int32_t PigsDAQ::ConfigureChannel(int32_t ch) {
	// Configures channel with parameters set in the class data
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
	if (isChannelDisabled(ch)) {
		printf("Warning: Channel %d disabled\n",ch);
		return 1;
	}
	fErrCode = CAENDPP_SetInputRange(handle, ch, iputLevel);
	if (fErrCode == CAENDPP_RetCode_Ok)  {
		printf("Input range for channel %d set to %d\n", ch, iputLevel);
	} else {
		printf("Error setting input range\n");
		return fErrCode;
	}
	fErrCode = CAENDPP_SetStopCriteria(handle, ch, StopCriteria, StopCriteriaValue);
	if (fErrCode == CAENDPP_RetCode_Ok) {
		printf("Stop Criteria successfully set: %d, %lu\n", StopCriteria, StopCriteriaValue);
	} else {
		printf("Can't set Stop Criteria for channel %d: %s\n", ch, decodeError(codeStr, fErrCode));
		return fErrCode;
	}
	return fErrCode;
}

int32_t PigsDAQ::isChannelDisabled(int32_t ch) {
	/// \fn      int32_t isChannelDisabled(int32_t handle, int32_t ch);
	/// \brief   Get if a channel is Enabled
	///
	/// \param   [IN] handle    : Handle to the Library
	/// \param   [IN] ch        : Channel index to check
	///
	/// \return  0 if the channel is enabled, 1 if it is disabled
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
	int32_t enabled;
	int32_t ret;
	ret = CAENDPP_IsChannelEnabled(handle, ch, &enabled);
	if (ret != CAENDPP_RetCode_Ok)
		return ret;
	return !enabled;
}

int32_t PigsDAQ::SetDgtzParams() {
	/// \fn      int32_t SetDgtzParams(CAENDPP_DgtzParams_t *Params);
	/// \brief   Configures Dgtz parameters for PIGS purposes
	///
	/// \param   [OUT]  Params      : Pointer to the parameters structure
	///
	/// \return  0 = Success; negative numbers are error codes
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

	// The channel mask is used to set the enabled Acquisition Input Channels
	// Binary mask in hex, 0 to F
	dgtzParams.ChannelMask = 0x1;	//  0x1 = channel 0 enabled

	int32_t ch;
	// Channel parameters
	for (ch = 0; ch < MAX_BOARD_CHNUM; ch++) {
		// Channel parameters
		dgtzParams.DCoffset[ch] = 58950;
		//        dgtzParams.PulsePolarity[ch] = CAENDPP_PulsePolarityNegative;
		dgtzParams.PulsePolarity[ch] = CAENDPP_PulsePolarityPositive;

		// DPP Parameters
		dgtzParams.DPPParams.M[ch] = 250;          // Signal Decay Time Constant [ns]
		dgtzParams.DPPParams.m[ch] = 1500;         // Trapezoid Flat Top [ns]
		dgtzParams.DPPParams.k[ch] = 8000;         // Trapezoid Rise Time [ns]
		dgtzParams.DPPParams.ftd[ch] = 1200;       // Flat Top Delay [ns]
		dgtzParams.DPPParams.a[ch] = 4;            // Trigger Filter smoothing factor
		dgtzParams.DPPParams.b[ch] = 170;          // Input Signal Rise time [ns]
		dgtzParams.DPPParams.thr[ch] = 50;         // Trigger Threshold
		dgtzParams.DPPParams.nsbl[ch] = 1;         // Number of Samples for Baseline Mean
		dgtzParams.DPPParams.nspk[ch] = 3;         // Number of Samples for Peak Mean Calculation
		dgtzParams.DPPParams.pkho[ch] = 20000;     // Peak Hold Off [ns]
		dgtzParams.DPPParams.blho[ch] = 500;       // Base Line Hold Off [ns]
		dgtzParams.DPPParams.dgain[ch] = 0;        // Digital Probe Gain
		dgtzParams.DPPParams.enf[ch] = 1.0;        // Energy Normalization Factor
		dgtzParams.DPPParams.decimation[ch] = 0;   // Decimation of Input Signal
		dgtzParams.DPPParams.trgho[ch] = 2000;     // Trigger Hold Off
	}
	return 0;
}

int32_t PigsDAQ::InitDgtzParams() {
	/// \fn      int32_t InitDgtzParams(CAENDPP_DgtzParams_t *Params);
	/// \brief   Initialize the configuration parameters to their default value
	///
	/// \param   [OUT]  Params      : Pointer to the parameters structure to fill
	///
	/// \return  0 = Success; negative numbers are error codes
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

	// set listMode default parameters
	dgtzParams.ListParams.enabled = FALSE;
	dgtzParams.ListParams.saveMode = CAENDPP_ListSaveMode_FileBinary;
	strcpy(dgtzParams.ListParams.fileName, "UNNAMED");
	dgtzParams.ListParams.maxBuffNumEvents = 0;
	dgtzParams.ListParams.saveMask = 0xF;

	// default board parameters
	dgtzParams.ChannelMask = 0; // it will be filled later
	dgtzParams.EventAggr = 0;
	dgtzParams.IOlev = CAENDPP_IOLevel_NIM;

	// Generic Writes to Registers
	dgtzParams.GWn = 0;                                    // Number of Generic Writes
	memset(dgtzParams.GWaddr, 0, MAX_GW*sizeof(uint32_t)); // List of addresses (length = 'GWn')
	memset(dgtzParams.GWdata, 0, MAX_GW*sizeof(uint32_t)); // List of datas (length = 'GWn')
	memset(dgtzParams.GWmask, 0, MAX_GW*sizeof(uint32_t)); // List of masks (length = 'GWn')

	// Waveform parameters default settings
	dgtzParams.WFParams.dualTraceMode = 1;
	dgtzParams.WFParams.vp1 = CAENDPP_PHA_VIRTUALPROBE1_Input;
	dgtzParams.WFParams.vp2 = CAENDPP_PHA_VIRTUALPROBE2_TrapBLCorr;
	dgtzParams.WFParams.dp1 = CAENDPP_PHA_DigitalProbe1_Armed;
	dgtzParams.WFParams.dp2 = CAENDPP_PHA_DigitalProbe2_Trigger;
	dgtzParams.WFParams.recordLength = (int32_t)(8192);
	dgtzParams.WFParams.preTrigger = (int32_t)(100);
	dgtzParams.WFParams.probeSelfTriggerVal = 150;
	dgtzParams.WFParams.probeTrigger = CAENDPP_PHA_PROBETRIGGER_MainTrig;

	// Channel parameters
	for (ch = 0; ch < MAX_BOARD_CHNUM; ch++) {
		// Channel parameters
		dgtzParams.DCoffset[ch] = 7864;
		dgtzParams.PulsePolarity[ch] = CAENDPP_PulsePolarityPositive;

		// Coincidence parameters between channels
		dgtzParams.CoincParams[ch].CoincChMask = 0x0;
		dgtzParams.CoincParams[ch].CoincLogic = CAENDPP_CoincLogic_None;
		dgtzParams.CoincParams[ch].CoincOp = CAENDPP_CoincOp_OR;
		dgtzParams.CoincParams[ch].MajLevel = 0;
		dgtzParams.CoincParams[ch].TrgWin = 0;

		// DPP Parameters
		dgtzParams.DPPParams.M[ch] = 47500;        // Signal Decay Time Constant
		dgtzParams.DPPParams.m[ch] = 5000;         // Trapezoid Flat Top
		dgtzParams.DPPParams.k[ch] = 5000;         // Trapezoid Rise Time
		dgtzParams.DPPParams.ftd[ch] = 2500;       // Flat Top Delay
		dgtzParams.DPPParams.a[ch] = 4;            // Trigger Filter smoothing factor
		dgtzParams.DPPParams.b[ch] = 200;          // Input Signal Rise time
		dgtzParams.DPPParams.thr[ch] = 1500;       // Trigger Threshold
		dgtzParams.DPPParams.nsbl[ch] = 3;         // Number of Samples for Baseline Mean
		dgtzParams.DPPParams.nspk[ch] = 0;         // Number of Samples for Peak Mean Calculation
		dgtzParams.DPPParams.pkho[ch] = 0;         // Peak Hold Off
		dgtzParams.DPPParams.blho[ch] = 1000;      // Base Line Hold Off
		dgtzParams.DPPParams.dgain[ch] = 0;        // Digital Probe Gain
		dgtzParams.DPPParams.enf[ch] = 1.0;        // Energy Normalization Factor
		dgtzParams.DPPParams.decimation[ch] = 0;   // Decimation of Input Signal
		dgtzParams.DPPParams.trgho[ch] = 100;      // Trigger Hold Off
		dgtzParams.DPPParams.twwdt[ch] = 0;        //
		dgtzParams.DPPParams.trgwin[ch] = 0;       //

		// Parameters for X770
		dgtzParams.ChannelExtraParameters[ch].analogPath = 0;
		dgtzParams.ChannelExtraParameters[ch].CRgain = 0;
		dgtzParams.ChannelExtraParameters[ch].InputImpedance = 0;
		dgtzParams.ChannelExtraParameters[ch].PRDSgain = 0;
		dgtzParams.ChannelExtraParameters[ch].ResetDetector.EnableResetDetector = 0;
		dgtzParams.ChannelExtraParameters[ch].ResetDetector.thrhold = 100;
		dgtzParams.ChannelExtraParameters[ch].ResetDetector.reslenmin = 2;
		dgtzParams.ChannelExtraParameters[ch].ResetDetector.reslenpulse = 2000;
		dgtzParams.ChannelExtraParameters[ch].ResetDetector.thrhold = -10000;
		dgtzParams.ChannelExtraParameters[ch].SaturationHoldoff = 0;

		dgtzParams.DPPParams.X770_extraparameters[ch].decK2 = 0;
		dgtzParams.DPPParams.X770_extraparameters[ch].decK3 = 0;
		dgtzParams.DPPParams.X770_extraparameters[ch].deconvolutormode = 0;
		dgtzParams.DPPParams.X770_extraparameters[ch].energyFilterMode = 0;
		dgtzParams.DPPParams.X770_extraparameters[ch].PD_thrshld1 = 500;
		dgtzParams.DPPParams.X770_extraparameters[ch].PD_thrshld2 = 450;
		dgtzParams.DPPParams.X770_extraparameters[ch].PD_winlen = 1000;
		dgtzParams.DPPParams.X770_extraparameters[ch].trigK = 30;
		dgtzParams.DPPParams.X770_extraparameters[ch].trigm = 10;
		dgtzParams.DPPParams.X770_extraparameters[ch].trigMODE = 0;

		dgtzParams.SpectrumControl[ch].SpectrumMode = CAENDPP_SpectrumMode_Energy;
		dgtzParams.SpectrumControl[ch].TimeScale = 1;
	}
	return 0;
}

char * PigsDAQ::decodeError(char *dest, int32_t code) {
	/// \fn      char *decodeError(char *dest, int32_t ret);
	/// \brief   Decodes the given error code into a message
	///
	/// \param   [IN] dest   : the string to be filled
	/// \param   [IN] ret    : the error code
	///
	/// \return  a pointer to the string if success, otherwise NULL
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;

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
	default: strncpy(dest, "UNKNOWN Error", nc); break;
	}
	strcat(dest, codeStr);
	return dest;
}

int32_t PigsDAQ::EndLibrary() {
	// Close the Library
	if(fVerbose) std::cout<<__PRETTY_FUNCTION__ << std::endl;
	fErrCode =  CAENDPP_EndLibrary(handle);
	if(fErrCode != CAENDPP_RetCode_Ok) {
		std::cerr<<"Error closing the library: "<< decodeError(codeStr,fErrCode) << std::endl;
	}
	return fErrCode;
}

// getters & setters
TH1D   * PigsDAQ::getCurrHist() const { return fCurrHist; }
void     PigsDAQ::setCurrHist(TH1D *currHist) { fCurrHist = currHist; }
double   PigsDAQ::getCountsPerSecond() const { return countsPerSecond; }
uint64_t PigsDAQ::getRealTime() const { return realTime; }
uint64_t PigsDAQ::getDeadTime() const { return deadTime; }
uint32_t PigsDAQ::getTotCounts() const { return totCounts; }
uint32_t PigsDAQ::getGoodCounts() const { return goodCounts; }

CAENDPP_StopCriteria_t PigsDAQ::getStopCriteria() const {
    return StopCriteria;
}

void PigsDAQ::setStopCriteria(CAENDPP_StopCriteria_t stopCriteria) {
    StopCriteria = stopCriteria;
}

uint64_t PigsDAQ::getStopCriteriaValue() const {
    return StopCriteriaValue;
}
void PigsDAQ::setStopCriteriaValue(uint64_t stopCriteriaValue) {
    StopCriteriaValue = stopCriteriaValue;
}










