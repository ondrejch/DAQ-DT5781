/*
 * PigsEvent.h
 *
 *  Created on: Jun 26, 2015
 *      Author: Ondrej Chvala <ochvala@utk.edu>
 */

#ifndef PIGSEVENT_H_
#define PIGSEVENT_H_

#include <TObject.h>
#include <TH1D.h>

class PigsEvent: public TObject {
public:
    PigsEvent();
    virtual ~PigsEvent();

    TH1D *spectrum;
    unsigned long realTime;
    unsigned long deadTime;
    unsigned int  goodCounts;
    unsigned int  totCounts;
    double countsPerSecond;

    ClassDef(PigsEvent, 0);
};

#endif /* PIGSEVENT_H_ */
