#ifdef SQUISHCFS
extern "C" {
#include "smapi/msgapi.h"
}
#endif 

#define DEFINE_IDENT
#include "platform.h"


unsigned short IDENTMajor=1;
unsigned short IDENTMinor=0;
char *IDENTPos="a";
char IDENTString[80];

typedef unsigned char byte;
byte RequestInfo=0,NoDomainDir=0,ReadFEConfig=0,UseSquish=0,NoLoopRoute=0;
char FEConfDir[80];

unsigned short CONTROLBYTES = 8192;

#ifdef OS_2
# define INCL_BASE
# define INCL_DOS
# define INCL_DOSDEVICES
# define INCL_DOSDEVIOCTL
# define INCL_DOSERRORS
# define INCL_DOSFILEMGR
# define INCL_DOSMISC
# define INCL_DOSNMPIPES
# define INCL_DOSPROCESS
# define INCL_DOSSEMAPHORES
#define INCL_DOSMODULEMGR
#define INCL_DOSEXCEPTIONS
#include "os2.h"
#endif
#include "protos.hpp"
#include "errors.hpp"
#include "recode.cpp"
#include "basic.cpp"
#include "datetime.cpp"
#include "log.cpp"
#include "scontrol.cpp"
#include "password.cpp"
#include "encdet.cpp"
#include "macro.cpp"

CLog Log;

#include "config.cpp"
#include "akas.cpp"
#include "inbounds.hpp"

CAddresses AddressHandler;
C_Password PasswordHandler;
CInbounds InboundHandler;

#include "routing.cpp"

CRouteHandler RouteHandler;
CViaHandler Packet2Handler;
C_StringList CheckFiles;
C_StringList PendingTokens;
C_StringList SL_NetmailDir;
C_MacroContainer MacroHandler;

char OutboundDirectory[256];
char QQQOutboundDirectory[256];
byte IgnoreLastUsed,EOLEndsCommand=0;
byte RouteBossHold=0,RouteBossDirect=0;
byte RecordViaLines=0;
byte IgnoreBSY=0,KillInTransit=0,KillInTransitFiles=0;
byte PathLogStyle=0,FastechoPack=0;
byte IgnoreUnknownAttribs=0;
unsigned char *ControlText;

#include "netmail.cpp"

byte OutboundDirSetup=0, QQQOutboundDirSetup=0;
byte WarningCount=0;

#include "handlers.cpp"
#include "fastecho.cpp"

#ifdef SQUISHCFS
#include "smapi/msgapi.h"
#include "squish.cpp"
struct _minf minf;
#endif

#ifdef OS_2
#ifdef VAC
ULONG APIENTRY  exHandler (PEXCEPTIONREPORTRECORD,
                           PEXCEPTIONREGISTRATIONRECORD ,
                           PCONTEXTRECORD               ,
                           PVOID                        );
#endif
#endif

const int NV_Routed=0;
const int NV_Direct=1;
const int NV_Noroute=2;
const int NV_NoPack=3;

char Months[12][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug",
					"Sep","Oct","Nov","Dec"};

char Days[7][4]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};



int ProcessMacro (int MacroType,int (*CommandProcessor) (char *,S_FQAddress))
{
	int count;
	char Add[30];
	S_FQAddress GetA;
	if (AddressHandler.MainDefined()==0)
		return EMH_NOMAIN;
	switch (MacroType)
	{
		case TT_MYZONES:
			for (count=0;count<=AddressHandler.GetAKACount();count++)
			{
				AddressHandler.GetAKA (&GetA,count);
								sprintf (Add,"%u",GetA.Zone);
				strcat (Add,":*");
				if (CommandProcessor (Add,GetA)!=SUCCESS)
				{
					printf ("Fatal error processing macro MYZONES.\n");
					return FATAL;
				}
			}
			break;
		case TT_MYNETS:
			for (count=0;count<=AddressHandler.GetAKACount();count++)
			{
				AddressHandler.GetAKA (&GetA,count);
								sprintf (Add,"%u",GetA.Zone);
				strcat (Add,":");
								sprintf (Add+strlen (Add),"%u",GetA.Net);
				strcat (Add,"/*");
				if (CommandProcessor (Add,GetA)!=SUCCESS)
				{
					printf ("Fatal error processing macro MYNETS.\n");
					return FATAL;
				}
			}
			break;
		case TT_MYPOINTS:
		case TT_LOCAL:
			for (count=0;count<=AddressHandler.GetAKACount();count++)
			{
				AddressHandler.GetAKA (&GetA,count);
				if (GetA.Point==0 || MacroType==TT_LOCAL)
				{
										sprintf (Add,"%u",GetA.Zone);
					strcat (Add,":");
                                        sprintf (Add+strlen (Add),"%u",GetA.Net);
					strcat (Add,"/");
                                        sprintf (Add+strlen (Add),"%u",GetA.Node);
					strcat (Add,".");
					if (MacroType==TT_LOCAL)
												sprintf (Add+strlen (Add),"%u",GetA.Point);
					else
						strcat (Add,"*");
					if (CommandProcessor (Add,GetA)!=SUCCESS)
					{
						printf ("Fatal error processing macro MYPOINTS.\n");
						return FATAL;
					}
				}
			}
			break;
		default:
			return EMH_GENERAL;
	}
	return SUCCESS;
}

char *string_ctl (char *source)
{
	char *memr;
	memr=(char *) malloc (strlen (source)+1);
	if (memr!=NULL)
		strcpy (memr,source);
	return memr;
}

int ReadRouteFile (char *FileName)
{
	CConfigHandler ConfigHandler;
	char Line[256],JustRead[256],*Token;
	int (*CommandProcessor) (char *,S_FQAddress)=NULL;
	int MandatoryPar=0,IsParameter;
	S_Codes CurrentToken;
	long LineCounter;
	Macro *Mac;
	S_FQAddress Main;
        C_StringList MacroStack;
	if (ConfigHandler.Open (FileName)!=SUCCESS)
	{
		printf ("Unable to open file %s.\n",FileName);
		return (ECH_OPENFAILED);
	}
#ifdef DEBUG
	printf ("Route file open.\nReading...");
#endif
	LineCounter=0;
	while (ConfigHandler.ReadLine (Line,&LineCounter)!=ECH_EOF)
	{
#ifdef DEBUG
		printf ("Line: %s\n",Line);
#endif
		while (GetAndStripToken (Line,JustRead)==SUCCESS)
		{
#ifdef DEBUG
			printf ("Token: %s\n",JustRead);
#endif
			PendingTokens.AddString (JustRead);
			while (PendingTokens.GetStringCount()>0)
			{
				Token=string_ctl (PendingTokens.GetString(0));
				PendingTokens.RemoveFirst ();
				// Note that the macroend token has top priority
				if (!cistrcmp (Token," MACROEND "))
					MacroStack.RemoveLast ();
				else
				{
					if (AddressHandler.GetMain (&Main)!=SUCCESS)
						Main.Zone=Main.Net=Main.Node=Main.Point=0xFFFF;
					if (MandatoryPar>0)
						IsParameter=1;
					else
					{
						switch (GetTokenInfo (Token,&CurrentToken))
						{
							case PT_COMMAND:
								IsParameter=0;
								MandatoryPar=CurrentToken.Parameters;
								CommandProcessor=CurrentToken.ParameterProcessor;
								if (CurrentToken.Initiator!=NULL)
									switch (CurrentToken.Initiator (Main))
									{
										case WARNING:
											printf (" (%s, line %lu)\n",FileName,LineCounter);
											WarningCount++;
											break;
										case FATAL:
											return FATAL;
									}
								break;
							case PT_MACRO:
								IsParameter=0;
                                                                if (CommandProcessor!=NULL)
   								        switch (ProcessMacro (CurrentToken.Code,CommandProcessor))
								        {
  									case WARNING:
										printf (" (%s, line %lu)\n",FileName,LineCounter);
										WarningCount++;
										break;
									case FATAL:
										return FATAL;
                                                                        }
                                                                else
                                                                {
                                                                        printf ("Unexpected Macro %s at line %lu.\n",Token,LineCounter);
                                                                        WarningCount++;
                                                                }
								break;
							default:
								if ((Mac=MacroHandler.FindMacro (Token))==NULL)
									IsParameter=1;
								else
								{
									int count,store;
									IsParameter=0;
#ifdef DEBUG
									printf ("Called %s\n",Token);
#endif
									if (MacroStack.IsString (Token,&store))
									{
										printf ("Fatal: Circular reference detected processing %s. Dump follows:\n",Token);
										for (count=store;count<MacroStack.GetStringCount ();count++)
											printf ("%s -> ",MacroStack.GetString (count));
										printf ("%s\n",Token);
										return FATAL;
									}
									MacroStack.AddString (Token);
									for (count=Mac->GetStringCount()-1;count>=0;count--)
										if (PendingTokens.InsertString (Mac->GetString (count),1)!=SUCCESS)
										{
											printf ("Fatal: Out of memory processing %s.\n",Token);
											return FATAL;
										}
									PendingTokens.AddString (" MACROEND ");
								}
						}
					}
					if (IsParameter==1)
					{
						if (CommandProcessor!=NULL)
                                                        switch (CommandProcessor (Token,Main))
                                                        {
                                                        case WARNING:
                                                                printf (" (%s, line %lu)\n",FileName,LineCounter);
								WarningCount++;
								break;
							case FATAL:
								return FATAL;
							case NEEDEXTRAPAR:
		    					        MandatoryPar++;
                                                        }
						else
						{
							printf ("Unexpected parameter %s at line %lu.\n",Token,LineCounter);
							WarningCount++;
						}
						if (MandatoryPar)
							MandatoryPar--;
					}
				} // else (not MACROEND)
				if (Token!=NULL)
					free (Token);
			} // While pending tokens on the queue
		} // While token on line
		if (EOLEndsCommand)
		{
			// Won't harm if called outside a define
			MacroHandler.Process ("ENDDEFINE");
			CommandProcessor=NULL;
			MandatoryPar=0;
		}
	} // While lines
	printf ("Configuration file %s read, %u warning%s.\n",FileName,
		WarningCount,WarningCount==1?"":"s");
	return SUCCESS;
}

#ifdef SQUISHCFS
void ProcessSquish (char *SquishBase, int msgtype)
{
	unsigned long counttotal=0,countproc=0,count=0,countdelayed=0;
	HAREA BaseH;
        const char *areatype = (msgtype==MSGTYPE_SQUISH) ? "Squish" : "Jam";
	printf ("Processing %s base %s\n",areatype, SquishBase);
	BaseH=MsgOpenArea ((unsigned char *) SquishBase,MSGAREA_NORMAL,
                           msgtype);
	if (BaseH==NULL)
	{
		printf ("Failed to open %s area: ", areatype);
		switch (msgapierr)
		{
			case MERR_NOMEM:
				printf ("Not enough memory.\n");
				break;
			case MERR_NOENT:
				printf ("Area doesn't exist.\n");
				break;
			case MERR_BADF:
				printf ("Area is damaged.\n");
				break;
		}
		return;
	}
	counttotal=MsgGetNumMsg (BaseH);
	for (count=1;count<=MsgGetNumMsg (BaseH);count++)
        {
#ifdef DEBUG
		printf ("[%04lu]\n",count);
#endif
		switch (AnalyzeSquishNet (BaseH,count))
		{
			case ENH_KILLED:
				count--;
			case ENH_PROCESSED:
				countproc++;
				break;
			case ENH_DELAYED:
				countdelayed++;
				break;
		}
	}
#ifdef DEBUG
	printf ("Completed netmail processing.\n");
#endif
	printf ("Total %lu messages in %s base, %lu delayed, %lu packed.\n",
		counttotal,areatype,countdelayed,countproc);
	if (MsgCloseArea (BaseH)==-1)
		printf ("Warning: Failed to close %s area.\n", areatype);
}
#endif

void ProcessNetmail (char *NetmailDir)
{
	int control;
	short isolder;
	int counttotal=0,countproc=0,count=0,countdelayed=0;
#ifdef OS_2
	FILEFINDBUF3 findmsg;
	ULONG ulFileCount=1;
	HDIR hdir=HDIR_SYSTEM;
#else
	struct ffblk findmsg;
#endif
	char complete[512],LastFName[512];
	C_SpecialStringList NetNames;
	DateTime dtnow;
	union
	{
		struct
		{
			unsigned sec: 5;
			unsigned min: 6;
			unsigned hour: 5;
		} stt;
		unsigned short number;
	} dosfiletime;
        struct std_type_2
        {
          unsigned day: 5;
          unsigned mon: 4;
          unsigned year: 7;
        };

	union
        {
                struct std_type_2 std;    
		unsigned short number;
	} dosfiledate;
	long lrdate,lrtime,filedate,filetime;
	/* struct date d,dnow;
	struct time t,tnow; */
        DateTime dt;
	FILE *LastUsed;
	printf ("Processing netmail directory %s\n",NetmailDir);
	strcpy (complete,NetmailDir);
	strcat (complete,"*.MSG");
	strcpy (LastFName,NetmailDir);
	strcat (LastFName,"LASTRUN.CFR");
	if (IgnoreLastUsed==0)
		LastUsed=fopen (LastFName,"rb");
	else
		LastUsed=NULL;
	if (LastUsed!=NULL)
	{
                dt.read(LastUsed);
		printf ("Found lastrun.cfr (%s %u, %u - %02u:%02u:%02u)\n",
					Months[dt.month-1],dt.day,dt.year,
					dt.hours,dt.minutes,dt.seconds);
		fclose (LastUsed);
                lrdate=GetJulianDate (dt.day,dt.month,dt.year);
	}
	else
	{
                lrdate=0;
		memset (&dt,0,sizeof (dt));
		memset (&dt,0,sizeof (dt));
	}
	/* Init the random generator with a more or less unpredictable
	   number. */
	dtnow.getCurrentTime();
        srand (dtnow.hours*dtnow.minutes*dtnow.seconds);

#ifdef OS_2
        control=DosFindFirst ((PCSZ) complete,&hdir,
			FILE_NORMAL | FILE_READONLY,
			&findmsg,sizeof (findmsg),&ulFileCount,FIL_STANDARD);
#else
	control=findfirst (complete,&findmsg,FA_ARCH);
#endif
	while (!control)
	{
		counttotal++;
#ifdef DEBUG
#ifdef OS_2
		printf ("Found: %s\n",findmsg.achName);
#else
		printf ("Found: %s\n",findmsg.ff_name);
#endif
#endif
		strcpy (complete,NetmailDir);
#ifdef OS_2
		strcat (complete,findmsg.achName);
		memcpy (&dosfiletime,&findmsg.ftimeLastWrite,sizeof (struct _FTIME));
		memcpy (&dosfiledate,&findmsg.fdateLastWrite,sizeof (struct _FDATE));
#else
		strcat (complete,findmsg.ff_name);
		dosfiletime.number=findmsg.ff_ftime;
		dosfiledate.number=findmsg.ff_fdate;
#endif
#ifdef DEBUG
		printf ("Lastrun date: %02u-%02u-%04u  File date: %02u-%02u-%04u\n",
					dt.day,dt.month,dt.year,
					dosfiledate.std.day,dosfiledate.std.mon,dosfiledate.std.year+1980);
		printf ("Lastrun time: %02u:%02u:%02u  File time: %02u:%02u:%02u\n",
					dt.hours,dt.minutes,dt.seconds,
					dosfiletime.stt.hour,dosfiletime.stt.min,dosfiletime.stt.sec);
#endif
		filedate=GetJulianDate (dosfiledate.std.day,dosfiledate.std.mon,
							dosfiledate.std.year+1980);
#ifdef DEBUG
		printf ("Numeric date (lastrun & filetime): %ld, %ld\n",
				lrdate,filedate);
#endif
		if (lrdate>filedate) // Last processed after filedate - don't process it
			isolder=1;
		else
		{
			if (lrdate<filedate) // File received/touch after last process
				isolder=-1;
			else
			{	// Same date
				lrtime=dt.hours*(60*60)+dt.minutes*60+dt.seconds;
				filetime=dosfiletime.stt.hour*(60*60)+
				dosfiletime.stt.min*60+dosfiletime.stt.sec*2;
#ifdef DEBUG
				printf ("Numeric times (lastrun & filetime): %ld, %ld\n",
					lrtime,filetime);
#endif
				if (lrtime<=filetime)
					isolder=-1;
				else
					isolder=1;
			}
		}
		if (isolder<0)
		{
#ifdef DEBUG
			printf ("File %s hasn't been processed yet, added to queue.\n",complete);
#endif
			if (NetNames.AddFileName (complete)==NOMEMORY)
			{
				printf ("Fatal: Out of memory.\n");
				return;
			}
		}
#ifdef DEBUG
		else
			printf ("File %s has already been processed, skipped.\n",complete);
#endif
#ifdef OS_2
		control=DosFindNext (hdir,&findmsg,sizeof (findmsg),&ulFileCount);
#else
		control=findnext (&findmsg);
#endif
	 }
#ifdef OS_2
	 DosFindClose (hdir);
#endif
#ifdef DEBUG
	printf ("Calling Sort\n");
#endif
	NetNames.Sort ();
	for (count=0;count<NetNames.GetStringCount ();count++)
	{
#ifdef DEBUG
		printf ("[%04u] %s\n",count,NetNames.GetString (count));
#endif
		switch (AnalyzeNet (NetNames.GetString (count)))
		{
			case ENH_PROCESSED:
				countproc++;
				break;
			case ENH_DELAYED:
				countdelayed++;
				break;
			}
	}
#ifdef DEBUG
	printf ("Completed netmail processing.\n");
#endif
	printf ("Total %u .MSGs in netmail directory, %u processed, %u delayed, %u packed.\n",
		counttotal,NetNames.GetStringCount (),countdelayed,countproc);
	strcpy (LastFName,NetmailDir);
	strcat (LastFName,"LASTRUN.CFR");
	LastUsed=fopen (LastFName,"wb");
	if (LastUsed!=NULL)
	{
                dtnow.write(LastUsed);
		fclose (LastUsed);
	}
}

#if !defined(OS2) && !defined(UNIX)
extern unsigned _stklen=32000;
#endif

unsigned long getcompiletime(void)
{
        unsigned long rv = 0;
        int off = 0;
        static const char *datestr = __DATE__;

        switch (datestr[0])
        {
        case 'A':
                rv = (datestr[1] == 'u') ? 8*100      // August
                                         : 4*100;     // April
                break;
        case 'F':
                rv = 2*100; break; // February
        case 'M':
                rv = (datestr[2] == 'r') ? 3*100      // March
                                         : 5*100;     // May
                break;
        case 'J':
                rv = (datestr[1] == 'a') ? 1*100      // January
                   : (datestr[2] == 'n') ? 6*100      // June
                                         : 7*100;     // July
                break;
        case 'S':
                rv = 9*100; break;  // September
        case 'O':
                rv = 10*100; break; // October
        case 'N':
                rv = 11*100; break; // November
        case 'D':
                rv = 12*100; break; // December
        default:
                abort();  // Should not happen
        }

        if (datestr[5] == ' ')
        {
                rv = rv + datestr[4] - '0';
                off = 0;
        }
        else if (datestr[4] == ' ')
        {
                rv = rv + datestr[5] - '0';
                off = 1;
        }
        else
        {
                rv = rv + datestr[5] - '0' + (datestr[4] - '0') * 10;
                off = 1;
        }

        rv = rv + (datestr[6+off] - '0') * 10000000L +   // year
                  (datestr[7+off] - '0') * 1000000L +    // year
                  (datestr[8+off] - '0') * 100000L +     // year
                  (datestr[9+off] - '0') * 10000L;       // year
        return rv;
}

void ShowSyntax (void)
{
	printf ("Usage:\n");
	printf ("CFROUTE config_file [config_file...] [-I] [-D] [-V] [-?]\n\n");
	printf ("See documentation for a (more or less) detailed explanation on the\n");
	printf ("configuration file syntax.\n");
	printf ("-I forces a complete netmail processing (ignoring file timestamps).\n");
	printf ("-D write a configuration dump on screen.\n");
	printf ("-V log via lines.\n");
	printf ("-? shows this help (!?) screen.\n");
}

int ProcessArguments (int argc,char **argv)
{
	int count,keep,configfile=0;
	IgnoreLastUsed=0; RequestInfo=0;
	for (count=1;count<argc;count++)
	{
		if (argv[count][0]!='-'
#ifndef UNIX                    
                    && argv[count][0]!='/'
#endif
                    )
		{
			if ((keep=ReadRouteFile (argv[count]))!=SUCCESS)
			{
				printf ("Non-recoverable error reading configuration file %s, exiting.\n",
					argv[count]);
				return (keep);
			}
			else
				configfile=1;
		}
		else
		{
			switch (toupper (argv[count][1]))
			{
				case 'D':
					RequestInfo=1;
					break;
				case 'I':
					IgnoreLastUsed=1;
					printf ("lastrun.cfr will be ignored.\n");
					break;
				case 'V':
					RecordViaLines=1;
					break;
				case 'H':
				case '?':
					ShowSyntax ();
					return (ECH_FORCEOUT);
				default:
					printf ("Unknown parameter %s.\n",argv[count]);
					return (ECH_UNKNOWNARG);
			}
		}
	}
	if (configfile)
		return SUCCESS;
	else
	{
		ShowSyntax ();
		return ECH_NOCONFIG;
	}
}

int main (int argc,char **argv)
{
	int keep,count;
#ifdef OS_2
	FILEFINDBUF3 LookForFiles;
	ULONG ulFileCount;
        HDIR hDir=HDIR_SYSTEM;
#ifdef VAC
        EXCEPTIONREGISTRATIONRECORD xcpthand;
#endif
#else
	struct ffblk LookForFiles;
#endif
	S_WAddress WA;
	PassAdd PassAddWork;
	int count2;
	S_FQAddress OnProcess;
#ifdef OS_2
	HMTX Mutex;
#endif
#ifdef OS_2
#ifdef VAC
        xcpthand.prev_structure   = 0;
        xcpthand.ExceptionHandler = &exHandler;
        DosError(FERR_DISABLEEXCEPTION | FERR_DISABLEHARDERR);
        DosSetExceptionHandler(&xcpthand);
#endif
#endif
#ifdef RELEASE
	sprintf (IDENTString,"%s %u.%u%s",IDENTProgramName,IDENTMajor,IDENTMinor,IDENTPos);
	printf ("%s (%s, %s) - A freeware netmail router for FTN.\n",
			IDENTString,__DATE__,__TIME__);
#else
	sprintf (IDENTString,"%s %08lu-devel",IDENTProgramName,getcompiletime());
	printf ("%s (%s, %s) - Freeware FTN netmail router\n",
			IDENTString,__DATE__,__TIME__);
#endif
#ifndef UNIX        
	printf ("By Carlos Fern ndez Sanz, Madrid, Spain. Formerly 2:341/70, now cfs@nova.es.\n");
#else
        printf ("By Carlos Fernandez Sanz, Madrid, Spain. Formerly 2:341/70, now cfs@nova.es.\n");
#endif
        printf ("Modifications by Tobias Ernst, Stuttgart, GER. 2:2476/418, tobi@bland.fido.de\n");
#ifdef OS_2
#ifndef EMX
// EMX does not use this exception handler stuff - it has core files :-)
        printf ("OS/2 exception handler taken from Binkley-XE. Most of it by Robert Hoerner.\n");
#endif
#endif
        printf ("------------------------------------------------------------------------------\n");
#ifdef OS_2
        if (DosCreateMutexSem ((PCSZ) "\\SEM32\\RUNNING.CFR",&Mutex,DC_SEM_SHARED,TRUE)!=0)
	{
		printf ("CFRoute is currently running, exiting.\n");
		return EMAIN_CFRACTIVE;
	}
#endif
	Packet2Handler.SetStatus (1,1,1); // We'll use it as a simple container
	if ((keep=ProcessArguments (argc,argv))!=SUCCESS)
        {
                printf ("Leaving\n");
		return (keep);
        }
	if (ReadFEConfig)
		if (FastEchoConfig ()!=SUCCESS)
		{
			printf ("Error: Failed to read FastEcho's config, terminating.\n");
			return (ECH_OPENFAILED);
		}
	if ((AddressHandler.MainDefined ())==0)
	{
		printf ("Error: Main address not defined.\n");
		return (ENOMAIN);
	}
	if (SL_NetmailDir.GetStringCount()==0)
	{
		printf ("Fatal error: No netmail directories specified.\n");
		return (ECH_NONETMAILDIR);
	}
	if (!OutboundDirSetup)
	{
		printf ("Fatal error: Outbound directory not specified.\n");
		return (ECH_NOOUTBOUNDDIR);
	}
        if (!QQQOutboundDirSetup && FastechoPack)
        {
                printf ("Fatal error: QQQOutbound directory must be "
                        "specified when using FastechoPack");
                return (ECH_NOQQQOUTBOUNDDIR);
        }
        
#ifndef DEBUG
	if (RequestInfo)
#endif
	{
		printf ("Completed route-file reading.\n");
		printf ("Netmail directories:\n");
		for (count=0;count<SL_NetmailDir.GetStringCount();count++)
			printf ("[%03u] %s\n",count,SL_NetmailDir.GetString(count));
		printf ("AKA list: [0=Main]\n");
		for (count=0;count<=AddressHandler.GetAKACount();count++)
		{
			AddressHandler.GetAKA (&OnProcess,count);
			printf ("[%03u] %u:%u/%u.%u%s%s\n",count,OnProcess.Zone,
			OnProcess.Net,OnProcess.Node,OnProcess.Point,OnProcess.Domain[0]?"@":"",
			OnProcess.Domain);
		}
		printf ("Via systems:\n");
		for (count=0;count<RouteHandler.GetViaCount();count++)
		{
			if (RouteHandler.GetViaSystem (&OnProcess,count)==SUCCESS)
					printf ("[%03u] %u:%u/%u.%u - (%u,%u)\n",count,OnProcess.Zone,
					OnProcess.Net,OnProcess.Node,OnProcess.Point,RouteHandler.GetRouteToCount (count),
					RouteHandler.GetRouteFromCount (count));
			else
					printf ("[%03u] - Non-routed (%u,%u)\n",count,
					RouteHandler.GetRouteToCount (count),
					RouteHandler.GetRouteFromCount (count));
			for (count2=0;count2<RouteHandler.GetRouteToCount (count);count2++)
			{
				RouteHandler.GetRoutedToAddress (&WA,count,count2);
				printf ("        [%03u] To: %s,%s,%s,%s %s\n",count2,WA.Zone,
					WA.Net,WA.Node,WA.Point,WA.Reverse?"(E)":" ");
			}
			for (count2=0;count2<RouteHandler.GetRouteFromCount (count);count2++)
			{
				RouteHandler.GetRoutedFromAddress (&WA,count,count2);
				printf ("        [%03u] From: %s,%s,%s,%s %s\n",count2,WA.Zone,
					WA.Net,WA.Node,WA.Point,WA.Reverse?"(E)":" ");
			}
		}
		printf ("Packet 2:\n");
		for (count=0;count<Packet2Handler.GetRouteToCount();count++)
		{
			Packet2Handler.GetRoutedToAddress (&WA,count);
			printf ("        [%03u] To: %s,%s,%s,%s %s\n",count,WA.Zone,
				WA.Net,WA.Node,WA.Point,WA.Reverse?"(E)":" ");
		}
		printf ("Passwords:\n");
		for (count=0;count<PasswordHandler.GetPasswordCount();count++)
		{
			PasswordHandler.GetPair (&PassAddWork,count);
			printf ("[%03u] Address: %s:%s/%s.%s. Password: %s\n",
				count,PassAddWork.Address.Zone,PassAddWork.Address.Net,
				PassAddWork.Address.Node,PassAddWork.Address.Point,
				PassAddWork.Password);
		}
		printf ("Checkfiles:\n");
		for (count=0;count<CheckFiles.GetStringCount();count++)
			printf ("[%03u] File: %s\n",count,CheckFiles.GetString (count));
		printf ("Macros:\n");
		for (count=0;count<MacroHandler.GetMacroCount();count++)
		{
			Macro *Sel;
			Sel=MacroHandler.GetMacro (count);
			printf ("[%03u] Macro name: %s\n",count+1,Sel->MacroName);
			for (count2=0;count2<Sel->GetStringCount();count2++)
			printf ("        [%03u] Token: %s\n",
				count2+1,Sel->GetString (count2));
		}
	}
	for (count=0;count<CheckFiles.GetStringCount();count++)
	{
#ifdef OS_2
                if (DosFindFirst ((PCSZ) CheckFiles.GetString (count),&hDir,
			FILE_NORMAL | FILE_READONLY,&LookForFiles,
			sizeof (LookForFiles),&ulFileCount,FIL_STANDARD)==0)
#else
		if (findfirst (CheckFiles.GetString (count),
			&LookForFiles,FA_ARCH | FA_RDONLY)==0)
#endif
		{
			printf ("File %s exists, exiting.\n",CheckFiles.GetString (count));
			return EMAIN_FOUNDFLAG;
		}
#ifdef OS_2
			DosFindClose (hDir);
#endif
	}
#ifdef SQUISHCFS
	if (UseSquish)
	{
		ControlText=(unsigned char *) malloc (CONTROLBYTES);
		minf.req_version=MSGAPI_VERSION;
		AddressHandler.GetMain(&OnProcess);
		minf.def_zone=OnProcess.Zone;
#ifndef SMAPI_VERSION   /* SMAPI does not have these entries */
		minf.palloc=NULL;
		minf.pfree=NULL;
		minf.repalloc=NULL;
		minf.farpalloc=NULL;
		minf.farpfree=NULL;
		minf.farrepalloc=NULL;
#endif
		if (MsgOpenApi (&minf)==-1 || ControlText==NULL)
		{
			printf ("Fatal: Failed to init the Squish/Jam API.\n");
			exit (5);
		}
	}
#endif
	for (count=0;count<SL_NetmailDir.GetStringCount();count++)
	{
		if (SL_NetmailDir.GetString(count)[0]=='!')
#ifdef SQUISHCFS
			ProcessSquish (SL_NetmailDir.GetString (count)+1,
                                       MSGTYPE_SQUISH)
#endif
                ; else if (SL_NetmailDir.GetString(count)[0]=='?') 
#ifdef SQUISHCFS
                        ProcessSquish(SL_NetmailDir.GetString(count)+1,
                                      MSGTYPE_JAM)
#endif
                ; else
			ProcessNetmail (SL_NetmailDir.GetString (count));
	}
#ifdef SQUISHCFS
	if (UseSquish)
		if (MsgCloseApi ()==-1)
			printf ("Warning: Failed to close the Squish API.\n");
#endif
#ifdef OS_2
#ifdef VAC
        DosUnsetExceptionHandler(&xcpthand);
#endif
#endif
	return 0;
}
