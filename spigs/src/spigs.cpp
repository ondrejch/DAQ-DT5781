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
	PigsDAQ *daq  = PigsDAQ::getInstance();
	cout << "Hello World" << endl; /* prints Hello World */
    daq->BasicInit();

	return 0;
}
