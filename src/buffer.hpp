#ifndef __BUFFER_HPP
#define __BUFFER_HPP

#ifndef UNIX
#include <io.h>
#endif
#include <stdio.h>

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

#endif
