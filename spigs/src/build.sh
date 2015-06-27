#g++ -g -I/usr/local/include/CAEN -L/usr/local/lib/CAEN -I. PigsDAQ.cpp spigs.cpp -lCAENDPPLib -o spigs.exe
rootcint -f PigsDict.cpp -c -p PigsGUI.h PigsStorage.h PigsEvent.h PigsLinkDef.h 


g++ -g -I/usr/local/include/CAEN -L/usr/local/lib/CAEN -I. `root-config  --cflags --libs` PigsGUI.cpp PigsDAQ.cpp spigs.cpp -lCAENDPPLib -o spigs.exe
g++  ./src/PigsDict.o ./src/PigsEvent.o ./src/PigsGUI.o ./src/PigsGUI_test.o ./src/PigsStorage.o ./src/guitest.o  -lCAENDPPLib -L/usr/local/lib/CAEN -Xlinker -L/usr/local/lib `root-config --libs --glibs --cflags --ldflags --auxlibs --auxcflags` -pthread -lm -ldl --shared -fPIC -o lib.so 
