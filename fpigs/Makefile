##########################################################################
# *** fPIGS makefile ***
# 
# Ondrej Chvala <ochvala@utk.edu>
# GNU/GPL
#
# For debug build use: make DEBUG=1
##########################################################################


# by default, build Release
DEBUG=0
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS = -g
else
    CFLAGS = -O3
endif

##########################################################################

CC = g++

# directory with sources
SRCDIR = src
# temporary build files
OBJDIR = obj
# produced binary
EXEDIR = bin
# produced library
LIBDIR = lib

#COPTS = -fPIC -DLINUX -Wall $(shell root-config --cflags) -I/usr/local/include/CAEN 
COPTS = -fPIC -DLINUX -Wall -Wno-write-strings -Wno-unused-function $(shell root-config --cflags) -I/usr/local/include/CAEN 

LIBS = $(shell root-config --libs --glibs) -L/usr/local/lib/CAEN -lCAENDPPLib

CFLAGS += -Wall -O2 -I$(SRCDIR)
VPATH = src
LDFLAGS= 

_FUZZYDEF = "${SRCDIR}/fismain.fis"

DEPS = $(wildcard $(SRCDIR)/*.h)
_SRCS = $(wildcard $(SRCDIR)/*.cpp) 
_OBJS = $(_SRCS:.cpp=.o) 

_FUZZY_SRCS = $(wildcard ${SRCDIR}/*.c )
_FUZZY_OBJS = $(_FUZZY_SRCS:.c=.o)

TESTNAME = test_fis
TEST_SRC = $(SRCDIR)/$(TESTNAME).c
TESTEXE = ${EXEDIR}/${TESTNAME}.exe

# Don't include test files in sources
_FUZZY_SRCS := $(filter-out $(TEST_SRC), $(_FUZZY_SRCS))

OBJS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(_OBJS))
FUZZY_OBJS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(_FUZZY_OBJS))

DICT = fPigsDict
DICTSUF = cpp
DICTSRC = $(DICT).$(DICTSUF)

EXEC1NAME = fpigs
EXEC1 = $(EXEDIR)/$(EXEC1NAME).exe

.PHONY: clean
#.NOTPARALLEL: $(DICTSRC)

##########################################################################

all: $(FUZZY_OBJS) $(OBJS) $(DICTSRC) $(EXEC1) $(TESTEXE) 

# compile the fuzzy logic processor objects first
$(OBJDIR)/%.o: %.c ${DEPS}
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
	@echo [${CC}] Compiling C object $< 
	@$(CC) -c $(CFLAGS) $(COPTS) $< -o $@

# compile each class individually; recompile only those with changes
$(OBJDIR)/%.o: %.cpp $(DEPS)
	@echo [${CC}] Compiling class $<        
	@$(CC) -c $(CFLAGS) $(COPTS) $< -o $@

# generate dictionary for all classes, including shared dictionary 
# that gets loaded into interactive ROOT
$(DICTSRC): $(DEPS) 
	@if [ ! -d $(LIBDIR) ]; then mkdir -p $(LIBDIR); fi
	@echo [ROOTCINT] Generating dictionary $(DICTSRC)
	@cd $(SRCDIR) && rootcint -f ../$(OBJDIR)/$(DICTSRC) -c -p *.h ../PigsLinkDef.h
	@echo [${CC}] Compiling dictionary $(DICTSRC)
	@$(CC) -c $(CFLAGS) $(COPTS) -c $(OBJDIR)/$(DICTSRC) -o $(OBJDIR)/$(DICT).o
	@echo [${CC}] Generating shared library $(LIBDIR)/$(DICT).so
	@$(CC) -shared  -o $(LIBDIR)/$(DICT).so  $(OBJS) $(OBJDIR)/$(DICT).o $(LIBS)

# link executable
$(EXEC1) : ${FUZZY_OBJS} $(OBJS) $(DICTSRC) $(EXEC1NAME).cpp 
	@echo [${CC}] Compiling $(EXEC1)
	@if [ ! -d $(EXEDIR) ]; then mkdir -p $(EXEDIR); fi
	@$(CC) $(CFLAGS) $(COPTS) $(OBJDIR)/$(DICT).o ${FUZZY_OBJS} $(OBJS) $(LIBS) -o $@
	@if [ -e ${_FUZZYDEF} ]; then cp ${_FUZZYDEF} ${EXEDIR}; fi

$(TESTEXE) : ${FUZZY_OBJS} $(TEST_SRC)
	@echo [${CC}] Compiling test $(TESTEXE)
	@$(CC) $(CFLAGS) $(COPTS) ${FUZZY_OBJS} $(TEST_SRC) -o $@

clean:
	rm -rf $(OBJDIR)/*  $(LIBDIR)/*.so $(EXEC1) 
