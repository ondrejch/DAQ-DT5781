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
#include <TH1F.h>

class PigsEvent : public TObject {
public:
    PigsEvent();
    virtual ~PigsEvent();

    TH1F    *spectrum[4];           // data about the four detectors
    uint64_t realTime[4];           // Real time from DAQ [ns]
    uint64_t deadTime[4];           // Dead time from DAQ [ns]
    uint32_t goodCounts[4];         // Counts as measured by DAQ
    uint32_t totCounts[4];          // DAQ counts scaled up by (1+ deadTime)/realTime
    Float_t  countsPerSecond[4];    // goodCounts/(realTime-deadTime)
    Float_t  scaleFactor[4];        // scaling of the integral
    Float_t  detectorResponse[4];   // signals used to calculate the arrow pointing angle

    Float_t  acqTime;               // acquisition time set by DAQ
    Float_t  arrowAngle;            // angle of the arrow as calculated by the fuzzy logic algorithm

    ClassDef(PigsEvent, 1)
};

#endif /* PIGSEVENT_H_ */
