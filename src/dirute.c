/*
 *  DIRUTE.H
 *  Contributes findfirst, findnext and findclose for POSIX systems.
 *  Based on code taken from Msged TE. Mostly written by Tobias Ernst.
 *  patmat() written by Sreenath Chary on Nov 29 1998.
 *  Released to the Public Domain.
 *
 *  Please note that you must call findclose!!! So it is a good idea
 *  to also include this file on non-Unix systems, because in this
 *  case, it provides a dummy findclose.
 */

#include "dirute.h"
#include <errno.h>

#if defined(UNIX)

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

static int patmat(char *raw, char *pat)
{
        int i;

        if (*pat == '\0' && *raw == '\0')
        {
                /* if it is the end of both strings, then match */
                return 1;
        }
        if (*pat == '\0')
        {
                /* if it is the end of only pat then mismatch */
                return 0;
        }
        
        /* if pattern is `*' */
        
        if (*pat == '*')
        {
                if (*(pat + 1) == '\0')
                {
                        /* if pat is just `*' then match */
                        return 1;
                }
                else
                {
                        /* else hunt for match or wild card */
                        for (i = 0; i <= strlen(raw); i++)
                        {
                                if (*(raw + i) == *(pat + 1) || 
                                    *(pat + 1) == '?')
                                {
                                        /* if found, match rest of pattern */
                                        if (patmat(raw + i + 1, pat + 2) == 1)
                                        {
                                                return 1;
                                        }
                                }
                        }
                }
        }
        else
        {
                if (*raw == '\0')
                {
                        /* we've reached the end of raw; return a mismatch */
                        return 0;
                }
                
                if (*pat == '?' || *pat == *raw)
                {
                        /* if chars match then try and match rest of it */
                        if (patmat(raw + 1, pat + 1) == 1)
                        {
                                return 1;
                        }
                }
        }
        
        /* fell through; no match found */
        return 0;
}

static int match(const char *name, const char *pattern, int attribute,
                 int mode, int rdonly)
{
        char *matpattern = strdup(pattern);
        char *matname = strdup(name);
        char *cp;
        int rc;

        if (matpattern == NULL || matname == NULL)
        {
                return 0;
        }

        if (!(attribute & FA_CASE))
        {
                for (cp=matpattern; *cp; cp++)
                {
                        *cp = toupper(*cp);
                }
                for (cp=matname; *cp; cp++)
                {
                        *cp = toupper(*cp);
                }
        }

        rc = patmat(matname, matpattern);

        free(matname);
        free(matpattern);

        if (rc)
        {
                /* the name matches, now check the other criteria */

                if (!(attribute & FA_RDONLY))    
                        if (rdonly)
                                return 0;

                if (!(attribute & FA_DIREC))
                        if (S_ISDIR(mode))
                                return 0;

                if (!(attribute & FA_HIDDEN))
                        if (name[0] == '.')
                                return 0;
        }
        
        return rc;
}

static void copy_info(struct ffblk *pffblk, struct dirent *de, 
                      struct stat *psb)
{
        struct tm *ltm;

        strcpy(pffblk->ff_name, de->d_name);
        pffblk->ff_fsize = psb->st_size;
        pffblk->ff_attrib = 0;
        if (!access(pffblk->fullname, W_OK))
        {
                pffblk->ff_attrib |= FA_RDONLY;
        }
        if (S_ISDIR(psb->st_mode))
        {
                pffblk->ff_attrib |= FA_DIREC;
        }
        if (de->d_name[0] == '.')
        {
                pffblk->ff_attrib |= FA_HIDDEN;
        }

        ltm = localtime(&(psb->st_atime));
        pffblk->ff_ftime = ((ltm->tm_sec >> 1) & 31) |
                           ((ltm->tm_min & 63) << 5) |
                           ((ltm->tm_hour & 31) << 11);
        pffblk->ff_fdate = (ltm->tm_mday & 31) |
                           (((ltm->tm_mon + 1) & 15) << 5) |
                           (((ltm->tm_year - 80) & 127) << 9);
}
 
int findfirst(const char *filename, struct ffblk* pffblk, int attribute)
{
        char *p;
        int fin = 0;
        struct stat sb;

        p = strrchr(filename, '/');
        if (p == NULL)
        {
                strcpy(pffblk->firstbit, ".");
                strcpy(pffblk->lastbit, filename);
        }
        else
        {
                memcpy(pffblk->firstbit, filename, p - filename);
                pffblk->firstbit[p - filename] = '\0';
                strcpy(pffblk->lastbit, p + 1);
        }

        if (*pffblk->firstbit)
                pffblk->dir = opendir(pffblk->firstbit);
        else
                pffblk->dir = opendir("/");

        if (pffblk->dir == NULL)
        {
                return -1;
        }

        while (!fin)
        {
                pffblk->de = readdir(pffblk->dir);
                if (pffblk->de == NULL)
                {
                        closedir(pffblk->dir);
                        pffblk->dir = NULL;
                        return -1;
                }

                strcpy(pffblk->fullname, pffblk->firstbit);
                strcat(pffblk->fullname, "/");
                strcat(pffblk->fullname, pffblk->de->d_name);

                if (stat(pffblk->fullname, &sb))
                {
                        closedir(pffblk->dir);
                        pffblk->dir = NULL;
                        return -1;
                }
        
                if (match(pffblk->de->d_name, pffblk->lastbit,
                          attribute, sb.st_mode, 
                          access(pffblk->fullname, W_OK)))
                        fin = 1;
        }
        copy_info(pffblk, pffblk->de, &sb);
        pffblk->attribute_mask = attribute;
        return 0;
}

int findnext(struct ffblk *pffblk)
{
        int fin = 0;
        struct stat sb;

        while (!fin)
        {
                pffblk->de = readdir(pffblk->dir);
                if (pffblk->de == NULL)
                {
                        closedir(pffblk->dir);
                        pffblk->dir = NULL;
                        return -1;
                }
    
                strcpy(pffblk->fullname, pffblk->firstbit);
                strcat(pffblk->fullname, "/");
                strcat(pffblk->fullname, pffblk->de->d_name);

                if (stat(pffblk->fullname, &sb))
                {
                        closedir(pffblk->dir);
                        pffblk->dir = NULL;
                        return -1;
                }
                
                if (match(pffblk->de->d_name, pffblk->lastbit,
                          pffblk->attribute_mask, sb.st_mode, 
                          access(pffblk->fullname, W_OK)))
                        fin = 1;
        }
        copy_info(pffblk, pffblk->de, &sb);
        return 0;
}

int findclose(struct ffblk *pffblk)
{
        if (pffblk->dir != NULL)
                closedir(pffblk->dir);
        pffblk->dir = NULL;
        return 0;
}

/* The adaptcase routine behaves as follows: It assumes that pathname
   is a path name which may contain multiple dashes, and it assumes
   that you run on a case sensitive file system but want / must to
   match the path name insensitively. adaptcase takes every path
   element out of pathname and uses findfirst to check if it
   exists. If it exists, the path element is replaced by the exact
   spelling as used by the file system. If it does not exist, it is
   converted to lowercase.  This allows you to make you program deal
   with things like mounts of DOS file systems under unix

   Return value is 1 if the file exists and 0 if not.
*/

int adaptcase(char *pathname)
{
        int i,j, found=1;
        char buf[FILENAME_MAX + 1];
        char buf2[FILENAME_MAX + 1];
        char *cp;
        struct ffblk ffblk;

        if (!*pathname)
                return 0;

        j = 0; i = 0;
   
        while (pathname[i])
        {
                if (pathname[i] == '/')
                {
                        buf[i] = tolower(((int)(pathname[i]))); i++;
                }
                j = i;
                for (; pathname[i] && pathname[i]!='/'; i++)
                        buf[i] = tolower(((int)(pathname[i])));
                buf[i] = '\0';

                if (!fexist(buf))
                {
                    if (!findfirst(buf, &ffblk, FA_DIREC | FA_RDONLY |
                                   FA_ARCH | FA_HIDDEN))
                    {
                        /* file exists, take over it's name */
                        
                        if (ffblk.fullname[0] == '.' &&
                            ffblk.fullname[1] == '/' && 
                            buf[0] != '.' && buf[1] != '/')
                                cp = ffblk.fullname + 2;
                        else
                                cp = ffblk.fullname;
                        assert(strlen(buf) == strlen(cp));
                        strcpy(buf , cp);
                    }
                    else
                    {
                        /* file does not exist - so the rest is brand new and
                           should be converted to lower case */
                        
                        for (i = j; pathname[i]; i++)
                            buf[i] = tolower(pathname[i]);
                        buf[i] = '\0';
                        findclose(&ffblk);
                        found = 0;
                        break;
                    }
                    findclose(&ffblk);
                }
        }
        assert(strlen(pathname) == strlen(buf));
        strcpy(pathname, buf);
        return found;
}
  
#endif

#ifndef UNIX
#include <stdio.h>
#include <stdlib.h>
#endif

int file_exists(char *fn)
{
        FILE *f;

        f = fopen(fn, "r");
        if ((f == NULL) && (errno == ENOENT))
                return 0;
        if (f != NULL) 
                fclose(f);
        return 1;
}

#if defined (TEST)
int main(int argc, char **argv)
{
        struct ffblk ffblk; int rc;
        
        if (argc < 2)
                return 0;

/*        rc = findfirst(argv[1], &ffblk, FA_RDONLY | FA_DIREC |
                       FA_HIDDEN | FA_ARCH);
        while (!rc)
        {
                printf("%s \t %ld\n", ffblk.ff_name, ffblk.ff_fsize);
                rc = findnext(&ffblk);
        }
        findclose(&ffblk); */
        printf ("%s\n", adaptcase(argv[1]));

        return 0;
}

#endif
