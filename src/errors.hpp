// Return codes for parameter processors
#define SUCCESS             0
#define WARNING             1
#define FATAL               2
#define NEEDEXTRAPAR        3
#define RET_SUCCESS			4

// Common error codes
#define OUTRANGE			4
#define NOTINITED   	    5
#define NOMEMORY			6

// Config. file handler errors.
#define ECH_ALREADYOPEN 	1 // There is already a config. file open
#define ECH_OPENFAILED  	2 // Unable to open configuration file
#define ECH_EOF         	4 // Reached the end of file
#define ECH_NONETMAILDIR     5    // No netmail directory
#define ECH_NOOUTBOUNDDIR    6    // No outbound directory
#define ECH_NOQQQOUTBOUNDDIR 7    // FastechoPack && ! QQQOutbound
#define ECH_NOCONFIG        8     // No config file specified
#define ECH_UNKNOWNARG      9     // Unknown parameter in command line
#define ECH_FORCEOUT        10    // Generic value to force a termination

// String handling errors
#define SH_ZERO        		21 // Source string is zero-length.

// Token Types
#define TT_UNKNOWN     		30 // Unknown
#define TT_ROUTETO     		31 // Command: Route-to
#define TT_MAIN        		32 // Command: Main-address
#define TT_AKA         		33 // Command: AKA
#define TT_ADDRESS     		34 // Address
#define TT_FROM             35 // From
#define TT_EXCEPT           36 // Except
#define TT_NETMAILDIR       37 // Command: Netmail directory
#define TT_LOGFILENAME      38 // Command: Log filename
#define TT_TOPDOWN          39 // Topdown priority - first match wins
#define TT_CRASH            40 // Crash attribute
#define TT_HOLD             41
#define TT_DIR              42
#define TT_NORMAL           44
#define TT_MYZONES          45
#define TT_MYNETS           46
#define TT_MYPOINTS         47
#define TT_PARAMETER        48
#define TT_DIRECT           49
#define TT_ROUTEMAIL        50
#define TT_ROUTEFILES       51
#define TT_DIRECTMAIL       52
#define TT_DIRECTFILES      53
#define TT_NOROUTE          54
#define TT_NOROUTEMAIL      55
#define TT_NOROUTEFILES     56
#define TT_OUTBOUND         57
#define TT_PVT              58
#define TT_HLD              59
#define TT_CRA              60
#define TT_KS               61
#define TT_SNT              62
#define TT_RCV              63
#define TT_AS               64
#define TT_ZON              65
#define TT_HUB              66
#define TT_FIL              67
#define TT_FRQ              68
#define TT_IMM              69
#define TT_XMA              70
#define TT_KFS              71
#define TT_TFS              72
#define TT_LOK              73
#define TT_RRQ              74
#define TT_CFM              75
#define TT_HIR              76
#define TT_COV              77
#define TT_SIG              78
#define TT_LET              79
#define TT_FAX              80
#define TT_FPU              81
#define TT_PACKET2          82
#define TT_PASSWORD         83
#define TT_PASSWORDPARAM    84
#define TT_DOW              85
#define TT_ENCRYPTED        86
#define TT_NONENCRYPTED     87
#define TT_INCLUDE          88
#define TT_NOPACK      		89
#define TT_NOPACKMAIL		90
#define TT_NOPACKFILES		91
#define TT_CHECKFILE 		92
#define TT_DEFINE			93
#define TT_ENDDEFINE        94
#define TT_ASSUMEPOINTS		95
#define TT_ASSUMEWAYPOINTS  96
#define TT_EOLENDSCOMMAND   97
#define TT_LOCAL			98
#define TT_VIABOSSHOLD		99
#define TT_VIABOSSDIRECT   100
#define TT_IGNOREBSY	   101
#define TT_PRODUCTINFO	   102
#define TT_KILLINTRANSIT   103
#define TT_LOGPATH         104
#define TT_FASTECHOPACK    105
#define TT_NODOMAINDIR     106
#define TT_FECONFIG		   107
#define TT_NETSQUISH	   108
#define TT_IGNOREUNKNOWNATTRIBS 109
#define TT_QQQOUTBOUND     110
#define TT_QQQALL          111
#define	TT_RECODEFILENAME  112
#define TT_NETJAM          113
#define TT_IMMEDIATE       114
#define TT_NOLOOPROUTE     115

// Main() errors
#define ENOMAIN             95 // Main address not specified
#define EMAIN_FOUNDFLAG     96 // Found a file that prevents CFR from running
#define EMAIN_CFRACTIVE		97

// Netmail handling errors
#define ENH_OPENFAIL       101
#define ENH_SHORTFILE      103
#define ENH_CORRUPTED      104
#define ENH_PROCESSED      105
#define ENH_DELAYED  	   106
#define ENH_KILLED		   107

// Log handling errors
#define ELH_ALREADYOPEN    111
#define ELH_CANTOPEN       112
#define ELH_NOTOPEN        113

// Buffered file handler errors
#define EBH_OPENFAIL       121
#define EBH_ALREADYOPEN    122
#define EBH_SEARCHFAILED   124
#define EBH_EOF            125

// Address handling errors
#define EAD_TWOMAIN    	   141 // Attempt to store two main addresses
#define EAD_NOMAIN     	   142 // No main address
#define EAD_WILDCARDS  	   143 // Address with wildcards
#define EAD_ZERO       	   144 // Zero-length address
#define EAD_CANTRESOLVE    145 // Unable to get a FQA
#define EAD_INVALID    	   146 // Nonsense address, such as X:/Y
#define EAD_OURZONE        149
#define EAD_OURNET         150
#define EAD_FOREIGN        151

// Macro handler(s)
#define EMH_NOMAIN         161
#define EMH_GENERAL        162 // Failure at route handler
#define EMH_ALREADYDEFINED 163
// Others


// MSG -> PKT conversion
#define ECONV_OPENFAILIN   181
#define ECONV_OPENFAILOUT  182
#define ECONV_SHORTIN      183

// Via handling errors
#define EVH_TWOVIA	   		262
#define EVH_NO_VIAHANDLER   263 // Requested to process an address without seting up a via first
#define EVH_CANTRESOLVE     264 // Via address has wildcards
#define EVH_NOVIA           266
#define EVH_INVALID         267
#define EVH_LOCKED          268
#define EVH_DIRECT          269
#define EVH_NOROUTE         270
#define EVH_NONSENSE        271
#define EVH_NOPACK			272

// Route types
#define ST_NOTOUCH          300
#define ST_ROUTE            301
#define ST_DIRECT           302
#define ST_BOSS				303

// Subject to file errors
#define ESTF_FAILURE        304

// FlagAsSent
#define EFAS_OPENFAILED     306
#define EFAS_CANTWRITE      307

// Password handler
#define EPH_TOOLONG         312
#define EPH_INVALID         313
#define EPH_NOTFOUND        315

// String handler
#define ESH_INVALID         322

// Special errors (high numbers to avoid collisions)
#define SPECIAL_OUTRANGE 0xFFFE

