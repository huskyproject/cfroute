### Make file for CFRoute for OS/2
### Compiler: EMX 0.9c or 0.9d

### Directory options
SRCDIR=../src
SMAPIDIR=../../smapi

### Compiler flags
CFLAGS = -Zomf -O2 -DOS_2 -DEMX -DSQUISHCFS -DOS2 -I$(SMAPIDIR) $(DEBUG)

### Linker flags
LFLAGS = -s -L$(SMAPIDIR) -Zomf
LIBS=-lsmapiemo

### Platform specifics
DEL      = rm
CC       = gcc
OBJ      = .obj
EXE      = .exe

### List of object files
cfrobjs = cfroute$(OBJ) dirute$(OBJ) buffer$(OBJ) structs$(OBJ) fecfg145$(OBJ)

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
        errors.hpp fecfg145.h protos.hpp squish.cpp dirute.h structs.hpp

dirute.o: dirute.c dirute.h
structs.o: structs.cpp structs.hpp
buffer.o: buffer.cpp errors.hpp protos.hpp buffer.hpp
fecfg145.o: fecfg145.c fecfg145.h

clean:
	-cmd /c del cfroute$(EXE)
	-cmd /c del cfroute$(OBJ)
	-cmd /c del dirute$(OBJ)
	-cmd /c del buffer$(OBJ)
	-cmd /c del structs$(OBJ)
	-cmd /c del fecfg145$(OBJ)
