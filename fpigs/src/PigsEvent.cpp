/*
 * PigsEvent.cpp
 * Class to hold the measurement information
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#include <PigsEvent.h>

PigsEvent::PigsEvent() {
    // constructor
    uint8_t i;
    for (i=0;i<4;i++) {
        spectrum[i] = 0;
        realTime[i] = 0;
        deadTime[i] = 0;
        goodCounts[i] = 0;
        totCounts[i] = 0;
        countsPerSecond[i] = 0;
        scaleFactor[i] = 0;
        energySum[i] = 0;
    }
    acqTime = 0;
}

PigsEvent::~PigsEvent() {
    // destructor
    uint8_t i;
    for (i=0;i<4;i++) {
        if(spectrum[i]) delete spectrum[i];
    }
}

