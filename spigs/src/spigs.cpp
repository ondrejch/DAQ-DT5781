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

PigsDAQ *daq;

int main(void) {
	daq  = new PigsDAQ();
	cout << "Hello World" << endl; /* prints Hello World */
	delete daq;
	return 0;
}
