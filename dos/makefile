### Make file for CFRoute for DOS
### Compiler: Borland C++ 3.1

### Directory options
SRCDIR=..\src
SMAPIDIR=..\..\smapi

### Compiler flags
CFLAGS = -DSQUISHCFS -I$(SMAPIDIR) -DMSDOS -O2 -w -ml $(RELEASE)

### Linker flags
LFLAGS = -L$(SMAPIDIR) -ml
LIBS = smapibcd.lib

### Platform specifics
CC       = bcc $(RELEASEFLAGS)
OBJ      = .obj
EXE      = .exe

### List of object files
cfrobjs = cfroute$(OBJ) dirute$(OBJ) structs$(OBJ) buffer$(OBJ) fecfg146$(OBJ)

### Build rules
.SUFFIXES:
.SUFFIXES: .cpp .c $(OBJ)

all: cfroute$(EXE)

cfroute$(OBJ):  $(SRCDIR)\cfroute.cpp $(SRCDIR)\akas.cpp $(SRCDIR)\basic.cpp \
                $(SRCDIR)\buffer.cpp $(SRCDIR)\config.cpp \
                $(SRCDIR)\datetime.cpp $(SRCDIR)\encdet.cpp \
                $(SRCDIR)\errors.hpp $(SRCDIR)\fastecho.cpp \
                $(SRCDIR)\handlers.cpp $(SRCDIR)\log.cpp $(SRCDIR)\macro.cpp \
                $(SRCDIR)\netmail.cpp $(SRCDIR)\password.cpp \
                $(SRCDIR)\protos.hpp $(SRCDIR)\routing.cpp \
                $(SRCDIR)\scontrol.cpp $(SRCDIR)\errors.hpp \
                $(SRCDIR)\fecfg146.h $(SRCDIR)\protos.hpp $(SRCDIR)\squish.cpp
	$(CC) -c -o$*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $(SRCDIR)\$*.cpp

dirute$(OBJ): $(SRCDIR)\dirute.c
	$(CC) -c -o$*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $(SRCDIR)\$*.c

fecfg146$(OBJ): $(SRCDIR)\fecfg146.c
	$(CC) -c -o$*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $(SRCDIR)\$*.c

buffer$(OBJ): $(SRCDIR)\buffer.cpp
	$(CC) -c -o$*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $(SRCDIR)\$*.cpp

structs$(OBJ): $(SRCDIR)\structs.cpp
	$(CC) -c -o$*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $(SRCDIR)\$*.cpp

cfroute$(EXE): $(cfrobjs)
	$(CC) $(LFLAGS) -ecfroute$(EXE) $(cfrobjs) $(LIBS)
