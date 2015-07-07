/*
 * PigsEvent.h
 * Class to hold the measurement information
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#ifndef PIGSEVENT_H
#define PIGSEVENT_H

#include <stdint.h>
#include <TObject.h>
#include <TH1D.h>

class PigsEvent : public TObject {
public:
    PigsEvent();
    virtual ~PigsEvent();

    TH1D *spectrum[4];              // data about the four detectors
    uint64_t realTime[4];
    uint64_t deadTime[4];
    uint32_t totCounts[4];
    uint32_t goodCounts[4];
    Float_t countsPerSecond[4];
    Float_t scaleFactor[4];        // scaling of the integral
    Float_t energySum[4];

    Float_t acqTime;               // acquisition time set by DAQ

    ClassDef(PigsEvent, 1);
};

#endif /* PIGSEVENT_H_ */
