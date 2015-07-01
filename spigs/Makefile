##########################################################################
# *** sPIGS makefile ***
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

COPTS = -fPIC -DLINUX -Wall $(shell root-config --cflags) -I/usr/local/include/CAEN 

LIBS = $(shell root-config --libs --glibs) -L/usr/local/lib/CAEN -lCAENDPPLib

CFLAGS += -Wall -I$(SRCDIR)
VPATH = src
LDFLAGS= 

DEPS = $(wildcard $(SRCDIR)/*.h)
_SRCS = $(wildcard $(SRCDIR)/*.cpp)
_OBJS = $(_SRCS:.cpp=.o)
OBJS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(_OBJS))

DICT = PigsDict
DICTSUF = cpp
DICTSRC = $(DICT).$(DICTSUF)

EXEC1NAME = spigs
EXEC1 = $(EXEDIR)/$(EXEC1NAME).exe

.PHONY: clean $(EXEC1)
.NOTPARALLEL: $(EXEC1)

##########################################################################

all: $(OBJS) $(DICTSRC) $(EXEC1) 

# compile each class individually; recompile only those with changes
$(OBJDIR)/%.o: %.cpp $(DEPS)
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
	@echo [${CC}] Compiling class $<
	@$(CC) -c $(CFLAGS) $(COPTS) $< -o $@

# generate dictionary for all classes, including shared dictionary 
# that gets loaded into interactive ROOT
$(DICTSRC): $(DEPS) $(OBJS)
	@if [ ! -d $(LIBDIR) ]; then mkdir -p $(LIBDIR); fi
	@echo [ROOTCINT] Generating dictionary $(DICTSRC)
	@cd $(SRCDIR) && rootcint -f ../$(OBJDIR)/$(DICTSRC) -c -p *.h ../PigsLinkDef.h
	@echo [${CC}] Compiling dictionary $(DICTSRC)
	@$(CC) -c $(CFLAGS) $(COPTS) -c $(OBJDIR)/$(DICTSRC) -o $(OBJDIR)/$(DICT).o
	@echo [${CC}] Generating shared library $(LIBDIR)/$(DICT).so
	@$(CC) -shared  -o $(LIBDIR)/$(DICT).so  $(OBJS) $(OBJDIR)/$(DICT).o $(LIBS)

# link executable
$(EXEC1) : $(EXEC1NAME).cpp
	@echo [${CC}] Compiling $(EXEC1)
	@if [ ! -d $(EXEDIR) ]; then mkdir -p $(EXEDIR); fi
	@$(CC) $(CFLAGS) $(COPTS) $(OBJDIR)/$(DICT).o $(OBJS) $(LIBS) -o $@
	
clean:
	rm -rf $(OBJDIR)/*.o $(DICT).* *.o  $(LIBDIR)/*.so $(EXEC1) 
