// Configuration file handling - this class provides basic
// routines to read the file.
#include <stdio.h>
#include <string.h>
#include "errors.hpp"

void RepEnv (char *s)
{
	char *t,*p1,*p2,*c,*c2,*e;
	t=(char *) malloc (strlen (s)+1);
	if (!t)
		return;
	strcpy (t,s);
	p1=s; p2=t;
	while (*p2)
	{
		strcpy (p1,p2);
		c=strchr (p2,'%');
		if (c)
			c2=strchr (c+1,'%');
		else
			c2=NULL;
		if (!c2)
		{
			p1+=strlen (p1); p2+=strlen (p2); // To the final zero
		}
		else
		{
			*c2=0;
			c2=c+1;
			while (*c2)
			{
				*c2=toupper (*c2);
				*c2++;
			}
			e=getenv (c+1);
			if (e)
			{
				strcpy (p1+(unsigned) (c-p2),e);
				p1=p1+strlen (p1);
				p2=c2+1;
			}
			else
			{
				p1=p1+(unsigned) (c-p2)+1;
				p2=c+1;
				*c2='%';
			}
		}
	}
	free (t);
}

class CConfigHandler
{
private:
	FILE *FileHandle; // Handler, as provided by fopen()
	int Inited; // Signal if a config file is open or not
public:
	// Routines
	CConfigHandler();
	int Open (char *FileName);
	int Close ();
	int ReadLine(char *ReadBuffer,long *linecounter=NULL);
	// Variables
	char ConfigFileName[128]; // Name of the current config file, if any
};

// Open route (configuration) file.
int CConfigHandler::Open (char *FileName)
{
	if (FileHandle!=NULL)
		return ECH_ALREADYOPEN;
	FileHandle=fopen (FileName,"rt");
	if (FileHandle==NULL)
		return ECH_OPENFAILED;
	Inited=1;
	strcpy (ConfigFileName,FileName);
	return SUCCESS;
}

// Close file
int CConfigHandler::Close ()
{
	if (!Inited)
		return NOTINITED;
	fclose (FileHandle);
	Inited=0;
	return SUCCESS;
}

// Init data
CConfigHandler::CConfigHandler()
{
	Inited=0;
	FileHandle=NULL;
}

// Get a line from the route-file
int CConfigHandler::ReadLine(char *ReadBuffer,long *linecounter)
{
	char *NL;
	char Check[5];
	do
	{
		if (fgets (ReadBuffer,255,FileHandle)==NULL)
			return ECH_EOF;
		RepEnv (ReadBuffer);
		if (linecounter!=NULL)
			(*linecounter)++;
		NL=strchr (ReadBuffer,'\n');
		if (NL)
			*NL=0; // Strip newline code
		NL=ReadBuffer;
		while (*NL!=0)
		{
			if (*NL==9) // Remove tabs
				*NL=' ';
			NL++;
		}
		while (ReadBuffer[0]==' ') // Strip leading spaces
			strcpy (ReadBuffer,ReadBuffer+1);
		memcpy (Check,ReadBuffer,4);
		Check[4]=0;
                if (cistrcmp (Check,";CFR")==0)
		{
			strcpy (ReadBuffer,ReadBuffer+4);
			while (ReadBuffer[0]==' ') // Strip leading spaces
				strcpy (ReadBuffer,ReadBuffer+1);
		}
		if ((NL=strchr (ReadBuffer+1,';'))!=NULL)
        	*NL=0; // Ignore lines after a semicolon

	}
	while (ReadBuffer[0]==';');
	return SUCCESS;
}
