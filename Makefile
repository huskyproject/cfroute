include ../huskymak.cfg

.PHONY: default

all: default

default: cfroute$(EXE)

ifeq ($(DEBUG), 1)
  CFLAGS= -I$(INCDIR) $(DEBCFLAGS)
  LFLAGS=$(DEBLFLAGS)
else
  CFLAGS= -I$(INCDIR) $(OPTCFLAGS)
  LFLAGS=$(OPTLFLAGS)
endif

ifeq ($(SHORTNAME), 1)
  LIBS=-L$(LIBDIR) -lsmapi
else
  LIBS=-L$(LIBDIR) -lsmapi
endif

CDEFS=-D$(OSTYPE) $(ADDCDEFS) -Isrc

%$(OBJ): src$(DIRSEP)%.c
	$(CC) $(CFLAGS) $(CDEFS) -c $<

%$(OBJ): src$(DIRSEP)%.cpp
	$(CXX) $(CFLAGS) $(CDEFS) -c $<


cfroute$(EXE): cfroute.o dirute.o fecfg146.o buffer.o structs.o
	$(CXX) $(LFLAGS) $(EXENAMEFLAG)cfroute$(EXE) cfroute.o dirute.o fecfg146.o buffer.o structs.o $(LIBS)

clean:
	-$(RM) cfroute.o
	-$(RM) dirute.o
	-$(RM) fecfg146.o
	-$(RM) buffer.o
	-$(RM) structs.o


distclean: clean
	-$(RM) cfroute$(EXE)

install: cfroute$(EXE)
	$(INSTALL) $(IBOPT) cfroute$(EXE) $(BINDIR)

uninstall:
	-$(RM) $(BINDIR)$(DIRSEP)cfroute$(EXE)
