Simple DAQ system for DT5781
============================
Ondrej Chvala <ochvala@utk.edu>


ABOUT:

Four-channel Position Indicating Gamma Sensor (F-PIGS) 
is a data acquisition system GUI for DT5781.
It is written in C++ and uses the ROOT/C++ framework. 
The channel parameters are optimized for an NaI scintillator. 


USAGE:

First initialize the DAQ by clicking "Init DAQ".
Then set acquisition time in Config tab.
Start DAQ by clicking "Start DAQ" button.
To stop the DAQ loop, press "Stop DAQ" button - this will finish 
the current loop and stop. 
"Exit DAQ" button will interrupt the data acquisition without 
waiting to finish the loop, and quit the program. Closing the program
window is equivalent to pressing the "Exit DAQ" button.


INSTALLATION:

Compile the project by typing "make". 
All sources are in src/.
Object files and ROOT class dictionary are created in obj/.
Library for output file reading: lib/fPigsDict.so.  
The linked binary executable: bin/fpigs.exe.


OUTPUT DATA:

The DAQ produces output file out-<date>_<time>.root.
It contains a tree with events, each event corresponding to 
a measurement: histogram spectrum, realTime, deadTime,
goodCounts, totCounts, countsPerSecond. 
To read the file, first load the library:

    root [0] .L lib/fPigsDict.so   

There is an example data file out-test-fpigs.root 
and a ROOT macro  read_example_fpigs.C in the examples/ 
directory. The macro shows how to process the measurements. 
Please see comments in the macro for detailed explanation.  

    
TERMS OF USE:

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation. This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
software, documentation and results solely at his own risk.

