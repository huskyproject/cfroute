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

cfroute$(EXE): cfroute$(OBJ) dirute$(OBJ) fecfg146$(OBJ) buffer$(OBJ) structs$(OBJ) inbounds$(OBJ)
	$(CXX) $(LFLAGS) $(EXENAMEFLAG)cfroute$(EXE) cfroute$(OBJ) dirute$(OBJ) fecfg146$(OBJ) buffer$(OBJ) structs$(OBJ) inbounds$(OBJ) $(LIBS)

fc2cfr$(EXE): fc2cfr$(OBJ)
	$(CXX) $(LFLAGS) $(EXENAMEFLAG)fc2cfr$(EXE) fc2cfr$(OBJ) $(LIBS)

clean:
	-$(RM) $(RMOPT) cfroute$(OBJ)
	-$(RM) $(RMOPT) dirute$(OBJ)
	-$(RM) $(RMOPT) fecfg146$(OBJ)
	-$(RM) $(RMOPT) buffer$(OBJ)
	-$(RM) $(RMOPT) structs$(OBJ)
	-$(RM) $(RMOPT) fc2cfr$(OBJ)


distclean: clean
	-$(RM) $(RMOPT) cfroute$(EXE)
	-$(RM) $(RMOPT) fc2cfr$(EXE)

install: all
	$(INSTALL) $(IBOPT) cfroute$(EXE) $(BINDIR)
	$(INSTALL) $(IBOPT) fc2cfr$(EXE) $(BINDIR)

uninstall:
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)cfroute$(EXE)
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)fc2cfr$(EXE)
