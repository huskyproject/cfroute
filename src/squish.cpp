#ifdef SQUISHCFS
// Transform the (not so useless) standard Squish format in something we can
// handle easily.
int GetVisibleInfoSquish (HAREA BaseH,unsigned long MsgNumber,S_Visu *storage,C_StringList *SL_Via, C_StringList *SL_Path)
{
	HMSG MsgH;
	XMSG SqHeader;
        S_FQAddress FQA;
	unsigned long TotalToRead,ReadThisPass,ReadOffset;
	char newpath[26],*linecopy=NULL;
	unsigned char *p;
	char *p2;
	size_t length;
	MsgH=MsgOpenMsg (BaseH,MOPEN_READ,MsgNumber);
	storage->MsgNumber=MsgNumber;
	storage->formattype=formatSQUISH;
	storage->MSGIDZone=0;
	storage->ProcessedHere=0;
	if (MsgH==NULL)
	{
		printf ("Warning: Failed to open message %lu.\n",MsgNumber);
		return ENH_OPENFAIL;
	}
	MsgReadMsg (MsgH,&SqHeader,0L,0L,NULL,CONTROLBYTES,ControlText);
	TotalToRead=MsgGetTextLen (MsgH);
	if (MsgGetCtrlLen (MsgH)>CONTROLBYTES)
	{
		ControlText=(unsigned char *)realloc (ControlText,(size_t) MsgGetCtrlLen (MsgH));
		if (ControlText==NULL)
		{
			printf ("Not enough memory to read control lines from message %lu.\n",MsgNumber);
			return NOMEMORY;
		}
		CONTROLBYTES=MsgGetCtrlLen (MsgH);
	}
	// Empty zones, point & domain
	AddressHandler.GetMain (&FQA);
	storage->Origin.Domain[0]=storage->Destination.Domain[0]=0;
	storage->DetectedLoop=0;
	// Empty encryption
	storage->Encrypted=0;
	// Names and subject
	strcpy (storage->FromUserName,(char *) SqHeader.from);
	strcpy (storage->ToUserName,(char *) SqHeader.to);
	strcpy (storage->Subject,(char *) SqHeader.subj);

	// Original date & time string
	strcpy (storage->OrigDateTime,(char *) SqHeader.__ftsc_date);
	storage->DateParsed=1;
	storage->date.day=SqHeader.date_written.date.da;
	storage->date.month=SqHeader.date_written.date.mo;
	storage->date.year=SqHeader.date_written.date.yr+1980;
	storage->time.hour=SqHeader.date_written.time.hh;
	storage->time.minute=SqHeader.date_written.time.mm;
	storage->time.second=SqHeader.date_written.time.ss;

	storage->attrib.number=SqHeader.attr;
	storage->attrib2.number=0;
        storage->attrib2.bits.Direct = storage->attrib.bits.Unused;
	storage->Origin.Zone=SqHeader.orig.zone;
	storage->Origin.Net=SqHeader.orig.net;
	storage->Origin.Node=SqHeader.orig.node;
	storage->Origin.Point=SqHeader.orig.point;
	storage->Destination.Zone=SqHeader.dest.zone;
	storage->Destination.Net=SqHeader.dest.net;
	storage->Destination.Node=SqHeader.dest.node;
	storage->Destination.Point=SqHeader.dest.point;
	// We still need the origin and destination zones & points
	// they are somewhere in the message text...
	ReadOffset=0;
	while (TotalToRead>0)
	{
		ReadThisPass=(TotalToRead>=((unsigned long)(CONTROLBYTES-1)))?
                                ((unsigned long)(CONTROLBYTES-1)):TotalToRead;
		memset (ControlText,0,CONTROLBYTES);
		MsgReadMsg (MsgH,&SqHeader,ReadOffset,ReadThisPass,ControlText,0L,NULL);
		p=ControlText;
		do
		{
			if (strchr ((char *)p,'\n'))
			{
				ReadThisPass=strchr ((char *)p,'\n')-(char *)ControlText;
				ControlText[(int) ReadThisPass]=0;
				ReadThisPass++;
			}
			if (strchr ((char *)p,'\r'))
			{
				ReadThisPass=strchr ((char *)p,'\r')-(char *)ControlText;
				ControlText[(int) ReadThisPass]=0;
				ReadThisPass++;
			}
			ProcessLine ((char *)p,storage,SL_Via, SL_Path);
			p+=strlen ((char *)p)+1;
		}
		while (strchr ((char *) p,'\n') || strchr ((char *) p,'\r'));
		TotalToRead-=ReadThisPass;
		ReadOffset+=ReadThisPass;
	}
	/* Process Control text */
	memset (ControlText,0,CONTROLBYTES);
	MsgReadMsg (MsgH,&SqHeader,0L,0L,NULL,MsgGetCtrlLen (MsgH),ControlText);
	p=ControlText;
	do
	{
		if
		((p2=strchr ((char *) p+1,0x1))!=NULL)
		{
			linecopy=(char *) realloc (linecopy,1+(size_t) (p2-(char *) p));
			if (linecopy==NULL)
			{
				MsgCloseMsg (MsgH);
				return NOMEMORY;
			}
			memcpy (linecopy,p,(size_t) (p2-(char *) p));
			linecopy[(size_t) (p2-(char *) p)]=0;
		}
		else
		{
			linecopy=(char *) realloc (linecopy,1+strlen ((char *) p));
			if (linecopy==NULL)
			{
				MsgCloseMsg (MsgH);
				return NOMEMORY;
			}
			strcpy (linecopy,(char *) p);
		}
		length=strlen (linecopy);
		ProcessLine (linecopy,storage,SL_Via, SL_Path);
		p+=length;
	}
	while (*p);
	free (linecopy);
	if (storage->Origin.Zone==0)
		storage->Origin.Zone=storage->MSGIDZone;
	if (storage->Destination.Zone==0)
		storage->Destination.Zone=storage->MSGIDZone;
	AddressHandler.GetMain (&FQA);
	if (storage->Origin.Zone==0)
		storage->Origin.Zone=FQA.Zone;
	if (storage->Destination.Zone==0)
		storage->Destination.Zone=FQA.Zone;
        memcpy (&storage->TransitAddr,&storage->Destination,sizeof (S_FQAddress));

	storage->MessageSize=MsgGetTextLen (MsgH);
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
	sprintf (newpath,"%hu:%hu/%hu.%hu",storage->Origin.Zone,
		storage->Origin.Net,storage->Origin.Node,storage->Origin.Point);
	if (SL_Path!=NULL)
	{
		if (SL_Path->GetStringCount ()==0)
			SL_Path->AddString (newpath);
		else
			if (strcmp (SL_Path->GetString (0),newpath))
				SL_Path->InsertString (newpath,0);
	}
	MsgCloseMsg (MsgH);
	return SUCCESS;
}

int SquishToPKT (HAREA BaseH,unsigned long MsgNumber,char
                 *PKTPath,char *ext,S_FQAddress via,S_FQAddress
                 OurAKA,S_FQAddress FinalDest,int UseT2, int is_qqq)
{
	FILE *out;
	XMSG headerin;
	HMSG in;
	S_Packed headerout;
	S_PKT headerPKT;
	DATETIME dt;
	char *buffer,completepath[256],*Kludges;
	char NoPathSubject[80],lasttwo[2];
	long ToCopy,CurrentPos;
	size_t BufferSize;
	size_t read;
	in=MsgOpenMsg (BaseH,MOPEN_READ,MsgNumber);
	if (in==NULL)
		return ECONV_OPENFAILIN;
	if (MsgGetCtrlLen (in)>CONTROLBYTES)
	{
		ControlText=(unsigned char *)realloc (ControlText,(size_t) MsgGetCtrlLen (in));
		if (ControlText==NULL)
		{
			printf ("Not enough memory to read control lines.\n");
			return NOMEMORY;
		}
		CONTROLBYTES=MsgGetCtrlLen (in);
	}
	strcpy    (completepath,PKTPath);
	strcat    (completepath,ext);
        adaptcase (completepath);
	out=fopen (completepath,"r+b");
	if (out==NULL)
	{
		out=makedirandfopen (completepath,"w+b");
		if (out==NULL)
		{
			MsgCloseMsg (in);
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
	MsgReadMsg (in,&headerin,0L,0L,NULL,MsgGetCtrlLen (in),ControlText);
	
	if (DoRecode) {
		recodeToTransportCharset (headerin.from, 36);
		recodeToTransportCharset (headerin.to, 36);
		recodeToTransportCharset (headerin.subj, 72);
		recodeToTransportCharset (headerin.__ftsc_date, 20);
	}
	
	/* We truncate the last 2 bytes of the .PKT if they are both zero.
	   Otherwise it is likely that the .PKT is damaged and it is better
	   not to destroy anything on it */
	fseek (out,-2,SEEK_END);
	fread (lasttwo,1,2,out);
	if (lasttwo[0]==0 && lasttwo[1]==0)
		fseek (out,-2,SEEK_END);

	headerout.Signature=2;
	headerout.OrigNode=headerin.orig.node;
	headerout.OrigNet=headerin.orig.net;
	headerout.DestNode=headerin.dest.node;
	headerout.DestNet=headerin.dest.net;
	headerout.Attrib.number=headerin.attr;
	// Note that the InTransit and local flags are stripped (!!)
	headerout.Attrib.bits.InTransit=0;
	headerout.Attrib.bits.Local=0;
	headerout.Cost=0;
	memcpy (&headerout.DateTime,&headerin.__ftsc_date,20);
        fwrite (&headerout,1,sizeof (S_Packed),out);
	fwrite (&headerin.to,1,strlen ((char *) headerin.to)+1,out);
	fwrite (&headerin.from,1,strlen ((char *) headerin.from)+1,out);
	if (headerin.attr&MSGFILE)
		StripPath ((char *) headerin.subj,NoPathSubject);
	else
		strcpy (NoPathSubject,(char *) headerin.subj);
	fwrite (NoPathSubject,1,strlen (NoPathSubject)+1,out);
	// Write control text
#ifdef SMAPI_VERSION
        Kludges=(char *) CvtCtrlToKludge (ControlText);
#else
	Kludges=(char *) MsgCvtCtrlToKludge (ControlText);
#endif
	if (DoRecode)
		recodeToTransportCharset (Kludges, strlen(Kludges));
	fwrite (Kludges,1,strlen (Kludges),out);
#if !defined(OS_2) && !defined(SMAPI_VERSION)
        MsgFreeCtrlBuf (Kludges);
#else
        free (Kludges);
#endif
	ToCopy=MsgGetTextLen (in);
	BufferSize=(ToCopy>32767)?32767:(size_t) ToCopy;
	while (((buffer=(char *) malloc (BufferSize)) == NULL) && (BufferSize>1024))
		BufferSize = (BufferSize / 10) * 9;
	if (buffer==NULL)
	{
		MsgCloseMsg (in);
		fclose (out);
		return NOMEMORY;
	}
	/* Note that in the body copy loop we copy up to, but not including,
	   the first zero. If there is something beyond that zero, if won't
	   be copied, because that would result in a damaged .PKT. */
	CurrentPos=0;
	if (ToCopy>0)
	{
		while (ToCopy>0)
		{
			if (ToCopy>(long)((unsigned long)BufferSize))
				read=MsgReadMsg (in,NULL,CurrentPos,BufferSize,(unsigned char *) buffer,0L,NULL);
			else
				read=MsgReadMsg (in,NULL,CurrentPos,ToCopy,(unsigned char *) buffer,0L,NULL);
			if (strlen (buffer)<read)
			{
				read=strlen (buffer);
				ToCopy=read;
			}
			if (DoRecode)
				recodeToTransportCharset (buffer, read);
			fwrite (buffer,1,(size_t) read,out);
			ToCopy-=read;
			CurrentPos+=read;
		}
		if (buffer[(size_t) read-1]!='\r')
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
	MsgCloseMsg (in);
	fclose (out);
	return (SUCCESS);
}

int AnalyzeSquishNet (HAREA BaseH,unsigned long MsgNumber)
{
        S_Visu extra;
	struct S_Control x;
	x.GotSystem=0; x.is_qqq = 0;
	int killed=0;
	XMSG header;
	HMSG MsgH;
	sprintf (x.define," msg. %lu",MsgNumber);
	if (GetVisibleInfoSquish (BaseH,MsgNumber,&extra,NULL,NULL)!=SUCCESS)
		return FATAL;
	if (PostAnalysis (&extra,&x)==RET_SUCCESS)
		return SUCCESS;
	if (IgnoreBSY==0 && CheckAndCreateSem (x.savepath))
	{
                printf ("(System is now busy - message not exported)\n");
                Log.WriteOnLog ("This message was not exported because"
                                " the waypoint was busy.\n\n");
		return (ENH_DELAYED);
	}
	if (SquishToPKT (BaseH,MsgNumber,x.savepath,x.ext,
                         x.ShouldGo,x.OurAKA,extra.Destination,
                         Packet2Handler.DoesMatch (x.ShouldGo,
                                                   x.ShouldGo,0,0),
                         x.is_qqq)
            ==ECONV_OPENFAILOUT)
	{
		Log.WriteOnLog ("Warning: Failed to "
                                "open/create PKT (%s).\n\n",x.savepath);
                if (IgnoreBSY==0)
                        ReleaseSem (x.savepath);
		return (SUCCESS);
	}
	// Process file-attaches and file-requests
	if (extra.attrib.bits.FileAttached)
	{
		if (SubjectToFile(extra.Subject,x.savepathattach,x.extattach,
                                  extra.attrib2.bits.Truncate,
                                  extra.attrib2.bits.KillFileSent)!=SUCCESS)
			Log.WriteOnLog ("Warning: Failed to update"
                                        " fileattach queue.\n");
	}
	if (extra.attrib.bits.FileRequest)
	{
		strcpy (x.extattach,"REQ");
		if (SubjectToFile (extra.Subject,x.savepathattach,
                                   x.extattach,0,0)!=SUCCESS)
			Log.WriteOnLog ("Warning: Failed to "
                                        "update filerequest queue.\n");
	}
        if (extra.attrib.bits.KillSent ||
            (extra.attrib.bits.InTransit && KillInTransit))
	{
		MsgKillMsg (BaseH,MsgNumber);
		killed=1;
	}
	else
	{
		MsgH=MsgOpenMsg (BaseH,MOPEN_RW,MsgNumber);
		MsgReadMsg (MsgH,&header,0L,0L,NULL,0L,NULL);
		header.attr|=MSGSENT;
		MsgWriteMsg (MsgH,0,&header,NULL,0L,0L,0L,NULL);
		MsgCloseMsg (MsgH);
	}
	Log.WriteOnLog ("\n");
        if (IgnoreBSY==0)
                ReleaseSem (x.savepath);
	if (killed)
		return ENH_KILLED;
	return (ENH_PROCESSED);
}
#endif
