#ifndef UNIX
#include <conio.h>
#else
#define getch getchar
#ifdef __FreeBSD__
#include <sys/time.h>
#else
#include <utime.h>
#endif
#endif
#include <stdlib.h> // <malloc.h>
#include "buffer.cpp"
// #include "datetime.cpp"
#include "dirute.h"

typedef struct
{
	unsigned Private: 1; //
	unsigned Crash: 1;   //
	unsigned Recd: 1; //
	unsigned Sent: 1; //
	unsigned FileAttached: 1;
	unsigned InTransit: 1;
	unsigned Orphan: 1;
	unsigned KillSent: 1; //
	unsigned Local: 1;
	unsigned Hold: 1;   //
	unsigned Unused: 1; // should be direct !?
	unsigned FileRequest: 1; //
	unsigned ReturnReceiptRequest: 1;
	unsigned IsReturnReceipt: 1;
	unsigned AuditRequest: 1;
	unsigned FileUpdateReq: 1;
} S_Attrib;

typedef struct
{
	char FromUserName[36];
	char ToUserName[36];
	char Subject[72];
	char DateTime[20];
	unsigned short TimesRead;
	unsigned short DestNode;
	unsigned short OrigNode;
	unsigned short Cost;
	unsigned short OrigNet;
	unsigned short DestNet;
	char Fill[8];
	unsigned short ReplyTo;
	union
	{
                S_Attrib bits;
		unsigned short number;
	} Attribute;
	// short Attribute;
	unsigned short NextReply;
} S_MSG;

typedef struct
{
	unsigned ArchiveSent: 1;
	unsigned KillFileSent: 1;
	unsigned Direct: 1;
	unsigned Zonegate: 1;
	unsigned Hub: 1;
	unsigned Immediate: 1;
	unsigned XMA: 1;
	unsigned Lock: 1;
	unsigned Truncate: 1;
	unsigned HiRes: 1;
	unsigned CoverLetter: 1;
	unsigned Signature: 1;
	unsigned LetterHead: 1;
	unsigned Fax: 1;
	unsigned ForcePickup: 1;
} S_Attrib2;

typedef struct
{
	char FromUserName[36];
	char ToUserName[36];
	char Subject[72];
	char OrigDateTime[20];
	struct
	{
		short day,month,year;
	} date;
	struct
	{
		short hour,minute,second;
	} time;
	long ElapsedDays;
	byte DateParsed;
	union
	{
                S_Attrib bits;
		unsigned short number;
	} attrib;
	union
	{
                S_Attrib2 bits;
		unsigned short number;
	} attrib2;
	S_FQAddress Origin,Destination;
	S_FQAddress TransitAddr,*WishAddresses;
	short WishAddrCount;
	byte DetectedLoop;
	long MessageSize;
	byte Encrypted;
	short ProcessedHere;
	unsigned MSGIDZone;
} S_Visu;

typedef struct
{
	unsigned short Signature;
	unsigned short OrigNode;
	unsigned short DestNode;
	unsigned short OrigNet;
	unsigned short DestNet;
	union
	{
                S_Attrib bits;
		unsigned short number;
	} Attrib;
	unsigned short Cost;
	char DateTime[20];
} S_Packed;

typedef struct
{
	unsigned short OrigNode;
	unsigned short DestNode;
	unsigned short Year;
	unsigned short Month;
	unsigned short Day;
	unsigned short Hour;
	unsigned short Minute;
	unsigned short Second;
	unsigned short Baud;
	unsigned short Signature;
	unsigned short OrigNet;
	unsigned short DestNet;
	char ProdCode,SerialNo;
	char Password[8];
	unsigned short OrigZone;
	unsigned short DestZone;
	union
	{
		char Fill[20];
		struct
		{
			char Fill[2];
			unsigned short CapabilitySwapped;
			char PrdCodH;
			char PVMinor;
			unsigned short Capability;
			unsigned short OrigZone;
			unsigned short DestZone;
			unsigned short OrigPoint;
			unsigned short DestPoint;
			char ProdData[4];
		} t2plus;
	} dif;
	char EndOfPKT[2];
} S_PKT;

int FlagAsSent (char *path)
{
	FILE *msg;
	S_MSG header;
	msg=fopen (path,"r+b");
	if (msg==NULL)
		return EFAS_OPENFAILED;
        fread (&header,1,sizeof (S_MSG),msg);
	fseek (msg,0,SEEK_SET);
	header.Attribute.bits.Sent=1;
        if (fwrite (&header,1,sizeof (S_MSG),msg)!=sizeof (S_MSG))
	{
		fclose (msg);
		return EFAS_CANTWRITE;
	}
	fclose (msg);
	return SUCCESS;
}

int SubjectToFile (char *Subject,char *savepath,char *extattach,int Truncate,int Delete)
{
	FILE *out;
	char complete[256];
	strcpy (complete,savepath);
	strcat (complete,extattach);
        adaptcase(complete);
	out=fopen (complete,"r+a");
	if (out==NULL)
	{
		out=makedirandfopen (complete,"w+a");
		if (out==NULL)
			return ESTF_FAILURE;
	}
	fseek (out,0,SEEK_END);
	while (Subject[0])
	{
		GetAndStripToken (Subject,complete+1);
		if (Delete)
			complete[0]='^';
		else
		if (Truncate)
			complete[0]='#';
		else
			strcpy (complete,complete+1);
		fprintf (out,"%s\n",complete);
	}
	fclose (out);
	return SUCCESS;
}

void StripPath (char *source, char *storage)
{
	char token[80],target[80],*c;
	strcpy (target,source);
	storage[0]=0;
	while (strlen (target))
	{
		GetAndStripToken (target,token);
		c=token+strlen (token)-1;
		while (c>=token && *c!=DIRSEPC && *c!=':')
			c--;
		strcat (storage,c+1);
		strcat (storage," ");
	}
	while (strlen (storage) && storage[strlen (storage)-1]==' ')
		storage[strlen (storage)-1]=0;
}

int SignNet (FILE *onet,S_FQAddress addressinnet)
{
	DATETIME dt;
	char point[10];
	DosGetDateTime (&dt);
	char ViaLine[120];
	S_FQAddress OurAddress;
	AddressHandler.AKAMatch (addressinnet,&OurAddress);
	if (OurAddress.Point==0)
		point[0]=0;
	else
		sprintf (point,".%hu",OurAddress.Point);
	sprintf (ViaLine,"%cVia %hu:%hu/%hu%s%s%s @%04hu%02hu%02hu.%02hu%02hu%02hu %s\r",
		1,OurAddress.Zone,OurAddress.Net,OurAddress.Node,point,
						OurAddress.Domain[0]?"@":"",OurAddress.Domain,dt.year,
						dt.month,dt.day,dt.hours,dt.minutes,dt.seconds,
						IDENTString
						);
	fwrite (ViaLine,strlen (ViaLine)+1,1,onet);
	return SUCCESS;
}

int MSGToPKT (char *MSGPath,char *PKTPath,char *ext,S_FQAddress via,S_FQAddress
              OurAKA,S_FQAddress FinalDest,int UseT2, int is_qqq)
{
	FILE *in,*out;
	S_MSG headerin;
	S_Packed headerout;
	S_PKT headerPKT;
	DATETIME dt;
	char *buffer,completepath[256];
	char NoPathSubject[80],lasttwo[2];
	long ToCopy;
	size_t BufferSize,read;
	in=fopen (MSGPath,"rb");
	if (in==NULL)
		return ECONV_OPENFAILIN;
	strcpy (completepath,PKTPath);
	strcat (completepath,ext);
        adaptcase(completepath);
	out=fopen (completepath,"r+b");
	if (out==NULL)
	{
		out=makedirandfopen (completepath,"w+b");
		if (out==NULL)
		{
			fclose (in);
			return ECONV_OPENFAILOUT;
		}
		// The file doesn't exist yet - we have to create it and add
		// a header
		DosGetDateTime (&dt);
		headerPKT.OrigNode=OurAKA.Node;
		headerPKT.DestNode=via.Node;
		headerPKT.Year=dt.year;
		headerPKT.Month=dt.month;
		headerPKT.Day=dt.day;
		headerPKT.Hour=dt.hours;
		headerPKT.Minute=dt.minutes;
		headerPKT.Second=dt.seconds;
		headerPKT.Baud=0;
		headerPKT.Signature=2;
		headerPKT.OrigNet=OurAKA.Net;
		headerPKT.DestNet=via.Net;
		headerPKT.ProdCode=0;
		headerPKT.SerialNo=0;
		memset (&headerPKT.Password,0,8);
		PasswordHandler.GetPassword (via,headerPKT.Password);
		headerPKT.OrigZone=OurAKA.Zone;
		headerPKT.DestZone=via.Zone;
		memset (&headerPKT.dif.Fill,0,20);
		memset (&headerPKT.EndOfPKT,0,sizeof (headerPKT.EndOfPKT));
		if (UseT2==0) // Add type2+ data
		{
			headerPKT.dif.t2plus.CapabilitySwapped=256;
			headerPKT.dif.t2plus.Capability=1;
			headerPKT.dif.t2plus.PrdCodH=1;
			headerPKT.dif.t2plus.PVMinor=1;
			headerPKT.dif.t2plus.OrigZone=OurAKA.Zone;
			headerPKT.dif.t2plus.DestZone=via.Zone;
			headerPKT.dif.t2plus.OrigPoint=OurAKA.Point;
			headerPKT.dif.t2plus.DestPoint=via.Point;
		}
                fwrite (&headerPKT,1,sizeof (S_PKT),out);
	}
        if (fread (&headerin,1,sizeof (S_MSG),in)<sizeof (S_MSG))
	{
		fclose (in);
		fclose (out);
		return ECONV_SHORTIN;
	}

	/* We truncate the last 2 bytes of the .PKT if they are both zero.
	   Otherwise it is likely that the .PKT is damaged and it is better
	   not to destroy anything on it */
	fseek (out,-2,SEEK_END);
	fread (lasttwo,1,2,out);
	if (lasttwo[0]==0 && lasttwo[1]==0)
		fseek (out,-2,SEEK_END);

	headerout.Signature=2;
	headerout.OrigNode=headerin.OrigNode;
	headerout.OrigNet=headerin.OrigNet;
	headerout.DestNode=headerin.DestNode;
	headerout.DestNet=headerin.DestNet;
	headerout.Attrib.number=headerin.Attribute.number;
	// Note that the InTransit and local flags are stripped (!!)
	headerout.Attrib.bits.InTransit=0;
	headerout.Attrib.bits.Local=0;
	headerout.Cost=0;
	memcpy (&headerout.DateTime,&headerin.DateTime,20);
        fwrite (&headerout,1,sizeof (S_Packed),out);
	fwrite (&headerin.ToUserName,1,strlen (headerin.ToUserName)+1,out);
	fwrite (&headerin.FromUserName,1,strlen (headerin.FromUserName)+1,out);
	if (headerin.Attribute.bits.FileAttached==1)
		StripPath (headerin.Subject,NoPathSubject);
	else
		strcpy (NoPathSubject,headerin.Subject);
	fwrite (NoPathSubject,1,strlen (NoPathSubject)+1,out);
        ToCopy=lengthoffile(in)-sizeof (S_MSG);
	BufferSize=(ToCopy>32767)?32767:(size_t) ToCopy;
	while (((buffer=(char *) malloc (BufferSize)) == NULL) && (BufferSize>1024))
		BufferSize = (BufferSize / 10) * 9;
	if (buffer==NULL)
	{
		fclose (in);
		fclose (out);
		return NOMEMORY;
	}
	/* Note that in the body copy loop we copy up to, but not including,
	   the first zero. If there is something beyond that zero, if won't
	   be copied, because that would result in a damaged .PKT. */
	if (ToCopy>0)
	{
		while (ToCopy>0)
		{
			if (ToCopy>(long)((unsigned long)BufferSize))
				read=fread (buffer,1,BufferSize,in);
			else
				read=fread (buffer,1,(size_t) ToCopy,in);
			if (strlen (buffer)<read)
			{
				read=strlen (buffer);
				ToCopy=read;
			}
			fwrite (buffer,1,read,out);
			ToCopy-=read;
		}
		if (buffer[read-1]!='\r')
			fwrite ("\r",1,1,out);
	}
	/* Add our via line - note that it has a zero to end the message */
	SignNet (out,FinalDest);
	/* End of PKT 00 00 - added UNLESS we are generating .QQQs, to
	   follow Tobias Burchhardt's specs */
	if (!is_qqq)
	{
		memset (&headerPKT.EndOfPKT,0,sizeof (headerPKT.EndOfPKT));
		fwrite (&headerPKT.EndOfPKT,1,sizeof (headerPKT.EndOfPKT),out);
	}
	free (buffer);
	fclose (in);
	fclose (out);
	return (SUCCESS);
}

char *ToNextWord (char *begin)
{
	int sp=0;
	if (*begin=='\0')
		return NULL;
	if (*begin==' ')
		sp=1;
	while (*begin && *begin==' ') // Skip all spaces
		begin++;
	if (*begin=='\0')
		return NULL;
	if (sp)
		return begin; // We are a the beginning of a new world
	while (*begin && *begin!=' ') // To end of word
		begin++;
	while (*begin && *begin==' ') // Skip all spaces
		begin++;
	if (*begin=='\0')
		return NULL;
	return begin;
}

// Scan begin for a possible address
char *ScanForAddress (char *begin)
{
	char *store;
	char *s,*as;
	int qualify=0;
	S_FQAddress Address;
	store=(char *) malloc (strlen (begin)+1);
	if (!store)
		return NULL;
	s=begin;
	while (qualify==0 && s)
	{
		strcpy (store,s);
		as=strchr (store,' ');
		if (as) // Keep only to end of word
			*as=0;
		as=strchr (store,'@'); // We don't mind about domain stuff
		if (as)
			*as=0;
		if (GetFullQualifiedAddress (store,&Address,NULL)==SUCCESS)
			qualify=1;
		else
			s=ToNextWord (s);
	}
	free (store);
	return s;
}

/* Process line for kludges, vias, etc */
void ProcessLine (char *line,S_Visu *storage,C_StringList *SL_Via, C_StringList *SL_Path)
{
	struct S_FQAddress FQA;
	C_FileRead FHandler;
	char *first,*search,*possible,newpath[26];
	char Parsing[21];
	first=(char *) line;
	if (first[0]==1) // Kludge!
	{
		if (strstr (first+1,"ENC")==first+1)
			storage->Encrypted=1;
		else
		if (strstr (first+1,"Via")==first+1) // Already processed...
		{
 			possible=ScanForAddress (first);
			if (possible!=NULL)
			{
				GetFullQualifiedAddress (possible,&FQA,NULL);
				if (SL_Path!=NULL)
				{
					sprintf (newpath,"%u:%u/%u.%u",FQA.Zone,
						FQA.Net,FQA.Node,FQA.Point);
					if (SL_Path->GetStringCount ()==0)
							SL_Path->AddString (newpath);
				else
						if (strcmp (SL_Path->GetString (SL_Path->GetStringCount()-1),newpath))
							SL_Path->AddString (newpath);
				}
				if (AddressHandler.IsLocalAKA (FQA))
				{
					storage->ProcessedHere=1;
					if (!RecordViaLines)
						SL_Via->DestroyList ();
				}
				else
					/* If there was a previous Via line with one of
					   our addresses but current isn't local, then
					   we have a loop */
					if (storage->ProcessedHere==1)
						storage->DetectedLoop=1;
			}
			if (SL_Via!=NULL)
				SL_Via->AddString (first+1);
		}
		else
		if (strstr (first+1,"MSGID")==first+1) // MSGID
		{
			search=first+8;
			while (*search)
			{
				if (!isdigit (*search))
					break;
				search++;
			}
			if (*search==':') // It is a zone
			{
				*search=0;
				storage->MSGIDZone=atoi (first+8);
			}
		}
		else
		if (strstr (first+1,"TOPT")==first+1) // Destination point
		{
			if (isdigit (*(first+6))) // Always should but...
			{
				search=first+6;
				while (isdigit (*search))
					search++;
				*search=0;
				storage->Destination.Point=atoi (first+6);
			}
		}
		if (strstr (first+1,"FMPT")==first+1) // Destination point
		{
			if (isdigit (*(first+6))) // Always should but...
			{
				search=first+6;
				while (isdigit (*search))
					search++;
				*search=0;
				storage->Origin.Point=atoi (first+6);
			}
		}
		if (strstr (first+1,"INTL")==first+1) // Zones
		{
			search=strchr (first+6,' ');
			if (search)
			{
				*search=0;
				if (GetFullQualifiedAddress (first+6,&FQA,NULL)==SUCCESS)
				{
					storage->Destination.Zone=FQA.Zone;
					storage->Destination.Net=FQA.Net;
					storage->Destination.Node=FQA.Node;
					if (GetFullQualifiedAddress (search+1,&FQA,NULL)==SUCCESS)
					{
						storage->Origin.Zone=FQA.Zone;
						storage->Origin.Net=FQA.Net;
						storage->Origin.Node=FQA.Node;
					}
				}
			}
		}
		if (strstr (first+1,"FLAGS")==first+1)
		{
			first++;
			GetAndStripToken (first,Parsing); // remove 'flags'
			while (first[0])
			{
				GetAndStripToken (first,Parsing);
				switch (GetTokenType (Parsing))
				{
					case TT_PVT:
						storage->attrib.bits.Private=1;
						break;
					case TT_HLD:
						storage->attrib.bits.Hold=1;
						break;
					case TT_CRA:
						storage->attrib.bits.Crash=1;
						break;
					case TT_KS:
						storage->attrib.bits.KillSent=1;
						break;
					case TT_SNT:
						storage->attrib.bits.Sent=1;
						break;
					case TT_RCV:
						storage->attrib.bits.Recd=1;
						break;
					case TT_AS:
						storage->attrib2.bits.ArchiveSent=1;
						break;
					case TT_DIR:
						storage->attrib2.bits.Direct=1;
						break;
					case TT_ZON:
						storage->attrib2.bits.Zonegate=1;
						break;
					case TT_HUB:
						storage->attrib2.bits.Hub=1;
						break;
					case TT_FIL:
						storage->attrib.bits.FileAttached=1;
						break;
					case TT_FRQ:
						storage->attrib.bits.FileRequest=1;
						break;
					case TT_IMM:
						storage->attrib2.bits.Immediate=1;
						break;
					case TT_XMA:
						storage->attrib2.bits.XMA=1;
						break;
					case TT_KFS:
						storage->attrib2.bits.KillFileSent=1;
						break;
					case TT_TFS:
						storage->attrib2.bits.Truncate=1;
						break;
					case TT_LOK:
						storage->attrib2.bits.Lock=1;
						break;
					case TT_RRQ:
						storage->attrib.bits.ReturnReceiptRequest=1;
						break;
					case TT_CFM:
						storage->attrib.bits.IsReturnReceipt=1;
						break;
					case TT_HIR:
						storage->attrib2.bits.HiRes=1;
						break;
					case TT_COV:
						storage->attrib2.bits.CoverLetter=1;
						break;
					case TT_SIG:
						storage->attrib2.bits.Signature=1;
						break;
					case TT_LET:
						storage->attrib2.bits.LetterHead=1;
						break;
					case TT_FAX:
						storage->attrib2.bits.Fax=1;
						break;
					case TT_FPU:
						storage->attrib2.bits.ForcePickup=1;
						break;
				}
			}
		}
	}
}

// Transform the (useless) standard .MSG format in something we can
// handle easily.
int GetVisibleInfo (char *path,S_Visu *storage,C_StringList *SL_Via, C_StringList *SL_Path)
{
        DATETIME dtnow;
        S_MSG header;
	struct S_FQAddress FQA;
	C_FileRead FHandler;
	void *buffer;
	char newpath[26];
	char Parsing[21];
	int count;
	storage->ProcessedHere=0;
	storage->MSGIDZone=0;

	// Get as much info as possible from the header
	if (FHandler.OpenFile (path)!=SUCCESS)
		return ENH_OPENFAIL;
        storage->MessageSize=FHandler.FileSize()-sizeof (S_MSG);
	buffer=malloc (BUFFER_SIZE); // Buffer to get lines
	if (buffer==NULL)
	{
		FHandler.CloseFile ();
		return NOMEMORY;
	}
	// Read header
        if (FHandler.ReadBytes (&header,sizeof (S_MSG))<sizeof (S_MSG))
	{
		FHandler.CloseFile ();
		free (buffer);
		return ENH_SHORTFILE;
	}
// Empty zones, point & domain
	AddressHandler.GetMain (&FQA);
	storage->Origin.Zone=storage->Destination.Zone=0;
	storage->Origin.Point=storage->Destination.Point=0;
	storage->Origin.Domain[0]=storage->Destination.Domain[0]=0;
	storage->DetectedLoop=0;
	// Empty encryption
	storage->Encrypted=0;
	// Names and subject
	strcpy (storage->FromUserName,header.FromUserName);
	strcpy (storage->ToUserName,header.ToUserName);
	strcpy (storage->Subject,header.Subject);
        // Original date & time string
	strcpy (storage->OrigDateTime,header.DateTime);
	// Attempt to parse the time and date
	storage->DateParsed=0;
	strcpy (Parsing,header.DateTime);
	if (isdigit (Parsing[0]) && isdigit (Parsing[1]) && Parsing[2]==' ' &&
		isalpha (Parsing[3]) && isalpha (Parsing[4]) && isalpha(Parsing[5]) &&
		Parsing[6]==' ' && isdigit(Parsing[7]) && isdigit(Parsing[8]) &&
		Parsing[9]==' ' && Parsing[10]==' ' && isdigit(Parsing[11]) &&
		isdigit(Parsing[12]) && Parsing[13]==':' && isdigit(Parsing[14]) &&
		isdigit(Parsing[15]) && Parsing[16]==':' && isdigit(Parsing[17]) &&
		isdigit(Parsing[18]))
	{
		// If here it has the expected format - separate tokens
		Parsing[2]=Parsing[6]=Parsing[9]=Parsing[13]=Parsing[16]=0;
		storage->date.day=atoi (Parsing);
		storage->date.month=0;
		for (count=0;count<12;count++)
		{
			if (cistrcmp (Parsing+3,Months[count])==0)
			{
				storage->date.month=count+1;
				break;
			}
		}
		if (storage->date.month!=0) // If we don't have a month, quit parsing
		{
			storage->date.year=1900 + atoi (Parsing+7);
			storage->time.hour=atoi (Parsing+11);
			storage->time.minute=atoi (Parsing+14);
			storage->time.second=atoi (Parsing+17);
			storage->DateParsed=1;

                        /* sliding window adaption of year number */

                        DosGetDateTime(&dtnow);
                        while (storage->date.year + 50 <= dtnow.year)
                                storage->date.year += 100;
                        while (storage->date.year - 50 > dtnow.year)
                                storage->date.year -= 100;
		}
	}
	storage->attrib.number=header.Attribute.number;
	storage->attrib2.number=0;
        storage->attrib2.bits.Direct = storage->attrib.bits.Unused;
	storage->Origin.Net=header.OrigNet;
	storage->Origin.Node=header.OrigNode;
	storage->Destination.Net=header.DestNet;
	storage->Destination.Node=header.DestNode;
	// We still need the origin and destination zones & points
	// they are somewhere in the message text...
	while (FHandler.ReadUpTo (buffer,"\r\n")!=EBH_EOF)
	{
		ProcessLine ((char *) buffer,storage,SL_Via, SL_Path);
	}
	if (storage->Origin.Zone==0)
		storage->Origin.Zone=storage->MSGIDZone;
	if (storage->Destination.Zone==0)
		storage->Destination.Zone=storage->MSGIDZone;
	AddressHandler.GetMain (&FQA);
	if (storage->Origin.Zone==0)
		storage->Origin.Zone=FQA.Zone;
	if (storage->Destination.Zone==0)
		storage->Destination.Zone=FQA.Zone;
	memcpy (&storage->TransitAddr,&storage->Destination,sizeof (struct S_FQAddress));
	// Handle hostgated messages
	if (storage->attrib2.bits.Hub==1)
	{
		if (AddressHandler.OurNet (storage->Destination.Zone,storage->Destination.Net)==EAD_FOREIGN)
		{
				storage->TransitAddr.Node=0;
				storage->TransitAddr.Point=0;
		}
	}
	// Handle zonegated messages
	if (storage->attrib2.bits.Zonegate==1)
	{
		if (AddressHandler.OurZone (storage->Destination.Zone)==EAD_FOREIGN)
		{ // To a foreign zone...
			// Example: From 2:* to 1:* becomes 2:2/1.0.
			storage->TransitAddr.Net=storage->Destination.Zone;
			storage->TransitAddr.Zone=storage->Destination.Zone;
			storage->TransitAddr.Node=storage->Origin.Zone;
			storage->TransitAddr.Point=0;
		}
	}
	sprintf (newpath,"%u:%u/%u.%u",storage->Origin.Zone,
		storage->Origin.Net,storage->Origin.Node,storage->Origin.Point);
	if (SL_Path!=NULL)
	{
		if (SL_Path->GetStringCount ()==0)
			SL_Path->AddString (newpath);
		else
			if (strcmp (SL_Path->GetString (0),newpath))
				SL_Path->InsertString (newpath,0);
	}
	free (buffer);
	FHandler.CloseFile ();
	return SUCCESS;
}

int ShowNet (char *path)
{
        S_MSG *header;
        S_Visu extra;
	FILE *fnet;
	void *buffer;
	char *point,*first;
	int count=0;
	fnet=fopen (path,"rb");
	if (fnet==NULL)
		return ENH_OPENFAIL;
	buffer=malloc (2048);
	if (buffer==NULL)
	{
		fclose (fnet);
		return NOMEMORY;
	}
	fread (buffer,2048,1,fnet);
	((char *)buffer)[2047]=0;
	header=(S_MSG *) buffer;
	printf ("            From: %s\n",header->FromUserName);
	printf ("              To: %s\n",header->ToUserName);
	printf ("         Subject: %s\n",header->Subject);
	printf ("        DateTime: %s\n",header->DateTime);
	printf ("      Times read: %u\n",header->TimesRead);
	printf ("Destination node: %u\n",header->DestNode);
	printf ("     Origin node: %u\n",header->OrigNode);
	printf ("            Cost: %u\n",header->Cost);
	printf ("      Origin net: %u\n",header->OrigNet);
	printf (" Destination net: %u\n",header->DestNet);
	printf ("        Reply-to: %u\n",header->ReplyTo);
	printf ("      Attributes: %u\n",header->Attribute.number);
	printf ("       NextReply: %u\n",header->NextReply);
	printf ("    Rest of text: \n");
	first=(char *) (&header->NextReply)+sizeof (header->NextReply);
	while ((point=strchr (first,'\r'))!=NULL && count<8)
	{
		if (first+75<point)
			point=first+75;
		*point=0;
		printf ("%s\r\n",first);
		first=point+1;
		count++;
	}
	free (buffer);
	fclose (fnet);
	getch ();
	GetVisibleInfo (path,&extra,NULL,NULL);
        //clrscr ();
	printf ("Information as returned by GetVisibleInfo ().\n");
	printf ("    Sender: %s (%u:%u/%u.%u)\n",extra.FromUserName,
	extra.Origin.Zone,extra.Origin.Net,extra.Origin.Node,extra.Origin.Point);
	printf ("  Receiver: %s (%u:%u/%u.%u)\n",extra.ToUserName,
	extra.Destination.Zone,extra.Destination.Net,extra.Destination.Node,extra.Destination.Point);
	printf ("   Subject: %s\n",extra.Subject);
	printf ("Original date & time: %s  ",extra.OrigDateTime);
	if (extra.DateParsed)
	{
		printf ("Parsed: %02u-%02u-%04u  %02u:%02u:%02u",
		extra.date.day,extra.date.month,extra.date.year,
		extra.time.hour,extra.time.minute,extra.time.second);
	}
	printf ("\nAttributes: ");
	printf ("%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
		extra.attrib.bits.Private?"Pvt":"   ",
		extra.attrib.bits.Crash?"Cra":"   ",
		extra.attrib.bits.Recd?"Rev":"   ",
		extra.attrib.bits.Sent?"Snt":"   ",
		extra.attrib.bits.FileAttached?"Att":"   ",
		extra.attrib.bits.InTransit?"Trs":"   ",
		extra.attrib.bits.Orphan?"Orp":"   ",
		extra.attrib.bits.KillSent?"K/S":"   ",
		extra.attrib.bits.Local?"Loc":"   ",
		extra.attrib.bits.Hold?"Hld":"   ",
		extra.attrib.bits.Unused?"Dir":"   ",
		extra.attrib.bits.FileRequest?"Frq":"   ",
		extra.attrib.bits.ReturnReceiptRequest?"RRQ":"   ",
		extra.attrib.bits.IsReturnReceipt?"ReR":"   ",
		extra.attrib.bits.AuditRequest?"Aud":"   ",
		extra.attrib.bits.FileUpdateReq?"UpR":"   ");
	getch ();
	return (SUCCESS);
}

// DoQQQPack() determines whether a mail should be packed into the
// Binkley outbound or into a .QQQ files. A message only qualifies for
// being packed into a QQQ file if:
// - FASTECHOPACK has been specified in the cfroute cfg
// - FECFONIG     has been specified in the cfroute cfg
// - the mail is not crash, direct, immediate or hold, or has a file
//   attached
// - the destination system is listed in fastecho.cfg
//   (which can be seen from the fact if PasswordHandler knows this
//   system or not).

int DoQQQPack(S_FQAddress via, int AttribPack)
{
        if ((FastechoPack == 2)  ||    /* Pack anything as QQQ */
            (FastechoPack == 1  &&     /* Pack mail to listed links only */
             PasswordHandler.GetPassword(via, (char *)NULL) == SUCCESS &&
                                       /* destination system is a link */
             AttribPack == TT_NORMAL))
                                       /* no "priority" flag is set */
                return 1;
        return 0;
}

void FindUniqueQQQ (char *storage)
{
	char rname[9],temp[80];
	int c,r;
	FILE *o;
	for (;;) /* Keep trying until we succeed */
	{
		for (c=0;c<8;c++)
		{
			r=rand()%16;
			if (r>=10)
				rname[c]=r+'A'-10;
			else
				rname[c]=r+'0';
		}
		rname[8]=0;
		sprintf (storage,"%s"DIRSEPS"%s.",QQQOutboundDirectory,rname);
		sprintf (temp,"%sQQQ",storage);
                adaptcase(temp);
		o=fopen (temp,"rb");
		if (o!=NULL)
			fclose (o);
		else /* Failed to open it - finally... */
			return;
	}
}

int FindPKTPath (S_FQAddress destination,char *storage)
{
	S_FQAddress work;
	int UseDefaultOutbound;
	*storage=0;
	char Domain[256],temp[256],*lookup;
	int count;
	if (AddressHandler.GetMain (&work)!=SUCCESS)
		return (EAD_NOMAIN);
	// If there isn't domain information, we use zones ONLY
	if (work.Domain[0]==0 || destination.Domain[0]==0)
		if (destination.Zone==work.Zone)
			UseDefaultOutbound=1;
		else
			UseDefaultOutbound=0;
	else
		// Use specified domain, if there is one
		if (cistrcmp (work.Domain,destination.Domain)==0 &&
			destination.Zone==work.Zone)
			UseDefaultOutbound=1;
		else
			UseDefaultOutbound=0;
	if (UseDefaultOutbound)
		sprintf (temp,"%s"DIRSEPS,OutboundDirectory);
	else
	{
		if (destination.Domain[0]!=0)
			strcpy (Domain,destination.Domain);
		else
		{
			Domain[0]=0;
			for (count=1;count<=AddressHandler.GetAKACount();count++)
			{
				AddressHandler.GetAKA (&work,count);
				if (destination.Zone==work.Zone)
				{
					strcpy (Domain,work.Domain);
					break;
				}
			}
		}
		// If no domain, simply add the zone number to the outbound
		if (Domain[0]==0 || NoDomainDir)
			sprintf (temp,"%s.%03X"DIRSEPS,OutboundDirectory,destination.Zone);
		else
		{
			lookup=strchr (Domain,'.');
			if (lookup!=NULL)
				*lookup=0;
			strcpy (temp,OutboundDirectory);
			lookup=temp+strlen (temp)-1;
			while (*lookup!=DIRSEPC)
				lookup--;
			sprintf (lookup+1,
                                 "%s.%03X"DIRSEPS,Domain,destination.Zone);
		}
	}
	// OK, here we have the path - add the node
	sprintf (storage,"%s%04X%04X.",temp,destination.Net,destination.Node);
	// is it a point?
	if (destination.Point!=0)
	{
		sprintf (temp,"PNT"DIRSEPS"%08X.",destination.Point);
		strcat (storage,temp);
	}
	return SUCCESS;
}

void BufferHeader (S_Visu *header, C_StringList *SL_Header)
{
	char buffer[256];
	sprintf (buffer,"From: %-40s (%u:%u/%u.%u)\n",header->FromUserName,
		header->Origin.Zone,header->Origin.Net,header->Origin.Node,
		header->Origin.Point);
	SL_Header->AddString (buffer);
	sprintf (buffer,"  To: %-40s (%u:%u/%u.%u)\n",header->ToUserName,
		header->Destination.Zone,header->Destination.Net,
		header->Destination.Node,header->Destination.Point);
	SL_Header->AddString (buffer);
	sprintf (buffer,"Subj: %s\n",header->Subject);
	SL_Header->AddString (buffer);
	if (header->DateParsed)
	{
		sprintf (buffer,"Date: %02hu-%02hu-%04hu  %02hu:%02hu:%02hu - %ld days ago",
			header->date.day,header->date.month,header->date.year,
			header->time.hour,header->time.minute,header->time.second,
			(long) GetToday()-GetJulianDate (header->date.day,header->date.month,header->date.year));
	}
	else
		sprintf (buffer,"Date: %s (non-standard)",header->OrigDateTime);
	sprintf (buffer+strlen (buffer)," - Body size: %lu",header->MessageSize);
	if (header->Encrypted)
		strcat (buffer," - Encrypted");
	strcat (buffer,"\n");
	SL_Header->AddString (buffer);
	sprintf (buffer,"Attr: %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		header->attrib.bits.Private?"Pvt ":"", // Doesn't mind for routing
		header->attrib.bits.Crash?"Cra ":"", // Direct
		header->attrib.bits.Recd?"Rev ":"", // D.M.F.R.
		header->attrib.bits.Sent?"Snt ":"", // Sent - do not touch
		header->attrib.bits.FileAttached?"Att ":"",//D.M.F.R.
		header->attrib.bits.InTransit?"Trs ":"", //D.M.F.R.
		header->attrib.bits.Orphan?"Orp ":"", //D.M.F.R.
		header->attrib.bits.KillSent?"K/S ":"",//D.M.F.R.
		header->attrib.bits.Local?"Loc ":"", //D.M.F.R.
		header->attrib.bits.Hold?"Hld ":"", // Direct
		header->attrib.bits.Unused?"Dir ":"", // D.M.F.R.
		header->attrib.bits.FileRequest?"Frq ":"", // Direct
		header->attrib.bits.ReturnReceiptRequest?"RRQ ":"", // D.M.F.R.
		header->attrib.bits.IsReturnReceipt?"ReR ":"", // D.M.F.R.
		header->attrib.bits.AuditRequest?"Aud ":"", // D.M.F.R.
		header->attrib.bits.FileUpdateReq?"UpR ":"", // Direct
		header->attrib2.bits.ArchiveSent?"A/S ":"",
		header->attrib2.bits.KillFileSent?"KFS ":"",
		(header->attrib2.bits.Direct
                 && (!(header->attrib.bits.Unused))) ? "Dir ":"",
		header->attrib2.bits.Zonegate?"Zon ":"",
		header->attrib2.bits.Hub?"Hub ":"",
		header->attrib2.bits.Immediate?"Imm ":"",
		header->attrib2.bits.XMA?"XMA ":"",
		header->attrib2.bits.Lock?"Lok ":"",
		header->attrib2.bits.Truncate?"Tru ":"",
		header->attrib2.bits.HiRes?"HiR ":"",
		header->attrib2.bits.CoverLetter?"CvL ":"",
		header->attrib2.bits.Signature?"Sig ":"",
		header->attrib2.bits.LetterHead?"LeH ":"",
		header->attrib2.bits.Fax?"Fax ":"",
		header->attrib2.bits.ForcePickup?"FPU ":"");
	SL_Header->AddString (buffer);
}

int GetSendType (S_Visu *header,C_StringList *SL_Routeto)
{
	int SendType;
	char buffer[256];
	SendType=ST_ROUTE;
	if (AddressHandler.IsLocalAKA (header->Destination))
	{
		SL_Routeto->AddString (" Via: None (addressed to this system)\n");
		SendType=ST_NOTOUCH;
	}
	else
		// Determine the kind of routing
		// Conditions that will force the message NOT to be processed
	{
		if (header->attrib.bits.Crash==1 || header->attrib.bits.Hold==1 ||
			header->attrib.bits.FileRequest==1 || header->attrib.bits.FileUpdateReq==1 ||
			header->attrib2.bits.Direct==1 || header->attrib2.bits.Immediate==1 ||
			header->attrib2.bits.ForcePickup==1)
		{
			SendType=ST_DIRECT;
			if (header->Destination.Point!=0 && (RouteBossDirect==1 && (header->attrib.bits.Crash==1 ||
			header->attrib2.bits.Immediate==1 || header->attrib2.bits.Direct==1 &&
			(header->attrib.bits.FileRequest==0 && header->attrib.bits.FileUpdateReq==0))))
				SendType=ST_BOSS;
			if (header->Destination.Point!=0 && (RouteBossHold==1 && (header->attrib.bits.Hold==1 ||
			header->attrib2.bits.ForcePickup==1)))
				SendType=ST_BOSS;
                        if (SendType == ST_BOSS)
                        {
                                S_FQAddress ShouldGo;
                                memcpy (&ShouldGo,&header->Destination,
                                        sizeof (S_FQAddress));
          			ShouldGo.Point=0;
                                if (AddressHandler.IsLocalAKA(ShouldGo))
                                {
                                        SendType = ST_DIRECT;
                                        // Mail that is destined to a
                                        // point of our own is always
                                        // sent direct
                                }
                        }

		}
		// Handle hostgated messages
		if (header->attrib2.bits.Hub==1)
		{
			if (AddressHandler.OurNet (header->Destination.Zone,header->Destination.Net)==EAD_FOREIGN)
			{
				sprintf (buffer,"(Hostgated to %u:%u/%u.%u)\n",
					header->TransitAddr.Zone,header->TransitAddr.Net,
					header->TransitAddr.Node,header->TransitAddr.Point);
					SL_Routeto->AddString (buffer);
			}
		}
		// Handle zonegated messages
		if (header->attrib2.bits.Zonegate==1)
		{
			if (AddressHandler.OurZone (header->Destination.Zone)==EAD_FOREIGN)
			{ // To a foreign zone...
				// Example: From 2:* to 1:* becomes 2:2/1.0.
				sprintf (buffer,"(Zonegated to %u:%u/%u.%u)\n",
					header->TransitAddr.Zone,header->TransitAddr.Net,
					header->TransitAddr.Node,header->TransitAddr.Point);
				SL_Routeto->AddString (buffer);
			}
		}
	}
	return (SendType);
}

int DetermineRouteToSystem (S_Visu *header,int SendType,
	C_StringList *SL_Routeto,S_FQAddress *ShouldGo,int *AttribPack,
	char *ext,char *extattach)
{
	int GotSystem=0;
	char buffer[256];
	switch (SendType)
	{
		case ST_NOTOUCH:
			SL_Routeto->AddString ("Attributes caused message not to be processed.\n");
			break;
		case ST_ROUTE:
			if (RouteHandler.FindPackSystem (header->TransitAddr,
				header->Origin,header->attrib.bits.FileAttached,
				header->Encrypted,ShouldGo,AttribPack))
			{
				sprintf (buffer," Via: %u:%u/%u.%u as ",
				ShouldGo->Zone,ShouldGo->Net,ShouldGo->Node,ShouldGo->Point);
				GotSystem=1;
			}
			else
			{
				SL_Routeto->AddString (" Via: None (no route defined for this message).\n");
				SendType=ST_NOTOUCH;
				GotSystem=0;
			}
			break;
		case ST_DIRECT:
			memcpy (ShouldGo,&header->Destination,sizeof (S_FQAddress));
			*AttribPack=TT_NORMAL;
			sprintf (buffer," Via: Direct to %u:%u/%u.%u as ",
				ShouldGo->Zone,ShouldGo->Net,ShouldGo->Node,ShouldGo->Point);
				GotSystem=1;
			break;
		case ST_BOSS:
			memcpy (ShouldGo,&header->Destination,sizeof (S_FQAddress));
			ShouldGo->Point=0;
			*AttribPack=TT_NORMAL;
			sprintf (buffer," Via: Boss %u:%u/%u.%u as ",
				ShouldGo->Zone,ShouldGo->Net,ShouldGo->Node,ShouldGo->Point);
				GotSystem=1;
			break;
	}
	// The final attribute may be different that the one
	// on the route-to sentences
	if (GotSystem==1)
	{
		if (header->attrib.bits.Hold==1 && header->attrib.bits.Crash==1)
			*AttribPack=TT_DIR;
		else
		if (header->attrib.bits.Hold==1)
			*AttribPack=TT_HOLD;
		else
		if (header->attrib.bits.Crash==1)
			*AttribPack=TT_CRASH;
		else
		if (header->attrib2.bits.Direct==1)
			*AttribPack=TT_DIR;
		switch (*AttribPack)
		{
			case TT_NORMAL:
				strcat (buffer,"NORMAL (OUT)");
				strcpy (ext,"OUT");
				strcpy (extattach,"FLO");
				break;
			case TT_HOLD:
				strcat (buffer,"HOLD (HUT)");
				strcpy (ext,"HUT");
				strcpy (extattach,"HLO");
				break;
			case TT_CRASH:
				strcat (buffer,"CRASH (CUT)");
				strcpy (ext,"CUT");
				strcpy (extattach,"CLO");
				break;
			case TT_DIR:
				strcat (buffer,"DIRECT (DUT)");
				strcpy (ext,"DUT");
				strcpy (extattach,"DLO");
				break;
			default:
				strcat (buffer,"*** corrupted attribute ***");
				break;
		} // switch (Attribpack);
		strcat (buffer,".\n");
		SL_Routeto->AddString (buffer);
	} // if (GotSystem);
	return (GotSystem);
}

int CheckAndCreateSem (char *path)
{
	char *sav;
        FILE *f;
#ifdef OS_2
	FILEFINDBUF3 ffblk;
	ULONG ulFileCount=1;
	HDIR hdir=HDIR_SYSTEM;
#else
	struct ffblk ffblk;
#endif
	int result;
	sav=(char *) malloc (strlen (path)+5);
	strcpy (sav,path);
	strcat (sav,"BSY");
        result = adaptcase(sav);
        if (!result) /* BSY file does not exist - create it! */
        {
                f = fopen(sav, "wt");
                if (f) fclose(f);
        }
	free (sav);
	return (result);
}

void ReleaseSem (char *path)
{
	char *sav;
        FILE *f;

	int result;
	sav=(char *) malloc (strlen (path)+5);
	strcpy (sav,path);
	strcat (sav,"BSY");
        result = adaptcase(sav);
        if (result) /* BSY file does  exist - delete it! */
                remove(sav);
	free (sav);
}


void CreatePathLines (C_StringList *source,C_StringList *target)
{
	char work[80];
	int c;
	if (PathLogStyle==0)
		return;
        if (PathLogStyle==1)
	{
		strcpy (work,"Path: ");
		for (c=0;c<source->GetStringCount();c++)
		{
			strcat (work,source->GetString (c));
			if (c<source->GetStringCount()-1)
				strcat (work," -> ");
			if (strlen (work)>60)
			{
				strcat (work,"\n");
				target->AddString (work);
				strcpy (work,"Path: ");
			}
		}
		strcat (work,"\n");
		if (strlen (work)>7) /* 6=strlen ("Path: \n") */
			target->AddString (work);
		return;
	}
}

int PostAnalysis (S_Visu *extra,struct S_Control *x)
{
	int count;
	if (RouteHandler.NoPack (extra->Destination,
                                 extra->Origin,extra->attrib.bits.FileAttached,
                                 extra->Encrypted))
		return RET_SUCCESS;
	// If the message has any of the following flags, it is skipped
	// without further check.
        if (extra->attrib.bits.Sent==1 ||
            (
                    (extra->attrib2.bits.XMA==1 ||
                     extra->attrib2.bits.Lock==1 ||
                     extra->attrib2.bits.HiRes==1 ||
                     extra->attrib2.bits.HiRes==1 ||
                     extra->attrib2.bits.CoverLetter==1 ||
                     extra->attrib2.bits.CoverLetter==1 ||
                     extra->attrib2.bits.Signature==1 ||
                     extra->attrib2.bits.LetterHead==1 ||
                     extra->attrib2.bits.Fax==1)
                    &&!IgnoreUnknownAttribs)
                )
	{
#ifdef DEBUG
		printf ("Skipped because of attributes.\n");
#endif
		return RET_SUCCESS;
	}
	BufferHeader (extra,&x->SL_Header);
	x->SendType=GetSendType (extra,&x->SL_Routeto);
	if (x->SendType==ST_NOTOUCH)
		return RET_SUCCESS;
	x->GotSystem=DetermineRouteToSystem (extra,x->SendType,&x->SL_Routeto,
                                             &x->ShouldGo,&x->AttribPack,
                                             x->ext,x->extattach);
	// At this point: The message has to be processed, and
	// we have the header and route-to information. We log
	// them.
	for (count=0;count<x->SL_Header.GetStringCount();count++)
		Log.WriteOnLog ("%s",x->SL_Header.GetString (count));
	CreatePathLines (&x->SL_Path,&x->SL_ToWrite);
	for (count=0;count<x->SL_ToWrite.GetStringCount();count++)
		Log.WriteOnLog ("%s",x->SL_ToWrite.GetString (count));
	for (count=0;count<x->SL_Routeto.GetStringCount();count++)
		Log.WriteOnLog ("%s",x->SL_Routeto.GetString (count));
	if (extra->DetectedLoop==1)
		Log.WriteOnLog ("Warning: %s already processed here - possible loop.\n",x->define);
	if (extra->DetectedLoop==1 || RecordViaLines)
	{
		for (count=0;count<x->SL_Via.GetStringCount();count++)
			Log.WriteOnLog ("%s\n",x->SL_Via.GetString (count));
	}
	if (!x->GotSystem)
	{
#ifdef DEBUG
		printf ("No path.\n");
#endif
		Log.WriteOnLog ("\n");
		return (RET_SUCCESS);
	}
#ifdef DEBUG
	printf ("Found a path.\n");
#endif
        if (DoQQQPack(x->ShouldGo, x->AttribPack))
        {
                x->is_qqq = 1;
                strcpy(x->ext, "QQQ");
		FindUniqueQQQ (x->savepath);
        }
        else
                FindPKTPath (x->ShouldGo,x->savepath);

        FindPKTPath (x->ShouldGo, x->savepathattach);

	Log.WriteOnLog ("File: %s%s\n",x->savepath,x->ext);
	AddressHandler.AKAMatch (x->ShouldGo,&x->OurAKA);
	printf ("%lu bytes from %hu:%hu/%hu.%hu to %hu:%hu/%hu.%hu "
                "via %hu:%hu/%hu.%hu%s.\n", extra->MessageSize,
		extra->Origin.Zone,extra->Origin.Net,
                extra->Origin.Node,extra->Origin.Point,
		extra->Destination.Zone,extra->Destination.Net,
		extra->Destination.Node,extra->Destination.Point,
		x->ShouldGo.Zone,x->ShouldGo.Net,
                x->ShouldGo.Node,x->ShouldGo.Point,
		Packet2Handler.DoesMatch (x->ShouldGo,
                                          x->ShouldGo,0,0)?" (Packet 2)":"");
	return SUCCESS;
}

int AnalyzeNet (char *path)
{
        S_Visu extra;
	struct S_Control x;
	char buffer[256];
	FILE *touch;
	x.GotSystem=0; x.is_qqq = 0;
        adaptcase(path);
	strcpy (x.define,path);
	if (GetVisibleInfo (path,&extra,&x.SL_Via,&x.SL_Path)!=SUCCESS)
		return FATAL;
	if (PostAnalysis (&extra,&x)==RET_SUCCESS)
		return SUCCESS;
	if (IgnoreBSY==0 && CheckAndCreateSem (x.savepath))
	{
		printf ("(System is now busy - message not exported)\n");
		Log.WriteOnLog ("This message was not exported because"
                                " the waypoint was busy.\n\n");
		// Touch file so it is not skipped in the next run
		// because of lastrun.cfr
#ifndef UNIX
		touch=fopen (path,"r+b");
		if (touch)
		{
			fseek (touch,0,SEEK_SET);
			if (fread (buffer,1,16,touch)==16)
			{
				fseek (touch,0,SEEK_SET);
				fwrite (buffer,1,16,touch);
			}
			fclose (touch);
		}
#else
#ifdef __FreeBSD__
                utimes(path, NULL);
#else
                utime(path, NULL);
#endif
#endif
		return (ENH_DELAYED);
	}

	if (MSGToPKT (path,x.savepath,x.ext,x.ShouldGo,x.OurAKA,
                      extra.Destination,
                      Packet2Handler.DoesMatch(x.ShouldGo,x.ShouldGo,0,0), x.is_qqq)
            ==ECONV_OPENFAILOUT)
	{
		Log.WriteOnLog ("Warning: Failed to open/create PKT "
                                "(%s).\n\n",x.savepath);
                if (IgnoreBSY == 0)
                        ReleaseSem (x.savepath);
		return (SUCCESS);
        }

	// Process file-attaches and file-requests
	if (extra.attrib.bits.FileAttached)
	{
		if (SubjectToFile(extra.Subject,x.savepathattach,x.extattach,
                                  extra.attrib2.bits.Truncate,
                                  extra.attrib2.bits.KillFileSent)!=SUCCESS)
			Log.WriteOnLog ("Warning: Failed to update "
                                        "fileattach queue.\n");
	}
        if (extra.attrib.bits.FileRequest)
	{
		strcpy (x.extattach,"REQ");
		if (SubjectToFile (extra.Subject,x.savepathattach,
                                   x.extattach,0,0)!=SUCCESS)
			Log.WriteOnLog ("Warning: Failed to update"
                                        " filerequest queue.\n");
	}
	if (extra.attrib.bits.KillSent ||
            (extra.attrib.bits.InTransit && KillInTransit))
	{
		if (remove (path))
			Log.WriteOnLog ("Warning: Unable to delete %s.\n",
                                        x.define);
	}
	else
	{
		if (FlagAsSent (path)!=SUCCESS)
			Log.WriteOnLog ("Warning: Can't flag %s "
                                        "as sent.\n",x.define);
	}
	Log.WriteOnLog ("\n");
	if (IgnoreBSY==0)
                ReleaseSem (x.savepath);
	return (ENH_PROCESSED);
}



