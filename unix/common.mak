### Requires gmake!

### Build rules
.SUFFIXES:
.SUFFIXES: .cpp $(OBJ) .c

VPATH=$(SRCDIR)
.cpp$(OBJ):
	$(CC) -c -o $*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $<
.c$(OBJ):
	$(CC) -c -o $*$(OBJ) -I$(SRCDIR) $(CFLAGS) $(REL) $<

all: cfroute$(EXE)

cfroute$(EXE): $(cfrobjs)
	$(CC) $(LFLAGS) -o cfroute$(EXE) $(cfrobjs) $(LIBS)

cfroute.o: akas.cpp basic.cpp config.cpp datetime.cpp \
        encdet.cpp  errors.hpp fastecho.cpp handlers.cpp log.cpp macro.cpp \
        netmail.cpp password.cpp protos.hpp routing.cpp scontrol.cpp \
        errors.hpp fecfg145.h protos.hpp squish.cpp dirute.h buffer.hpp \
	structs.hpp
dirute.o: dirute.c dirute.h
buffer.o: buffer.cpp buffer.hpp
structs.o: structs.cpp structs.hpp
fecfg145.o: fecfg145.c fecfg145.h

clean:
	-rm cfroute$(EXE)
	-rm cfroute$(OBJ)
	-rm dirute$(OBJ)
	-rm buffer$(OBJ)
	-rm structs$(OBJ)
	-rm fecfg145$(OBJ)


