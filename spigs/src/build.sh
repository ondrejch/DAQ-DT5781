#g++ -g -I/usr/local/include/CAEN -L/usr/local/lib/CAEN -I. PigsDAQ.cpp spigs.cpp -lCAENDPPLib -o spigs.exe
rootcint -f PigsDict.cpp -c -p PigsGUI.h PigsStorage.h PigsEvent.h PigsLinkDef.h 


g++ -g -I/usr/local/include/CAEN -L/usr/local/lib/CAEN -I. `root-config  --cflags --libs` PigsGUI.cpp PigsDAQ.cpp spigs.cpp -lCAENDPPLib -o spigs.exe
