### Make file for CFRoute for Windows
### Compiler: Borland C++ 4.0

### Directory options
SRCDIR=..\src
SMAPIDIR=x:\smapi

### Compiler flags
CFLAGS = -DSQUISHCFS -I$(SMAPIDIR) -WC -DWIN -D__NT__

### Linker flags
LFLAGS = -L$(SMAPIDIR) -WC
LIBS = smapibcw.lib

### Platform specifics
CC       = bcc32
OBJ      = .obj
EXE      = .exe

### List of object files
cfrobjs = cfroute$(OBJ)

### Build rules
.SUFFIXES:
.SUFFIXES: .cpp $(OBJ)

all: cfroute$(EXE)

cfroute$(OBJ):  $(SRCDIR)\cfroute.cpp $(SRCDIR)\akas.cpp $(SRCDIR)\basic.cpp \
                $(SRCDIR)\buffer.cpp $(SRCDIR)\config.cpp \
                $(SRCDIR)\datetime.cpp $(SRCDIR)\encdet.cpp \
                $(SRCDIR)\errors.hpp $(SRCDIR)\fastecho.cpp \
                $(SRCDIR)\handlers.cpp $(SRCDIR)\log.cpp $(SRCDIR)\macro.cpp \
                $(SRCDIR)\netmail.cpp $(SRCDIR)\password.cpp \
                $(SRCDIR)\protos.hpp $(SRCDIR)\routing.cpp \
                $(SRCDIR)\scontrol.cpp $(SRCDIR)\errors.hpp \
                $(SRCDIR)\fecfg145.h $(SRCDIR)\protos.hpp $(SRCDIR)\squish.cpp
	$(CC) -c -o$*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $(SRCDIR)\$*.cpp

cfroute$(EXE): $(cfrobjs)
	$(CC) $(LFLAGS) -ecfroute$(EXE) $(cfrobjs) $(LIBS)
