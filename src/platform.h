#pragma pack(1)
#if defined(OS_2)
        char *IDENTProgramName="CFR-OS/2";
#elif defined(WIN)
        #include <dos.h>
        char *IDENTProgramNAme="CFR-WIN32";
#elif defined(MSDOS)
        char *IDENTProgramName="CFR-DOS";
#elif defined(UNIX)
        char *IDENTProgramName="CFR-UNX";
#else
#error Unknown operating system!
#endif

#ifdef UNIX
#define DIRSEPS "/"
#define DIRSEPC '/'
#else
#define DIRSEPS "\\"
#define DIRSEPC '\\'
#endif

#include "dirute.h"



