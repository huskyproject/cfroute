#include <stdarg.h>

class CLog
{
	int inited;
	char LogFileName[256];
	FILE *flog;
public:
	CLog(void);
	int SetLogFileName (char *FileName);
	int OpenLog (void);
	int WriteOnLog (char *string,...);
	int CloseLog (void);
};

CLog::CLog (void)
{
	inited=0;
}

int CLog::OpenLog (void)
{
        DATETIME dt;
	char buffer[256];
	if (inited)
		return ELH_ALREADYOPEN;
	flog=fopen (LogFileName,"at");
	if (flog==NULL)
		return ELH_CANTOPEN;
	inited=1;
        DosGetDateTime (&dt);
        sprintf (buffer,
                 "%s started at %s %02d,%02d (%s) %02u:%02u:%02u\n",
                 IDENTString,Months[dt.month-1],dt.day,dt.year%100,
                 Days[GetDOW()],dt.hours,dt.minutes,dt.seconds);
        WriteOnLog ("%s",buffer);
	WriteOnLog ("------------------------------------------------------\n");
	return SUCCESS;
}

int CLog::CloseLog (void)
{
	if (inited)
		fclose (flog);
	return SUCCESS;
}
int CLog::WriteOnLog (char *string,...)
{
	char *expand;
	va_list vap;
	expand=(char *) malloc (640); // 640 bytes should be enough for everyone :-)
	if (inited==0 || expand==NULL)
		return ELH_NOTOPEN;
	va_start (vap,string);
	vsprintf (expand,string,vap);
	fputs (expand,flog);
#ifdef DEBUG
	fputs (expand,stdout);
#endif
	free (expand);
	va_end (vap);
	return SUCCESS;
}

int CLog::SetLogFileName (char *FileName)
{
	if (inited)
		return ELH_ALREADYOPEN;
	strcpy (LogFileName,FileName);
	return SUCCESS;
}

// This small classes handles a string list, to buffer log writes
class C_StringList
{
protected:
	char **Seed;
	int StringCount;
public:
	C_StringList (void);
	~C_StringList (void);
	int AddString (char *NewString);
	int InsertString (char *NewString,int pos);
	void DestroyList (void);
	char *GetString (int number);
	int GetStringCount (void);
	void RemoveFirst (void);
	void RemoveLast (void);
	int IsString (char *string, int *storage);
};

int C_StringList::IsString (char *string,int *storage)
{
	int count;
	for (count=0;count<StringCount;count++)
                if (!cistrcmp (Seed[count],string))
		{
			if (storage!=NULL)
				*storage=count;
			return 1;
		}
	return 0;
}

void C_StringList::RemoveLast (void)
{
	if (StringCount>0)
	{
		StringCount--;
		Seed=(char **) realloc (Seed,StringCount*sizeof (char *));
	}
}

void C_StringList::RemoveFirst (void)
{
	if (StringCount>0)
	{
		StringCount--;
		memcpy (Seed,Seed+1,StringCount*sizeof (char *));
		Seed=(char **) realloc (Seed,StringCount*sizeof (char *));
	}
}

int C_StringList::GetStringCount (void)
{
	return (StringCount);
}

C_StringList::C_StringList (void)
{
	StringCount=0;
	Seed=NULL;
}

void C_StringList::DestroyList (void)
{
	int count;
	if (StringCount>0)
	{
		for (count=0;count<StringCount;count++)
			free (Seed[count]);
		free (Seed);
		StringCount=0;
		Seed=NULL;
	}
}

C_StringList::~C_StringList (void)
{
	DestroyList ();
}

int C_StringList::InsertString (char *NewString,int pos)
{
	char *NewItem;
	int tomove;
	NewItem=(char *) malloc (strlen (NewString)+1);
	if (NewItem==NULL)
		return NOMEMORY;
	strcpy (NewItem,NewString);
	Seed=(char **) realloc (Seed,(1+StringCount)*(sizeof (char *)));
	if (Seed==NULL)
		return NOMEMORY;
	if (pos>StringCount)
		pos=StringCount;
	tomove=StringCount-pos;
	if (pos<StringCount)
		memmove (Seed+pos+1,Seed+pos,tomove*sizeof (char *));
	Seed[pos]=NewItem;
	StringCount++;
	return SUCCESS;
}

int C_StringList::AddString (char *NewString)
{
	char *NewItem;
	NewItem=(char *) malloc (strlen (NewString)+1);
	if (NewItem==NULL)
		return NOMEMORY;
	strcpy (NewItem,NewString);
	Seed=(char **) realloc (Seed,(1+StringCount)*(sizeof (char *)));
	if (Seed==NULL)
		return NOMEMORY;
	Seed[StringCount]=NewItem;
	StringCount++;
	return SUCCESS;
}

char *C_StringList::GetString (int number)
{
	if (number>=StringCount)
		return NULL;
    return (Seed[number]);
}

// A stringlist class with a sort function
class C_SpecialStringList: public C_StringList
{
	public:
		void Sort (void);
		int AddFileName (char *NewString);
};

int C_SpecialStringList::AddFileName (char *NewString)
{
	char *help;
        size_t l = strlen(NewString);
	help=NewString;
	while (*help)
	{
		*help=toupper (*help);
		help++;
	}
        if (l < 5 || 
            toupper(NewString[l-1] != 'G') ||
            toupper(NewString[l-2] != 'S') ||
            toupper(NewString[l-3] != 'M') ||
            NewString[l-4] != '.')
	{
#ifdef DEBUG
		printf ("C_SpecialStringList:: "
                        "Rejected string - file extension not .MSG\n");
#endif
		return ESH_INVALID;
	}
	if (strchr (NewString,DIRSEPC)==NULL)
	{
#ifdef DEBUG
		printf ("C_SpecialStringList:: Rejected string - "
                        "character "DIRSEPS" not found\n");
#endif
		return ESH_INVALID;
	} 
	return (AddString (NewString));
}

int
#ifdef VAC
_LNK_CONV
#endif
SortStuff (void const *name1,void const *name2)
{
	char *namestart;
	char assist[256],*search;
	char dir1[256],dir2[256];
        size_t l;
	int number1,number2,res;

        l = strlen(*(const char**)name1);
        memcpy ((char *) assist,*(char **)name1, l);
        assist[l - 4] = '\0'; /* cut the ".msg" extension */
	namestart=assist + l - 4;
	while (*namestart!=DIRSEPC)
		namestart--;
	*namestart=0;
	strcpy (dir1,assist);
	number1=atoi (namestart+1);

	l = strlen(*(const char**)name2);
        memcpy ((char *) assist, *(char**)name2, l);
        assist[l - 4] = '\0'; /* cut the ".msg" extension */
        namestart = assist + l - 4;
	while (*namestart!=DIRSEPC)
		namestart--;
	*namestart=0;
	strcpy (dir2,assist);
	number2=atoi (namestart+1);
	if ((res=strcmp (dir1,dir2))==0)
		return (number1-number2);
	return res;
};

void C_SpecialStringList::Sort (void)
{
#ifdef OS_2
        qsort (Seed,StringCount,sizeof (char *),&SortStuff);
#else
        qsort (Seed,StringCount,sizeof (char *),SortStuff);
#endif
}
