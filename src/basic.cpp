// Provide some basic string handling
#include <ctype.h>
#if !defined(OS_2) && !defined(UNIX)
#include <dos.h>
#else
// #include <direct.h>
#endif

#ifdef UNIX
#include <sys/stat.h>
#if !defined(__FreeBSD__) && !defined(_AIX) && !defined(__GLIBC__)
#define mkdir __mkdir
#endif
#else
#include <io.h>
#endif

#ifndef OS_2
typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef short SHORT;
typedef unsigned short USHORT;


typedef struct DATETIME
{
  UCHAR	 hours;
  UCHAR	 minutes;
  UCHAR	 seconds;
  UCHAR	 hundredths;
  UCHAR	 day;
  UCHAR	 month;
  USHORT year;
  SHORT	 timezone;
  UCHAR	 weekday;
} DATETIME;

#ifdef UNIX
#include <time.h>
void DosGetDateTime (DATETIME *dt)
{
        time_t t;
        struct tm *ltm;

        time(&t);
        ltm = localtime(&t);

        dt->hours   = ltm->tm_hour;
        dt->minutes = ltm->tm_min;
        dt->seconds = ltm->tm_sec;
        dt->day     = ltm->tm_mday;
        dt->month   = ltm->tm_mon + 1;
        dt->year    = ltm->tm_year + 1900;
}
#else
void DosGetDateTime (DATETIME *dt)
{
        struct date d;
	struct time t;
	
        getdate (&d);
	gettime (&t);
	dt->hours=t.ti_hour;
	dt->minutes=t.ti_min;
	dt->seconds=t.ti_sec;
	dt->day=d.da_day;
	dt->month=d.da_mon;
	dt->year=d.da_year;
}
#endif
#endif

int cistrcmp (char *c1,char *c2)
{
		char *s1,*s2;
        s1=c1; s2=c2;
        while (*s1 || *s2)
        {
				if (toupper (*s1)==toupper (*s2))
                {
                        s1++;
                        s2++;
                }
                else
                        return (toupper (*s1)-toupper (*s2));
        }
        return 0;
}

// Copy first token from string to token and strip it
int GetAndStripToken (char *string, char *token)
{
	char *search;
	if (*string==0)
		return SH_ZERO;
	if (string[0]=='"')
	{
		strcpy (string,string+1); // Strip "
		search=strchr (string,'"');
	}
	else
		search=strchr (string,' ');
	if (search!=NULL)
		*search=0;
	strcpy (token,string);
	if (search!=NULL)
	{
		strcpy (string,search+1);
		while (*string==' ')
			strcpy (string,string+1);
	}
	else
		*string=0;
	return SUCCESS;
}

int IsCommand (int TokenType)
{
	switch (TokenType)
	{
		case TT_MAIN:
		case TT_AKA:
		case TT_NETMAILDIR:
		case TT_OUTBOUND:
                case TT_QQQOUTBOUND:
		case TT_LOGFILENAME:
		case TT_TOPDOWN:
		case TT_UNKNOWN:
		case TT_ROUTETO:
		case TT_DIRECT:
		case TT_ROUTEMAIL:
		case TT_ROUTEFILES:
		case TT_DIRECTMAIL:
		case TT_DIRECTFILES:
		case TT_NOROUTE:
		case TT_NOROUTEMAIL:
		case TT_NOROUTEFILES:
		case TT_PACKET2:
		case TT_PASSWORD:
		case TT_INCLUDE:
			return 1;
		default:
			return 0;
	}
}


struct S_Codes
CodeTable[]={               {"OUTBOUND",TT_OUTBOUND,NULL,ProcParOutboundDir,1,PT_COMMAND},
                            {"QQQOUTBOUND", TT_QQQOUTBOUND, NULL, ProcParQQQOutboundDir, 1, PT_COMMAND},
                            {"NETMAIL",TT_NETMAILDIR,NULL,ProcParNetmail,1,PT_COMMAND},
#ifdef SQUISHCFS
                            {"NETSQUISH",TT_NETSQUISH,NULL,ProcParNetSquish,1,PT_COMMAND},
#endif
                            {"LOG",TT_LOGFILENAME,NULL,ProcParLogFilename,1,PT_COMMAND},
                            {"PASSWORD",TT_PASSWORD,InitComPassword,ProcParPassword,1,PT_COMMAND},
                            {"LOGPATH",TT_LOGPATH,InitComLogpath,NULL,0,PT_COMMAND},
                            {"INCLUDE",TT_INCLUDE,NULL,ProcParInclude,1,PT_COMMAND},
                            {"DEFINE",TT_DEFINE,NULL,ProcParDefine,1,PT_COMMAND},
                            {"ENDDEFINE",TT_ENDDEFINE,InitComEndDefine,NULL,0,PT_COMMAND},
                            {"CHECKFILE",TT_CHECKFILE,NULL,ProcParCheckFile,1,PT_COMMAND},
                            {"TOPDOWN",TT_TOPDOWN,InitComTopdown,NULL,0,PT_COMMAND},
                            {"FECONFIG",TT_FECONFIG,InitComFEConfig,ProcParFEConfig,0,PT_COMMAND},
                            {"ROUTETO",TT_ROUTETO,InitComRouteTo,ProcParRoute,0,PT_COMMAND},
                            {"ROUTE",TT_ROUTETO,InitComRouteTo,ProcParRoute,0,PT_COMMAND},
                            {"SEND",TT_ROUTETO,InitComRouteTo,ProcParRoute,0,PT_COMMAND},
                            {"ROUTEMAIL",TT_ROUTEMAIL,InitComRouteMail,ProcParRoute,0,PT_COMMAND},
                            {"PRODUCTINFO",TT_PRODUCTINFO,InitComProductInfo,ProcParProductInfo,1,PT_COMMAND},
                            {"ROUTEFILES",TT_ROUTEFILES,InitComRouteFiles,ProcParRoute,0,PT_COMMAND},
                            {"DIRECT",TT_DIRECT,InitComDirect,ProcParRoute,0,PT_COMMAND},
                            {"DIRECTTO",TT_DIRECT,InitComDirect,ProcParRoute,0,PT_COMMAND},
                            {"DIRECTMAIL",TT_DIRECTMAIL,InitComDirectMail,ProcParRoute,0,PT_COMMAND},
                            {"DIRECTFILES",TT_DIRECTFILES,InitComDirectFiles,ProcParRoute,0,PT_COMMAND},
                            {"NOROUTE",TT_NOROUTE,InitComNoroute,ProcParRoute,0,PT_COMMAND},
                            {"NOROUTETO",TT_NOROUTE,InitComNoroute,ProcParRoute,0,PT_COMMAND},
                            {"ROUTEBOSS",TT_NOROUTE,InitComNoroute,ProcParRoute,0,PT_COMMAND},
                            {"NOROUTEMAIL",TT_NOROUTEMAIL,InitComNorouteMail,ProcParRoute,0,PT_COMMAND},
                            {"ROUTEBOSSMAIL",TT_NOROUTEMAIL,InitComNorouteMail,ProcParRoute,0,PT_COMMAND},
                            {"NOROUTEFILES",TT_NOROUTEFILES,InitComNorouteFiles,ProcParRoute,0,PT_COMMAND},
                            {"ROUTEBOSSFILES",TT_NOROUTEFILES,InitComNorouteFiles,ProcParRoute,0,PT_COMMAND},
                            {"NOPACK",TT_NOPACK,InitComNoPack,ProcParRoute,0,PT_COMMAND},
                            {"NOPACKTO",TT_NOPACK,InitComNoPack,ProcParRoute,0,PT_COMMAND},
                            {"NOPACKMAIL",TT_NOPACKMAIL,InitComNoPackMail,ProcParRoute,0,PT_COMMAND},
                            {"IGNOREUNKNOWNATTRIBS",TT_IGNOREUNKNOWNATTRIBS,InitComIgnoreUnknownAttribs,NULL,0,PT_COMMAND},
                            {"NOPACKFILES",TT_NOPACKFILES,InitComNoPackFiles,ProcParRoute,0,PT_COMMAND},
                            {"VIABOSSHOLD",TT_VIABOSSHOLD,InitComViaBossHold,NULL,0,PT_COMMAND},
                            {"VIABOSSDIRECT",TT_VIABOSSDIRECT,InitComViaBossDirect,NULL,0,PT_COMMAND},
                            {"IGNOREBSY",TT_IGNOREBSY,InitComIgnoreBSY,NULL,0,PT_COMMAND},
                            {"NODOMAINDIR",TT_NODOMAINDIR,InitComNoDomainDir,NULL,0,PT_COMMAND},
                            {"KILLINTRANSIT",TT_KILLINTRANSIT,InitComKillInTransit,NULL,0,PT_COMMAND},
                            {"FASTECHOPACK",TT_FASTECHOPACK,InitComFastechoPack,ProcParFastechoPack,0,PT_COMMAND},
                            {"ALL",TT_QQQALL,NULL,NULL,0,PT_QQQMODE},
                            {"ENC",TT_ENCRYPTED,NULL,NULL,0,PT_ENC},
                            {"NONENC",TT_NONENCRYPTED,NULL,NULL,0,PT_ENC},
                            {"CRASH",TT_CRASH,NULL,NULL,0,PT_FLAVOUR},
                            {"HOLD",TT_HOLD,NULL,NULL,0,PT_FLAVOUR},
                            {"DIR",TT_DIR,NULL,NULL,0,PT_FLAVOUR},
                            {"NORMAL",TT_NORMAL,NULL,NULL,0,PT_FLAVOUR},
                            {"MYZONES",TT_MYZONES,NULL,NULL,0,PT_MACRO},
                            {"MYNETS",TT_MYNETS,NULL,NULL,0,PT_MACRO},
                            {"MYPOINTS",TT_MYPOINTS,NULL,NULL,0,PT_MACRO},
                            {"LOCAL",TT_LOCAL,NULL,NULL,0,PT_MACRO},
                            {"PACKET2",TT_PACKET2,InitComPacket2,ProcParPacket2,0,PT_COMMAND},
                            {"MAIN",TT_MAIN,NULL,ProcParMain,0,PT_COMMAND},
                            {"AKA",TT_AKA,NULL,ProcParAKA,0,PT_COMMAND},
                            {"FROM",TT_FROM,NULL,NULL,0,PT_FROM},
                            {"EXCEPT",TT_EXCEPT,NULL,NULL,0,PT_EXCEPT},
                            {"ASSUMEPOINTS",TT_ASSUMEPOINTS,InitComAssumePoints,NULL,0,PT_COMMAND},
                            {"ASSUMEWAYPOINTS",TT_ASSUMEWAYPOINTS,InitComAssumeWaypoints,NULL,0,PT_COMMAND},
                            {"EOLENDSCOMMAND",TT_EOLENDSCOMMAND,InitComEOLEndsCommand,NULL,0,PT_COMMAND},
                            {"PVT",TT_PVT,NULL,NULL,0,PT_ATTRIB},
                            {"HLD",TT_HLD,NULL,NULL,0,PT_ATTRIB},
                            {"CRA",TT_CRA,NULL,NULL,0,PT_ATTRIB},
                            {"K/S",TT_KS,NULL,NULL,0,PT_ATTRIB},
                            {"SNT",TT_SNT,NULL,NULL,0,PT_ATTRIB},
                            {"RCV",TT_RCV,NULL,NULL,0,PT_ATTRIB},
                            {"A/S",TT_AS,NULL,NULL,0,PT_ATTRIB},
                            {"ZON",TT_ZON,NULL,NULL,0,PT_ATTRIB},
                            {"HUB",TT_HUB,NULL,NULL,0,PT_ATTRIB},
                            {"FIL",TT_FIL,NULL,NULL,0,PT_ATTRIB},
                            {"FRQ",TT_FRQ,NULL,NULL,0,PT_ATTRIB},
                            {"IMM",TT_IMM,NULL,NULL,0,PT_ATTRIB},
                            {"XMA",TT_XMA,NULL,NULL,0,PT_ATTRIB},
                            {"KFS",TT_KFS,NULL,NULL,0,PT_ATTRIB},
                            {"TFS",TT_TFS,NULL,NULL,0,PT_ATTRIB},
                            {"LOK",TT_LOK,NULL,NULL,0,PT_ATTRIB},
                            {"RRQ",TT_RRQ,NULL,NULL,0,PT_ATTRIB},
                            {"CFM",TT_CFM,NULL,NULL,0,PT_ATTRIB},
                            {"HIR",TT_HIR,NULL,NULL,0,PT_ATTRIB},
                            {"COV",TT_COV,NULL,NULL,0,PT_ATTRIB},
                            {"SIG",TT_SIG,NULL,NULL,0,PT_ATTRIB},
                            {"LET",TT_LET,NULL,NULL,0,PT_ATTRIB},
                            {"FAX",TT_FAX,NULL,NULL,0,PT_ATTRIB},
                            {"FPU",TT_FPU,NULL,NULL,0,PT_ATTRIB},
                            {"",0,NULL,NULL,0,PT_END}
};

// Get Token Info
int GetTokenInfo (char *Token, struct S_Codes *storage)
{
	char *Mess,*Update;
	int count,canbedow;
	if (Token[0]==0)
		return SH_ZERO;
	count=0;
	Mess=(char *) malloc (strlen (Token)+1);
	strcpy (Mess,Token);
	Update=Mess;
	while (*Update)
	{
		*Update=toupper (*Update);
		Update++;
	}
	Update=Mess;
	while (*Update)
	{
		if (*Update=='-'|| *Update=='_')
			strcpy (Update,Update+1);
		else
			Update++;
	}

	while (CodeTable[count].Keyword[0]!=0)
	{
                if (!cistrcmp (Mess,CodeTable[count].Keyword))
		{
			memcpy (storage,&CodeTable[count],sizeof (struct S_Codes));
			free (Mess);
			return (CodeTable[count].Group);
		}
		count++;
	}
	// Not a known keyword
	// Check for a day of the week spec.
	canbedow=1;
	Update=Mess;
	while (*Update && canbedow)
	{
		if (*Update!='M' && *Update!='T' && *Update!='W' &&
			*Update!='H' && *Update!='F' && *Update!='S' &&
			*Update!='U')
		{
			canbedow=0;
			break;
		}
		Update++;
	}
	if (canbedow)
	{
		free (Mess);
		storage->Keyword=Token;
		storage->Code=TT_DOW;
		storage->Group=PT_DOW;
		return False;
	}
	if (isdigit (Mess[0]) || Mess[0]=='.' || Mess[0]=='!' || Mess[0]=='*' ||
		Mess[0]=='?' || Mess[0]=='#')
	{
		free (Mess);
		storage->Keyword=Token;
		storage->Code=TT_ADDRESS;
		storage->Group=PT_ADDRESS;
		return (PT_ADDRESS);
	}
	// Check if it is a definition
	storage->Code=TT_UNKNOWN;
	storage->Group=PT_OTHER;
	return TT_UNKNOWN;
}


// Attempt to determine token type
int GetTokenType (char *Token)
{
	char *Mess,*Update;
	int count,canbedow;
	if (Token[0]==0)
		return SH_ZERO;
	// Tokens starting with a digit are most probably network addresses
	Mess=(char *) malloc (strlen (Token)+1);
	strcpy (Mess,Token);
	Update=Mess;
	while (*Update)
	{
		*Update=toupper (*Update);
		Update++;
	}
	Update=Mess;
	while (*Update)
	{
		if (*Update=='-'|| *Update=='_')
			strcpy (Update,Update+1);
		else
			Update++;
	}
	canbedow=1;
	Update=Mess;
	while (*Update && canbedow)
	{
		if (*Update!='M' && *Update!='T' && *Update!='W' &&
			*Update!='H' && *Update!='F' && *Update!='S' &&
			*Update!='U')
		{
			canbedow=0;
			break;
		}
		Update++;
	}
	if (canbedow)
	{
		free (Mess);
		return TT_DOW;
	}
	if (isdigit (Mess[0]) || Mess[0]=='.' || Mess[0]=='!' || Mess[0]=='*' ||
		Mess[0]=='?' || Mess[0]=='#')
	{
		free (Mess);
		return (TT_ADDRESS);
	}
	count=0;
	while (CodeTable[count].Keyword[0]!=0)
	{
                if (!cistrcmp (Mess,CodeTable[count].Keyword))
		{
			free (Mess);
			return (CodeTable[count].Code);
		}
        count++;
	}
	free (Mess);
	return TT_UNKNOWN;
}

int GetFullQualifiedAddress (char *Saddress, struct S_FQAddress *storage,struct S_FQAddress *Main)
{
	int NoMain;
        size_t count,count2;
	char Compo[25]; // Maximum: AAAAA:BBBBB:CCCCC:DDDDD\0
	char *search,*first,*colon,*slash,*point;
	char address[80];
	if (strlen (Saddress)>79)
		return EAD_INVALID;
	strcpy (address,Saddress);
	search=strchr (address,' ');
	if (search)
		*search=0;
	if (Main==NULL || (Main->Zone==0xFFFF && Main->Net==0xFFFF &&
		Main->Node==0xFFFF && Main->Point==0xFFFF))
		NoMain=1;
	else
		NoMain=0;
	if (*address=='\0')
		return EAD_ZERO;
    // First thing to do is parse the domain
	search=strchr (address,'@');
	if (search!=NULL)
	{
		strcpy (storage->Domain,search+1);
		*search=0;
	}
	else
        {
                storage->Domain[0]=0;
                search=address+strlen(address)-1; /* Last char */
                while (search >= address &&
                       !isdigit (*search) && *search!=':' && *search!='/' &&
                       *search!='.')
                        *(search--)=0;
        }
	for (count=0;count<strlen (address);count++)
		if (!isdigit (address[count]) && address[count]!=':' &&
			address[count]!='/' && address[count]!='.')
			return EAD_INVALID;
	count2=0;
	for (count=0;count<strlen (address);count++)
		if (address[count]==':')
			count2++;
	if (count2>1)
		return EAD_INVALID;
	count2=0;
	for (count=0;count<strlen (address);count++)
		if (address[count]=='/')
			count2++;
	if (count2>1)
		return EAD_INVALID;
	count2=0;
	for (count=0;count<strlen (address);count++)
		if (address[count]=='.')
			count2++;
	if (count2>1)
		return EAD_INVALID;
    // count2=0;
	// for (count=0;count<strlen (address);count++)

	// Discard any address with wildcards
	if (strchr (address,'*') || strchr (address,'?'))
		return EAD_WILDCARDS;
	// At the very least, there must be a : and a / to be a FQA.
	if ((!strchr (address,':') || !strchr (address,'/')) && NoMain)
		return EAD_CANTRESOLVE;
	// Add what's missing, if anything, to get a 4D address
	// Zone
	first=address;
	search=strchr (first,':');
	if (!search)
		if (NoMain)
			return EAD_CANTRESOLVE;
		else
						sprintf (Compo,"%u",Main->Zone);
	else
	{
		*search=0;
		strcpy (Compo,first);
		first=search+1;
	}
	strcat (Compo,":");
	// So far compo has the zone number and the :.
	// Test for net
	search=strchr (first,'/');
	if (!search)
		if (NoMain)
			return EAD_CANTRESOLVE;
		else
                        sprintf (Compo+strlen(Compo),"%u",Main->Net);
	else
	{
		*search=0;
		strcat (Compo,first);
		first=search+1;
	}
	strcat (Compo,"/");
	// Compo: XXXX:YYYY/    ->
	// Node & Point
	search=strchr (first,'.');
	if (first==search) // No hay nodo
                sprintf ("%u",&Compo[strlen (Compo)],Main->Node);
	strcat (Compo,first);
	if (!search) // The rest is only the node
		strcat (Compo,".0");
	// FQA completed - now we have to split it to get the number
#ifdef DEBUG
	printf ("FQA: %s (",Compo);
#endif
	colon=strchr (Compo,':');
	slash=strchr (Compo,'/');
	point=strchr (Compo,'.');
	if ((slash==colon+1) || (point==slash+1))
		return EAD_INVALID;
	*colon=*slash=*point=0;
	storage->Zone=atoi (Compo);
	storage->Net=atoi (colon+1);
	storage->Node=atoi (slash+1);
	storage->Point=atoi (point+1);
	if (storage->Domain[0]==0 && NoMain==0)
    	strcpy (storage->Domain,Main->Domain);
#ifdef DEBUG
	printf ("%u:%u/%u.%u)\n",storage->Zone,storage->Net,storage->Node,storage->Point);
#endif
	return SUCCESS;
}

int HasWildcards (char *address)
{
	size_t count;
	for (count=0;count<strlen (address);count++)
		if ((address[count]=='#') ||
			(address[count]=='?') ||
			(address[count]=='*'))
			return 1;
	return 0;
}

int SplitWildcardedAddress (char *origaddress, struct S_WAddress *storage,struct S_WAddress *Main)
{
	int NoMain;
        size_t count,count2;
	char *colon,*slash,*point;
	char address[81];
	if (Main==NULL)
		NoMain=1;
	else
		NoMain=0;
	if (strlen (origaddress)>80)
		return EAD_INVALID;
	strcpy (address,origaddress);
	if (*address=='\0')
		return EAD_ZERO;
	storage->Reverse=0;
	colon=strchr (address,'@');
	if (colon)
	{
		*colon=0;
		strcpy (storage->Domain,colon+1);
	}
	else
		storage->Domain[0]=0;
	if (address[0]=='!')
	{
		storage->Reverse=1;
		strcpy (address,address+1);
	}
	for (count=0;count<strlen (address);count++)
		if (!isdigit (address[count]) && address[count]!=':' &&
			address[count]!='/' && address[count]!='.' &&
			address[count]!='*' && address[count]!='?' &&
			address[count]!='#')
			return EAD_INVALID;
	count2=0;
	for (count=0;count<strlen (address);count++)
		if (address[count]==':')
			count2++;
	if (count2>1)
		return EAD_INVALID;
	count2=0;
	for (count=0;count<strlen (address);count++)
		if (address[count]=='/')
			count2++;
	if (count2>1)
		return EAD_INVALID;
	count2=0;
	for (count=0;count<strlen (address);count++)
		if (address[count]=='.')
			count2++;
	if (count2>1)
		return EAD_INVALID;
	storage->Zone[0]=storage->Net[0]=storage->Node[0]=storage->Point[0]=0;
	colon=strchr (address,':');
	slash=strchr (address,'/');
	point=strchr (address,'.');
	// There are eight possibilities, based on the separators found
	switch ((colon!=NULL)*4+(slash!=NULL)*2+(point!=NULL))
	{
		case 0: // No separators -> anything there is the node specification
			if (address[0]=='*')
			{
				strcpy (storage->Zone,"*");
				strcpy (storage->Net,"*");
				strcpy (storage->Node,"*");
				strcpy (storage->Point,"*");
			}
			else
			{
				if (NoMain==1)
					return EAD_CANTRESOLVE;
				if (HasWildcards (Main->Zone) || HasWildcards (Main->Net))
					return EAD_CANTRESOLVE;
				strcpy ((char *) &storage->Node,address);
				if (HasWildcards (storage->Node))
					strcpy ((char *) &storage->Point,"*");
			}
			break;
		case 1: // Only a point
			*point=0;
			if (NoMain==1)
				return EAD_CANTRESOLVE;
			if (HasWildcards (Main->Zone) || HasWildcards (Main->Net) ||
				HasWildcards (Main->Node))
				return EAD_CANTRESOLVE;
			strcpy ((char *) &storage->Point,point+1);
			strcpy ((char *) &storage->Node,address);
			break;
		case 2: // Only a slash
			if (NoMain==1)
				return EAD_CANTRESOLVE;
			if (HasWildcards (Main->Zone))
				return EAD_CANTRESOLVE;
			*slash=0;
			strcpy ((char *) &storage->Node,slash+1);
			strcpy ((char *) &storage->Net,address);
                        if ((storage->Node[strlen (storage->Node)-1]=='*') ||
                                (storage->Node[strlen (storage->Node)-1]=='?') ||
                                (stoE (íš  8‘Ä†‚9	EÀ _êÁÎÙ˜cŠy=D)-1]=='#'))
				 strcpy ((char *) &storage->Point,"*");
			break;
		case 3: // Slash & point
			if (slash>point)
				return EAD_INVALID;
			if (NoMain==1)
				return EAD_CANTRESOLVE;
			if (HasWildcards (Main->Zone))
				return EAD_CANTRESOLVE;
			*point=0;
			*slash=0;
			strcpy ((char *) &storage->Point,point+1);
			strcpy ((char *) &storage->Node,slash+1);
			strcpy ((char *) &storage->Net,address);
			break;
		case 4: // Only colon (ie 2: or 2:*)
			*colon=0;
			strcpy ((char *) &storage->Net,colon+1);
			strcpy ((char *) &storage->Zone,address);
                        if ((storage->Net[strlen (storage->Net)-1]=='*') ||
                                (storage->Net[strlen (storage->Net)-1]=='?') ||
                                (storage->Net[strlen (storage->Net)-1]=='#'))
				{
					strcpy ((char *) &storage->Node,"*");
					strcpy ((char *) &storage->Point,"*");
				}
			break;
		case 5: // Colon & point (A:B.C -> nonsense)
			return EAD_INVALID;
		case 6: // Colon & slash (ie 2:40/32)
			if (colon>slash)
				return EAD_INVALID;
			*colon=0;
			*slash=0;
			strcpy ((char *) &storage->Node,slash+1);
			strcpy ((char *) &storage->Net,colon+1);
			strcpy ((char *) &storage->Zone,address);
                        if ((storage->Node[strlen (storage->Node)-1]=='*') ||
                                (storage->Node[strlen (storage->Node)-1]=='?') ||
                                (storage->Node[strlen (storage->Node)-1]=='#'))
				 strcpy ((char *) &storage->Point,"*");
			break;
		case 7: // colon & slash & point
			if (colon>slash || slash>point)
				return EAD_INVALID;
			*colon=0;
			*slash=0;
			*point=0;
			strcpy ((char *) &storage->Point,point+1);
			strcpy ((char *) &storage->Node,slash+1);
			strcpy ((char *) &storage->Net,colon+1);
			strcpy ((char *) &storage->Zone,address);
			break;
	}
	if (storage->Node[0]=='*')
		storage->Node[0]=0;
	// Sixteen possibilities based on the filled fields
	switch ((storage->Zone[0]!=0)*8+(storage->Net[0]!=0)*4+
			(storage->Node[0]!=0)*2+(storage->Point[0]!=0))
	{
		case 0: // [ ]:[ ]/[ ].[ ]
			strcpy ((char *) storage->Zone,"*");
			strcpy ((char *) storage->Net,"*");
			strcpy ((char *) storage->Node,"*");
			strcpy ((char *) storage->Point,"*");
			break;
		case 1: // [ ]:[ ]/[ ].[X]
			if (NoMain)
				return EAD_CANTRESOLVE;
			strcpy (storage->Zone,Main->Zone);
			strcpy (storage->Net,Main->Net);
			strcpy (storage->Node,Main->Node);
			break;
		case 2: // [ ]:[ ]/[X].[ ]
			if (NoMain)
				return EAD_CANTRESOLVE;
			// Notice the following exception:
			//	* is not be parsed as *:*/*.* at not as z:n/*.* !!!!
			strcpy (storage->Zone,Main->Zone);
			strcpy (storage->Net,Main->Net);
			strcpy ((char *) storage->Point,"0");
		case 3: // [ ]:[ ]/[X].[X]
			if (NoMain)
				return EAD_CANTRESOLVE;
			strcpy (storage->Zone,Main->Zone);
			strcpy (storage->Net,Main->Net);
			break;
		case 4: // [ ]:[X]/[ ].[ ]
			strcpy (storage->Zone,Main->Zone);
			strcpy ((char *) storage->Node,"*");
			strcpy ((char *) storage->Point,"*");
			break;
		case 5: // [ ]:[X]/[ ].[X] (node was a *, so 341/*.34).
			strcpy (storage->Zone,Main->Zone);
			strcpy ((char *) storage->Node,"*");
			break;
		case 6: // [ ]:[X]/[X].[ ]
			strcpy (storage->Zone,Main->Zone);
			strcpy ((char *) storage->Point,"0");
			break;
		case 7: // [ ]:[X]/[X].[X]
			strcpy (storage->Zone,Main->Zone);
			break;
		case 8: // [X]:[ ]/[ ].[ ]
			strcpy ((char *) storage->Net,"*");
			strcpy ((char *) storage->Node,"*");
			strcpy ((char *) storage->Point,"*");
			break;
		case 9:  // [X]:[ ]/[ ].[X]
		case 10: // [X]:[ ]/[X].[ ]
		case 11: // [X]:[ ]/[X].[X]
			return EAD_INVALID;
		case 12: // [X]:[X]/[ ].[ ]
			strcpy ((char *) storage->Node,"*");
			strcpy ((char *) storage->Point,"*");
			break;
		case 13: // [X].[X]/[ ].[X] // (ie 2:341/*.0)
			strcpy ((char *) storage->Node,"*");
			break;
		case 14: // [X].[X]/[X].[ ]
			strcpy ((char *) storage->Point,"0");
			break;
		case 15: // [X].[X].[X].[X]
			;
	}
#ifdef DEBUG
	printf ("%s,%s,%s,%s)\n",storage->Zone,storage->Net,storage->Node,storage->Point);
#endif
	return SUCCESS;
}

int CompareBit (word Number,char *string)
{
	char SNum[6];
	char *pos1,*pos2;
	pos1=SNum; pos2=string;
        sprintf (SNum,"%u",Number);
	// Pos1 point to the number and cannot have wildcards
	while (*pos1 && *pos2) // Until a end-of-string is reached
	{
		if (*pos2=='*') // No need to check more. It is a match
			return 1;
		if (*pos2=='?') // Everything is ok - check next character
			{pos1++; pos2++;}
		else
		if (*pos2=='#')
		{
			if (*pos2) // There must be a character - no matter which
				{pos1++; pos2++;}
			else
				return 0; // No match
		}
		else // If here, no wildcard in pos2
		if (*pos1==*pos2)
			{pos1++;pos2++;}
		else
			return 0; // No match
	}
	// If here, all the characters in the shortest string matches
	// the first characters in the largest. What next...?
	if (*pos1==*pos2 || (*pos1==0 && *pos2=='*'))
		return 1; // Match
	else
		return 0;
}

// Determine whether an address matches a wildcard or not...
int CompareAddress (struct S_FQAddress FQA,struct S_WAddress *WA)
{
	if (CompareBit (FQA.Zone,WA->Zone) && CompareBit (FQA.Net,WA->Net) &&
		CompareBit (FQA.Node,WA->Node) && CompareBit (FQA.Point,WA->Point))
		return 1;
	else
    	return 0;
}

long lengthoffile(FILE *f)
{
#ifndef UNIX          
        return (filelength (fileno(f)));
#else
        long curofs = ftell(f);
        long rv = 0x7FFFFFFFL;

        fseek(f, 0, SEEK_END);
        rv = ftell(f);
        fseek(f, curofs, SEEK_SET);
        
        return rv;
#endif
}

FILE *makedirandfopen (char *filename,const char *mode)
{
	FILE *fp;
	char *workout,*pos;
	char *mkdirwork;
        adaptcase(filename);
	fp=fopen (filename,mode);
	if (fp!=NULL)
		return fp; // End of problem :-)
	if (strchr (filename,DIRSEPC)==NULL)
		return NULL; // Nothing to do without a slash...
	workout=(char *) malloc (strlen (filename)+1);
	if (workout==NULL)
		return NULL;
	strcpy (workout,filename);
	pos=workout+strlen (workout)-1;
	while (*pos!=DIRSEPC)
		pos--;
	*pos=0;
	// OK, now we have just the directory name
	mkdirwork=(char *) malloc (strlen (workout)+1);
	if (mkdirwork==NULL)
	{
		free (workout);
		return NULL;
	}
	strcpy (mkdirwork,workout);
	pos=mkdirwork;
	/* pos=strchr (mkdirwork,':');
	if (pos==NULL)
		pos=mkdirwork;
	else      /
		pos++; */
	while (*pos)
	{
		while (*pos!=DIRSEPC && *pos)
			pos++;
		if (*pos)
		{
			*pos=0;
#if defined(EMX) || defined(UNIX)
                        mkdir (mkdirwork,0);
#else
                        mkdir (mkdirwork);
#endif
			*pos=DIRSEPC;
			pos++;
		}
	}
#if defined(EMX) || defined(UNIX)
		mkdir (mkdirwork,0);
#else
		mkdir (mkdirwork);
#endif
	fp=fopen (filename,mode);
	return fp;
}

int GetMatchLevel (struct S_FQAddress A1,struct S_FQAddress A2)
{
	int DomCounts;
	if (A1.Domain[0] && A2.Domain[0])
		DomCounts=1;
	else
		DomCounts=0;
	if (DomCounts)
                if (cistrcmp (A1.Domain,A2.Domain)) // Domain mismatch, goodbye!
			return 0;
	if (A1.Zone==A2.Zone && A1.Net==A2.Net && A1.Node==A2.Node &&
		A1.Point==A2.Point)
		return 4+DomCounts;
	if (A1.Zone==A2.Zone && A1.Net==A2.Net && A1.Node==A2.Node)
		return 3+DomCounts;
	if (A1.Zone==A2.Zone && A1.Net==A2.Net)
		return 2+DomCounts;
	if (A1.Zone==A2.Zone)
		return 1+DomCounts;
	return DomCounts; //0 = no domain info, 1=domain matches
}

class C_WildAddress
{
	int inited;
	S_WAddress Last;
public:
	int ParseAddress (char *Token,S_WAddress *storage);
	C_WildAddress (void);
};

C_WildAddress::C_WildAddress (void)
{
	inited=0;
}

int C_WildAddress::ParseAddress (char *Token,S_WAddress *storage)
{
	int keep;
	if (inited==0)
		keep=SplitWildcardedAddress (Token,storage,NULL);
	else
		keep=SplitWildcardedAddress (Token,storage,&Last);
	if (keep==SUCCESS)
		inited=1;
	else
		inited=0;
	memcpy (&Last,storage,sizeof (struct S_WAddress));
	return (keep);
}
