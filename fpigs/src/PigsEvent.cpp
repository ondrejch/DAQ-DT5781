/*
 * PigsEvent.cpp
 * Class to hold the measurement information
 * Four channel version
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include <PigsEvent.h>

PigsEvent::PigsEvent() {
    // constructor
    uint8_t i;
    for (i=0;i<4;i++) {
        spectrum[i]         = 0;
        realTime[i]         = 0;
        deadTime[i]         = 0;
        goodCounts[i]       = 0;
        totCounts[i]        = 0;
        countsPerSecond[i]  = 0;
        scaleFactor[i]      = 0;
        detectorResponse[i] = 0;
    }
    acqTime     = 0;
    arrowAngle  = 0;
}

PigsEvent::~PigsEvent() {
    // destructor
    uint8_t i;
    for (i=0;i<4;i++) {
        if(spectrum[i]) delete spectrum[i];
    }
}

const char* PigsEvent::GetMeasurementDate() const {
    // Extracts measurement date from the histogram title
   return std::string(spectrum[0]->GetTitle(),5,22).c_str();
}

void PigsEvent::Print(Option_t *) const {
    // Prints information about the measurement
    std::cout << "*** Printing information of " << this->GetName() <<" ***"<< std::endl;
    std::cout << "Measurement time stamp: " << this->GetMeasurementDate();
    std::cout << std::endl << "Set acquisition time [s]:\t" << acqTime;
    std::cout << std::endl << "Arrow pointing angle [deg]:\t" << arrowAngle;
    uint8_t i;
    std::cout << std::endl << "Channel:   \t0\t1\t2\t3";
    std::cout << std::endl << "Real time [ms]:";
    for (i=0;i<4;i++) std::cout << "\t" << realTime[i]/1000000;
    std::cout << std::endl << "Dead time [ns]:";
    for (i=0;i<4;i++) std::cout << "\t" << deadTime[i]/1000;

    std::cout << std::endl << "Good counts: ";
    for (i=0;i<4;i++) std::cout << "\t" << goodCounts[i];
    std::cout << std::endl<<"Total counts: ";
    for (i=0;i<4;i++) std::cout << "\t" << totCounts[i];

    std::cout << std::endl << "Gain factor:";
    for (i=0;i<4;i++) std::cout << "\t" << scaleFactor[i];
    std::cout << std::endl << "Det. signal:";
    for (i=0;i<4;i++) std::cout << "\t" << detectorResponse[i];
    std::cout << std::endl;
}

