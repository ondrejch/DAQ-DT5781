CC = g++

SRCDIR = src
OBJDIR = obj
EXEDIR = bin
LIBDIR = lib

COPTS = -fPIC -DLINUX -Wall $(shell root-config --cflags)  -I/usr/local/include/CAEN 

LIBS = $(shell root-config --glibs) -lCAENDigitizer

CFLAGS = -c -g -Wall -I$(SRCDIR)
VPATH = src
LDFLAGS= -g

DEPS = $(wildcard $(SRCDIR)/*.h)
_SRCS = $(wildcard $(SRCDIR)/*.cpp)
_OBJS = $(_SRCS:.cpp=.o)
OBJS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(_OBJS))

DICT = PigsDict
DICTSUF = cpp
DICTSRC = $(DICT).$(DICTSUF)

EXEC1NAME = spigs
EXEC1 = $(EXEDIR)/$(EXEC1NAME).exe

#############################################################

all: $(OBJS) $(DICTSRC) $(EXEC1) 

# compile each class individually; recompile only those with changes
$(OBJDIR)/%.o: %.cpp $(DEPS)
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
	@echo [${CC}] Compiling class $<
	@$(CC) $(CFLAGS) $(COPTS) $< -o $@

# generate dictionary for all classes, including shared dictionary 
# that gets loaded into interactive ROOT
$(DICTSRC): $(DEPS) $(OBJS)
	@if [ ! -d $(LIBDIR) ]; then mkdir -p $(LIBDIR); fi
	@echo [ROOTCINT] Generating dictionary $(DICTSRC)
	@cd $(SRCDIR) && rootcint -f ../$(OBJDIR)/$(DICTSRC) -c -p *.h ../PigsLinkDef.h
	@echo [${CC}] Compiling dictionary $(DICTSRC)
	@$(CC) $(CFLAGS) $(COPTS) -c $(OBJDIR)/$(DICTSRC) -o $(OBJDIR)/$(DICT).o
	@echo [${CC}] Generating shared library $(LIBDIR)/$(DICT).so
	@$(CC) -shared  -o $(LIBDIR)/$(DICT).so  $(OBJS) $(OBJDIR)/$(DICT).o $(LIBS)


# compile executable
$(EXEC1) : $(EXEC1NAME).cpp
	@echo [${CC}] Compiling $(EXEC1NAME).cpp
	@if [ ! -d $(EXEDIR) ]; then mkdir -p $(EXEDIR); fi
	@$(CC) $(CFLAGS) $(COPTS) $< -o $@

.PHONY: clean

clean:
	rm -rf $(OBJDIR)/*.o $(DICT).* *.o  $(LIBDIR)/*.so $(EXEC1) 

