### Requires gmake!

### Build rules
.SUFFIXES:
.SUFFIXES: .cpp $(OBJ) .c

VPATH=$(SRCDIR)
.cpp$(OBJ):
	$(CC) -c -o $*$(OBJ) -I$(SRCDIR) $(CFLAGS) $<
.c$(OBJ):
	$(CC) -c -o $*$(OBJ) -I$(SRCDIR) $(CFLAGS) $<

all: cfroute$(EXE)

cfroute$(EXE): $(cfrobjs)
	$(CC) $(LFLAGS) -o cfroute$(EXE) $(cfrobjs) $(LIBS)

cfroute.o: akas.cpp basic.cpp buffer.cpp config.cpp datetime.cpp \
        encdet.cpp  errors.hpp fastecho.cpp handlers.cpp log.cpp macro.cpp \
        netmail.cpp password.cpp protos.hpp routing.cpp scontrol.cpp \
        errors.hpp fecfg145.h protos.hpp squish.cpp dirute.h
dirute.o: dirute.c dirute.h

clean:
	-rm cfroute$(EXE)
	-rm cfroute$(OBJ)
	-rm dirute$(OBJ)

