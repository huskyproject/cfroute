### Make file for CFRoute for OS/2
### Compiler: EMX 0.9c or 0.9d

### Directory options
SRCDIR=../src
SMAPIDIR=x:/smapi

### Compiler flags
# replace -g with -O2 for release build
CFLAGS = -g -DOS_2 -DEMX -DSQUISHCFS -DOS2 -I$(SMAPIDIR)

### Linker flags
# add -s to LFLAGS and remove -g for release build
LFLAGS = -g -L$(SMAPIDIR)
LIBS=-lsmapiemx

### Platform specifics
DEL      = rm
CC       = gcc
OBJ      = .o
EXE      = .exe

### List of object files
cfrobjs = cfroute$(OBJ) dirute$(OBJ)

### Build rules
.SUFFIXES:
.SUFFIXES: .cpp $(OBJ) .c $(OBJ)

VPATH=$(SRCDIR)
.cpp$(OBJ):
	$(CC) -c -o $*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $<
VPATH=$(SRCDIR)
.c$(OBJ):
	$(CC) -c -o $*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $<

all: cfroute$(EXE)

cfroute$(EXE): $(cfrobjs)
	$(CC) $(LFLAGS) -o cfroute$(EXE) $(cfrobjs) $(LIBS)

cfroute.o: akas.cpp basic.cpp buffer.cpp config.cpp datetime.cpp \
        encdet.cpp  errors.hpp fastecho.cpp handlers.cpp log.cpp macro.cpp \
        netmail.cpp password.cpp protos.hpp routing.cpp scontrol.cpp \
        errors.hpp fecfg145.h protos.hpp squish.cpp dirute.h

dirute.o: dirute.c dirute.h

clean:
	-cmd /c del cfroute$(EXE)
	-cmd /c del cfroute$(OBJ)
	-cmd /c del dirute$(OBJ)


