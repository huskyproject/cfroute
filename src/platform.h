#ifndef __PLATFORM_H
#define __PLATFORM_H

/* Map Husky defines to cfroute defines */
#if defined(OS2) && !defined(OS_2)
#define OS_2
#endif
#if defined(__EMX__) && !defined(EMX)
#define EMX
#endif
#if (defined(WINNT) || defined(__WINNT__) || defined(__NT__) || defined(NT)) \
    &&(!defined(WIN))
#define WIN
#endif
#if defined(DOS) && !defined(MSDOS)
#define MSDOS
#endif

#ifdef DEFINE_IDENT
#if defined(OS_2)
        char *IDENTProgramName="CFR-OS/2";
#elif defined(WIN)
        #include <dos.h>
        char *IDENTProgramName="CFR-WIN32";
#elif defined(MSDOS)
        char *IDENTProgramName="CFR-DOS";
#elif defined(UNIX)
        char *IDENTProgramName="CFR-UNX";
#else
#error Unknown operating system!
#endif
#endif

#ifdef UNIX
#define DIRSEPS "/"
#define DIRSEPC '/'
#else
#define DIRSEPS "\\"
#define DIRSEPC '\\'
#endif

#include "dirute.h"

#endif
