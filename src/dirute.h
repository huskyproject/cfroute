/*
 *  DIRUTE.H
 *  Contributes findfirst, findnext and findclose for POSIX systems.
 *  Based on code taken from Msged TE. Written by Tobias Ernst.
 *  Released to the public domain.
 *  Please note that you must call findclose!!! So it is a good idea
 *  to also include this file on non-Unix systems, because in this
 *  case, it provides a dummy findclose.
 */

#ifndef __DIRUTE_H__
#define __DIRUTE_H__

#ifndef UNIX
#ifndef OS_2
#define findclose(x) (x)
#define FA_CASE 0
#include <dir.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
int file_exists(char *);
#ifdef __cplusplus
}
#endif
#define adaptcase file_exists
#else

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>


#include <stdio.h>             /* FILENAME_MAX */

#ifdef __cplusplus
extern "C" {
#endif

struct ffblk
{
        /* public data */
        long           ff_fsize;
        unsigned long  ff_attrib;
        unsigned short ff_ftime;
        unsigned short ff_fdate;
        char           ff_name[FILENAME_MAX + 1];

        /* private data */
        DIR           *dir;
        struct dirent *de;
        int            attribute_mask;
        char           firstbit[FILENAME_MAX + 1];
        char           lastbit [FILENAME_MAX + 1];
        char           fullname[FILENAME_MAX + 1];
};

#define FA_RDONLY  1  /* also find files that are read only         */
#define FA_ARCH    0  /* also find "archived" files (dummy!)        */
#define FA_HIDDEN  4  /* also find hidden (dotted) files            */
#define FA_SYSTEM  4  /* also find "system" files (dummy!)          */
#define FA_DIREC   8  /* also find subdirectories                   */
#define FA_CASE   16  /* enforce case sensitivity on Unix (without  */
                      /* this flag, the search is case INSENSITIVE) */

int findfirst (const char *, struct ffblk*, int);
int findnext  (struct ffblk*);
int findclose (struct ffblk*);
int adaptcase (char *);

#ifdef __cplusplus
}  /* end of extern "C" */
#endif

#endif /* UNIX     */
#endif /* dirute.h */


