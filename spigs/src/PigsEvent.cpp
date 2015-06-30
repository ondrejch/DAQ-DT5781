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
    spectrum = 0;
    realTime = 0;
    deadTime = 0;
    goodCounts = 0;
    totCounts = 0;
    countsPerSecond = 0;
}

PigsEvent::~PigsEvent() {
    // destructor
    if(spectrum) delete spectrum;
}

