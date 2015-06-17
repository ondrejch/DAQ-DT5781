//============================================================================
// Name        : spigs.cpp
// Author      : Ondrej Chvala <ochvala@utk.edu>
// Version     :
// Copyright   : GNU/GPL
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <PigsDAQ.h>

using namespace std;


int main(void) {
	int ret = 0;
	cout << "Welcome to SPIGS DAQ!" << endl;
	PigsDAQ *daq  = PigsDAQ::getInstance();
    ret = daq->BasicInit();


    ret = daq->EndLibrary();
	return ret;
}
