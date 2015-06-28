########################################################################
#
#              --- CAEN SpA - Computing Division ---
#
#   CAENDPPLib Software Project
#
#   Created  :  November    2013      (Rel. 1.3.0)
#
#   Auth: F. Pepe, modified by Ondrej Chvala
#
########################################################################
ARCH    =   `uname -m`

EXEDIR  =  ./bin/$(ARCH)
BASENM  =  odaq
EXEEXT  =  bin
EXENAME =  $(BASENM).$(EXEEXT)
EXE	    =  $(EXEDIR)/$(EXENAME)

CC	=	gcc

#COPTS	=	-fPIC -DLINUX -O0 -g -pg -fprofile-arcs -ftest-coverage
COPTS	=	-fPIC -DLINUX -O0 -g

DEPLIBS	=	-lCAENDPPLib -lm -lc -lrt -lpthread -lgcov

LIBS	=	-L/usr/lib -L/usr/local/lib/CAEN -L/usr/lib/gcc/x86_64-linux-gnu/4.9/

INCLUDEDIR = -I/usr/include -I./include

INSTALLDIR =	/usr/bin

OBJS	=	./src/oDAQ.o ./src/Functions.o

INCLUDES = /usr/include/CAENDPPLib.h /usr/include/CAENDPPLibTypes.h ./include/*

#########################################################################

all	:	$(EXE)

Debug: $(EXE)

Release: $(EXE)

cleanDebug: clean

clean	:
		/bin/rm -f $(OBJS) $(EXE)

install	:	$(EXE)
		/bin/rm -f $(INSTALLDIR)/$(EXENAME)
		install $(EXE) $(INSTALLDIR)
		ldconfig

$(EXE)	:	$(OBJS)
		/bin/rm -f $(EXEDIR)/$(BASENM).*
		if [ ! -d bin/$(ARCH) ]; then mkdir -p bin/$(ARCH); fi
		$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(LIBS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.c
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<
