#ifndef UNIX
#include <io.h>
#endif

class C_FileRead
{
	int inited;
	FILE *readfile;
	long filesize,currentpos;
	char *buffer;
	long BytesOnBuffer;
public:
	int OpenFile (char *filename);
	void CloseFile (void);
	size_t ReadBytes (void *storage,size_t NumberOfBytes);
	int ReadUpTo (void *storage,char *lookfor);
	long FileSize (void);
	C_FileRead (void);
	~C_FileRead (void);
};

long C_FileRead::FileSize (void)
{
	if (inited==0)
		return 0x7FFFFFFFL;
	else
                return lengthoffile(readfile);          
}

C_FileRead::C_FileRead (void)
{
	inited=0;
	BytesOnBuffer=0;
}

C_FileRead::~C_FileRead (void)
{
	if (inited)
	{
		free (buffer);
		fclose (readfile);
	}
}

void C_FileRead::CloseFile (void)
{
	if (inited)
	{
		free (buffer);
		fclose (readfile);
	}
	inited=0;
}

int C_FileRead::OpenFile (char *filename)
{
	if (inited==1)
		return EBH_ALREADYOPEN;
	buffer=(char *) malloc (BUFFER_SIZE);
	if (buffer==NULL)
		return NOMEMORY;
	readfile=fopen (filename,"rb");
	if (readfile==NULL)
		return EBH_OPENFAIL;
	currentpos=0;
	inited=1;
	filesize=FileSize(); /* filelength (fileno (readfile)); */
	return SUCCESS;
}

int C_FileRead::ReadUpTo (void *storage,char *lookfor)
{
	char *posit,*posnow;
	long save;
	save=ftell (readfile);
	memset (buffer,0,BUFFER_SIZE);
	if (fread (buffer,1,BUFFER_SIZE,readfile)==0)
		return EBH_EOF;
	posit=NULL;
	while (*lookfor!='\0')
	{
		posnow=strchr (buffer,*lookfor);
		if (posit==NULL)
			posit=posnow;
		else
			if (posnow!=NULL)
				posit=(posit<posnow)?posit:posnow;
		lookfor++;
	}
	if (posit!=NULL)
	{
		*posit=0;
		strcpy ((char *) storage,buffer);
		fseek (readfile,save+(posit-buffer+1),SEEK_SET);
		currentpos=ftell (readfile);
	}
	else
	{
		buffer[BUFFER_SIZE-1]=0;
		strcpy ((char *) storage,buffer);
		return EBH_SEARCHFAILED;
	}
	return SUCCESS;
}

size_t C_FileRead::ReadBytes (void *storage,size_t NumberOfBytes)
{
	size_t value;
	value=fread (storage,1,NumberOfBytes,readfile);
	currentpos+=value;
	return (value);
}
