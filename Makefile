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
  LIBS=-L$(LIBDIR) -lsmapi -lfidoconf
else
  LIBS=-L$(LIBDIR) -lsmapi -lfidoconfig
endif

CDEFS=-D$(OSTYPE) $(ADDCDEFS) -DSQUISHCFS -Isrc

%$(OBJ): src$(DIRSEP)%.c
	$(CC) $(CFLAGS) $(CDEFS) -c $<

%$(OBJ): src$(DIRSEP)%.cpp
	$(CXX) $(CFLAGS) $(CDEFS) -c $<


all: cfroute$(EXE) fc2cfr$(EXE)

cfroute$(EXE): cfroute$(OBJ) dirute$(OBJ) fecfg146$(OBJ) buffer$(OBJ) structs$(OBJ)
	$(CXX) $(LFLAGS) $(EXENAMEFLAG)cfroute$(EXE) cfroute$(OBJ) dirute$(OBJ) fecfg146$(OBJ) buffer$(OBJ) structs$(OBJ) $(LIBS)

fc2cfr$(EXE): fc2cfr$(OBJ)
	$(CXX) $(LFLAGS) $(EXENAMEFLAG)fc2cfr$(EXE) fc2cfr$(OBJ) $(LIBS)

clean:
	-$(RM) cfroute$(OBJ)
	-$(RM) dirute$(OBJ)
	-$(RM) fecfg146$(OBJ)
	-$(RM) buffer$(OBJ)
	-$(RM) structs$(OBJ)
	-$(RM) fc2cfr$(OBJ)


distclean: clean
	-$(RM) cfroute$(EXE)

install: all
	$(INSTALL) $(IBOPT) cfroute$(EXE) $(BINDIR)
	$(INSTALL) $(IBOPT) fc2cfr$(EXE) $(BINDIR)

uninstall:
	-$(RM) $(BINDIR)$(DIRSEP)cfroute$(EXE)
	-$(RM) $(BINDIR)$(DIRSEP)fc2cfr$(EXE)
