#include <stdlib.h>
#include <stdio.h>
#include "platform.h"
#if !defined(OS_2) && !defined(UNIX)
#include <dir.h>
#else
// #include <dos.h>
#endif
#include <string.h>

typedef unsigned short word;

#define False 0
#define True  1

#define BUFFER_SIZE 2048

// Parameter group definition
#define PT_COMMAND  	0
#define PT_ADDRESS		1
#define PT_ENC      	2
#define PT_DOW      	3
#define PT_ATTRIB   	4
#define PT_FLAVOUR  	5
#define PT_MACRO    	6
#define PT_FROM     	7
#define PT_EXCEPT   	8
#define PT_DEFINITION 	9
#define PT_QQQMODE      10
#define PT_OTHER    	11
#define PT_END     		200

extern const int NV_Routed;
extern const int NV_Direct;
extern const int NV_Noroute;
extern const int NV_NoPack;

extern char Months[12][4];
extern char Days[7][4];



// Some structures
struct S_FQAddress
{
	word Zone, Net, Node, Point;
	char Domain[64];
};

// Structure to store an address with wildcards
struct S_WAddress
{
	int Reverse; // For miscellaneous use
	char Zone[6];
	char Net[6];
	char Node[6];
	char Point[6];
	char Domain[64];
};

struct S_Codes
{
	char *Keyword;
	int Code;
    int (*Initiator) (struct S_FQAddress);
    int (*ParameterProcessor) (char *,struct S_FQAddress);
	int Parameters;
	int Group;
};

int ReadRouteFile (char *FileName);

// Command initiators
int InitComPassword (struct S_FQAddress Main);
int InitComTopdown (struct S_FQAddress Main);
int InitComRouteTo (struct S_FQAddress Main);
int InitComRouteMail (struct S_FQAddress Main);
int InitComRouteFiles (struct S_FQAddress Main);
int InitComDirect (struct S_FQAddress Main);
int InitComDirectMail (struct S_FQAddress Main);
int InitComDirectFiles (struct S_FQAddress Main);
int InitComNoroute (struct S_FQAddress Main);
int InitComNorouteMail (struct S_FQAddress Main);
int InitComNorouteFiles (struct S_FQAddress Main);
int InitComNoPack (struct S_FQAddress Main);
int InitComNoPackMail (struct S_FQAddress Main);
int InitComNoPackFiles (struct S_FQAddress Main);
int InitComPacket2 (struct S_FQAddress Main);
int InitComEndDefine (struct S_FQAddress Main);
int InitComAssumePoints (struct S_FQAddress Main);
int InitComAssumeWaypoints (struct S_FQAddress Main);
int InitComEOLEndsCommand (struct S_FQAddress Main);
int InitComViaBossDirect (struct S_FQAddress Main);
int InitComViaBossHold (struct S_FQAddress Main);
int InitComIgnoreBSY (struct S_FQAddress Main);
int InitComProductInfo (struct S_FQAddress Main);
int InitComKillInTransit (S_FQAddress Main);
int InitComKillInTransitFiles (S_FQAddress Main);
int InitComLogpath (S_FQAddress Main);
int InitComFastechoPack (S_FQAddress Main);
int InitComNoDomainDir (S_FQAddress Main);
int InitComNoLoopRoute (S_FQAddress Main);
int InitComFEConfig (S_FQAddress Main);
int InitComIgnoreUnknownAttribs (S_FQAddress Main);

// Parameter processors
int ProcParNetmail (char *Token, struct S_FQAddress Main);
int ProcParNetSquish (char *Token, struct S_FQAddress Main);
int ProcParNetJam (char *Token, struct S_FQAddress Main);
int ProcParOutboundDir (char *Token, struct S_FQAddress Main);
int ProcParQQQOutboundDir (char *Token, struct S_FQAddress Main);
int ProcParLogFilename (char *Token, struct S_FQAddress Main);
int ProcParRecode (char *Token, struct S_FQAddress Main);
int ProcParPassword (char *Token, struct S_FQAddress Main);
int ProcParInbound (char *Token, struct S_FQAddress Main);
int ProcParInclude (char *Token, struct S_FQAddress Main);
int ProcParRoute (char *Token, struct S_FQAddress Main);
int ProcParPacket2 (char *Token,struct S_FQAddress Main);
int ProcParMain (char *Token,struct S_FQAddress Main);
int ProcParAKA (char *Token,struct S_FQAddress Main);
int ProcParCheckFile (char *Token, struct S_FQAddress Main);
int ProcParDefine (char *Token,struct S_FQAddress Main);
int ProcParProductInfo (char *Token,struct S_FQAddress Main);
int ProcParFEConfig (char *Token,struct S_FQAddress Main);
int ProcParFastechoPack (char *Token,struct S_FQAddress Main);

// misc. other functions
extern long lengthoffile(FILE *f);

