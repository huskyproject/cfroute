// Routing classes - here we store the 'via' systems and the
// address we want to route.

// CViaHandler provides the basic functions for every via
class CViaHandler
{
	int inited; // A viahandler is inited when its via system has been established
	int locked; // Locked -> An error in the via system was produced
	int Attrib; // Attribute
	int SendDirect; // List of direct systems?
	int RouteMail,RouteFiles; // What we route
	int RouteEncrypted, RouteNonEncrypted, EncryptInfo;
	S_FQAddress ViaSystem;
	word RouteFromCount,RouteToCount;
	S_WAddress **RouteToSystem,**RouteFromSystem;
	union
	{
		unsigned char number;
		struct
		{
			unsigned mon: 1;
			unsigned tue: 1;
			unsigned wed: 1;
			unsigned thu: 1;
			unsigned fri: 1;
			unsigned sat: 1;
			unsigned sun: 1;
			unsigned waste: 1;
		} x;
	} DOWs;
public:
	C_WildAddress WAHandler;
	CViaHandler (void);
	~CViaHandler (void);
	int SetViaSystem (S_FQAddress System);
	int GetViaSystem (S_FQAddress *System);
	int IsInited (void);
	int Unlock (void);
	int IsLocked (void);
	int AddNewRoutedAddress (S_WAddress NewAddress,int From);
	int GetRoutedToAddress (S_WAddress *storage,word count);
	int GetRoutedFromAddress (S_WAddress *storage,word count);
	int DoesMatch (S_FQAddress TargetAddress,S_FQAddress OriginAddress,int FileAttach, int Encrypted);
	int SetAttribType (int NewAttrib);
	int SetEncryption (int EncryptSpec);
	int SetDayOfTheWeek (char *spec);
	int GetAttribType (void);
	void SetStatus (int WantDirect,int WantMail,int WantFiles);
	int GetDirect (void);
	word GetRouteFromCount (void);
	word GetRouteToCount (void);
};

int CViaHandler::GetDirect (void)
{
	return SendDirect;
}

void CViaHandler::SetStatus (int WantDirect,int WantMail,int WantFiles)
{
	// A direct route doesn't need a via address, that's why we
	// considered the via inited if it contains direct system.
	SendDirect=WantDirect;
	if (SendDirect==NV_Direct || SendDirect==NV_Noroute ||SendDirect==NV_NoPack)
    	inited=1;
	RouteMail=WantMail;
	RouteFiles=WantFiles;
}

int CViaHandler::DoesMatch (S_FQAddress TargetAddress,S_FQAddress OriginAddress,int FileAttach, int Encrypted)
{
        int cur_Tostatus=0,cur_Fromstatus=0; // No match
	int count,last,except=0;
	unsigned char value;
	// Check if type of message match type of route
	if (((FileAttach) && (RouteFiles==0)) ||
		((!FileAttach) && (RouteMail==0)))
		return 0;
	// Check encryption
	if ((Encrypted) & (RouteEncrypted==0) ||
		((!Encrypted) && (RouteNonEncrypted==0)))
		return 0;
	// Day day of the week
	value=1<<GetDOW(); //1 = Monday, 2=Tuesday, 4=Wednesday...
	if ((DOWs.number&value)==0)
		return 0;
	// No data causes a mismatch.
	if (RouteToCount==0 && RouteFromCount==0)
		return 0;
	if (RouteToCount>0)
	{
		for (count=0;count<RouteToCount;count++)
		{
			if (RouteToSystem[count]->Reverse==1)
				except=!except; // Change the except condition
			last=CompareAddress (TargetAddress,RouteToSystem[count]);
			if (last)
				if (except)
					cur_Tostatus=0;
				else
					cur_Tostatus=1;
		}
	}
	else
		cur_Tostatus=1; // absence of a list means *!
	if (!cur_Tostatus) // Not on the To: list
		return 0;
	except=0;
	if (RouteFromCount>0)
	{
		for (count=0;count<RouteFromCount;count++)
		{
			if (RouteFromSystem[count]->Reverse==1)
				except=!except; // Change the except condition
			last=CompareAddress (OriginAddress,RouteFromSystem[count]);
			if (last)
				if (except)
					cur_Fromstatus=0;
				else
					cur_Fromstatus=1;
		}
	}
	else
		cur_Fromstatus=1; // absence of a list means *!
	return (cur_Fromstatus);
}

int CViaHandler::GetRoutedToAddress (S_WAddress *storage,word count)
{
	if (count>RouteToCount)
		return OUTRANGE;
	memcpy (storage,RouteToSystem[count],sizeof (struct S_WAddress));
	return SUCCESS;
}

int CViaHandler::GetRoutedFromAddress (S_WAddress *storage,word count)
{
	if (count>RouteFromCount)
		return OUTRANGE;
	memcpy (storage,RouteFromSystem[count],sizeof (struct S_WAddress));
	return SUCCESS;
}

word CViaHandler::GetRouteFromCount (void)
{
	return RouteFromCount;
}

word CViaHandler::GetRouteToCount (void)
{
	return RouteToCount;
}

int CViaHandler::AddNewRoutedAddress (S_WAddress NewAddress,int From)
{
	S_WAddress *NewSpec;
	NewSpec=(S_WAddress *) malloc (sizeof (S_WAddress));
	if (NewSpec==NULL)
		return NOMEMORY;
	memcpy (NewSpec,&NewAddress,sizeof (struct S_WAddress));
	if (From==1)
	{
		RouteFromCount++;
/*		if (RouteFromCount==1)
			RouteFromSystem=(S_WAddress **) malloc (sizeof (struct S_WAddress *));
		else */
			RouteFromSystem=(S_WAddress **) realloc (RouteFromSystem,RouteFromCount*sizeof (struct S_WAddress *));
		if (RouteFromSystem==NULL)
			return NOMEMORY;
		RouteFromSystem[RouteFromCount-1]=NewSpec;
	}
	else
	{
		RouteToCount++;
/*		if (RouteToCount==1)
			RouteToSystem=(S_WAddress **) malloc (sizeof (struct S_WAddress *));
		else */
			RouteToSystem=(S_WAddress **) realloc (RouteToSystem,RouteToCount*sizeof (struct S_WAddress *));
		if (RouteToSystem==NULL)
			return NOMEMORY;
		RouteToSystem[RouteToCount-1]=NewSpec;
	}
	return SUCCESS;
}

int CViaHandler::IsInited (void)
{
	return inited;
}

int CViaHandler::Unlock (void)
{
	locked=0;
	SendDirect=0;
	return SUCCESS;
}

int CViaHandler::IsLocked (void)
{
	return locked;
}

int CViaHandler::SetDayOfTheWeek (char *spec)
{
	char *p;
	DOWs.number=0;
	p=spec;
	while (*p)
	{
		switch (*p)
		{
			case 'M':
				DOWs.x.mon=1;
				break;
			case 'T':
				DOWs.x.tue=1;
				break;
			case 'W':
				DOWs.x.wed=1;
				break;
			case 'H':
				DOWs.x.thu=1;
				break;
			case 'F':
				DOWs.x.fri=1;
				break;
			case 'S':
				DOWs.x.sat=1;
				break;
			case 'U':
				DOWs.x.sun=1;
				break;
		}
        p++;
	}
	return SUCCESS;
}

int CViaHandler::SetEncryption (int EncryptSpec)
{
	switch (EncryptSpec)
	{
		case TT_ENCRYPTED:
			if (RouteEncrypted==0)
				return (EVH_NONSENSE);
			RouteNonEncrypted=0;
			break;
		case TT_NONENCRYPTED:
			if (RouteNonEncrypted==0)
				return (EVH_NONSENSE);
			RouteEncrypted=0;
			break;
		default:
			return (EVH_INVALID);
	}
	return SUCCESS;
}

int CViaHandler::SetAttribType (int NewAttrib)
{
	if (NewAttrib!=TT_NORMAL && NewAttrib!=TT_HOLD && NewAttrib!=TT_CRASH &&
		NewAttrib!=TT_DIR)
		return EVH_INVALID;
	else
	{
		Attrib=NewAttrib;
		return SUCCESS;
	}
}

int CViaHandler::GetAttribType (void)
{
	return Attrib;
}

CViaHandler::CViaHandler (void)
{
	inited=0;
	locked=0;
	SendDirect=0;
	RouteFromCount=RouteToCount=0;
	Attrib=TT_NORMAL;
	DOWs.number=255;
	RouteEncrypted=1;
	RouteNonEncrypted=1;
	EncryptInfo=0;
	RouteToSystem=RouteFromSystem=NULL;
}

CViaHandler::~CViaHandler (void)
{
	word count;
	for (count=0;count<RouteToCount;count++)
		free (RouteToSystem[count]);
	for (count=0;count<RouteFromCount;count++)
		free (RouteFromSystem[count]);
	free (RouteToSystem);
	free (RouteFromSystem);
}

int CViaHandler::SetViaSystem (S_FQAddress System)
{
	if (inited==1)
		return EVH_TWOVIA;
	memcpy (&ViaSystem,&System,sizeof (struct S_FQAddress));
	inited=1;
	return SUCCESS;
}

int CViaHandler::GetViaSystem (S_FQAddress *System)
{
	if (inited==0)
		return EVH_NOVIA;
	if (SendDirect==NV_Routed)
	{
		memcpy (System,&ViaSystem,sizeof (struct S_FQAddress));
		return SUCCESS;
	}
	if (SendDirect==NV_Direct)
		return EVH_DIRECT;
	if (SendDirect==NV_NoPack)
		return EVH_NOPACK;
	return EVH_NOROUTE;
}

// CRouteHandler provides the interface for external parts.
class CRouteHandler
{
	word ViaCount;
	CViaHandler **ViasArray;
	int OnExcept,OnFrom,TopPriority,AssumePoints,AssumeWaypoints,SetAs;
public:
	void SetTopDownMode (void);
	int SetupNewVia (int SendDirect,int SendMail, int SendFiles);
	CRouteHandler (void);
	int Process (char *Address);
	word GetViaCount (void);
	int GetViaSystem (S_FQAddress *storage,int count);
	word GetRouteToCount (word count);
	word GetRouteFromCount (word count);
	int GetRoutedFromAddress (S_WAddress *storage,word Viacount,word RouteCount);
	int GetRoutedToAddress (S_WAddress *storage,word Viacount,word RouteCount);
	void SetOnFrom (void);
	void SwitchExcept (void);
	int FindPackSystem (S_FQAddress NetTo,S_FQAddress NetFrom,int FileAttach,int Encrypted, S_FQAddress *storage,int *storeattrib);
	int NoPack (S_FQAddress NetTo,S_FQAddress NetFrom,int FileAttach,int Encrypted);
	int SetAttrib (int NewAttrib);
	int SetDayOfTheWeek (char *DOWspec);
	int SetEncryption (int EncryptSpec);
	void SetAssumePoints (int Value);
	void SetAssumeWaypoints (int Value);
};

void CRouteHandler::SetAssumeWaypoints (int Value)
{
	AssumeWaypoints=Value;
}

void CRouteHandler::SetAssumePoints (int Value)
{
	AssumePoints=Value;
}

// Constructor
CRouteHandler::CRouteHandler (void)
{
	ViaCount=0;
	OnExcept=0;
	OnFrom=0;
	TopPriority=0;
	AssumePoints=0;
	AssumeWaypoints=0;
	SetAs=0;
}

int CRouteHandler::GetViaSystem (S_FQAddress *storage, int count)
{
	if (count>=ViaCount)
		return OUTRANGE;
	return (ViasArray[count]->GetViaSystem (storage));
}

word CRouteHandler::GetRouteToCount (word count)
{
	if (count>=ViaCount)
		return SPECIAL_OUTRANGE;
	return (ViasArray[count]->GetRouteToCount ());
}

int CRouteHandler::GetRoutedFromAddress (S_WAddress *storage,word Viacount,word RouteCount)
{
	if (Viacount>=ViaCount)
		return OUTRANGE;
	return (ViasArray[Viacount]->GetRoutedFromAddress (storage,RouteCount));
}

int CRouteHandler::GetRoutedToAddress (S_WAddress *storage,word Viacount,word RouteCount)
{
	if (Viacount>=ViaCount)
		return OUTRANGE;
	return (ViasArray[Viacount]->GetRoutedToAddress (storage,RouteCount));
}


word CRouteHandler::GetRouteFromCount (word count)
{
	if (count>=ViaCount)
		return SPECIAL_OUTRANGE;
	return (ViasArray[count]->GetRouteFromCount ());
}

word CRouteHandler::GetViaCount (void)
{
	CViaHandler *InUseViaHandler;
	int correction=0;
	if (ViaCount>0)
	{
		InUseViaHandler=ViasArray[ViaCount-1];
		if (InUseViaHandler->IsInited()==0)
			correction=1; // Uninited vias don't count
	}
	return ViaCount-correction;
}

// Process an address. It may be the via system or the routed systems
void CRouteHandler::SetOnFrom (void)
{
	OnFrom=1;
}

void CRouteHandler::SwitchExcept (void)
{
	OnExcept=!OnExcept;

// TE 990805: I commented out the following line because EXCEPT can be
//            applied to either From or To statements.
//	OnFrom=0;  

}

int CRouteHandler::Process (char *Address)
{
	CViaHandler *InUseViaHandler;
	S_FQAddress FQA;
	S_WAddress WA,Record;
	int SetTarget=0;
	if (ViaCount==0)
		return EVH_NO_VIAHANDLER;
	InUseViaHandler=ViasArray[ViaCount-1];
	if (InUseViaHandler->IsLocked())
		return EVH_LOCKED;
	if (InUseViaHandler->IsInited()==0) // Incoming address is the via system
	{
		if (GetFullQualifiedAddress (Address,&FQA,NULL)!=SUCCESS)
		{
			printf ("%s is not a valid address for a waypoint specification. Must be 4D.\n",Address);
			return FATAL;
		}
		InUseViaHandler->SetViaSystem (FQA);
		// We do this to give a feed to the resolver
		InUseViaHandler->WAHandler.ParseAddress (Address,&WA);
		if (AssumeWaypoints)
			SetTarget=1;
		else
			SetTarget=0;
	}
	else
		SetTarget=1;
	if (SetTarget)
	{
		if (InUseViaHandler->WAHandler.ParseAddress (Address,&WA)!=SUCCESS)
		{
			printf ("Warning: %s is invalid, ignored",Address);
			return WARNING;
		}
		WA.Reverse=OnExcept;
		OnExcept=0;
		if (AssumePoints)
		{
			if (!strchr (Address,'.'))
				strcpy (WA.Point,"*");
		}
		memcpy (&Record,&WA,sizeof (struct S_WAddress));
		return (InUseViaHandler->AddNewRoutedAddress (WA,OnFrom));
	}
	return SUCCESS;
}

int CRouteHandler::SetDayOfTheWeek (char *DOWspec)
{
	CViaHandler *InUseViaHandler;
	if (ViaCount==0)
		return EVH_NO_VIAHANDLER;
	InUseViaHandler=ViasArray[ViaCount-1];
	if (InUseViaHandler->IsLocked())
		return EVH_LOCKED;
	return (InUseViaHandler->SetDayOfTheWeek (DOWspec));
}

int CRouteHandler::SetAttrib (int NewAttrib)
{
	CViaHandler *InUseViaHandler;
	if (ViaCount==0)
		return EVH_NO_VIAHANDLER;
	InUseViaHandler=ViasArray[ViaCount-1];
	if (InUseViaHandler->IsLocked())
		return EVH_LOCKED;
	return (InUseViaHandler->SetAttribType (NewAttrib));
}

int CRouteHandler::SetEncryption (int EncryptSpec)
{
	CViaHandler *InUseViaHandler;
	if (ViaCount==0)
		return EVH_NO_VIAHANDLER;
	InUseViaHandler=ViasArray[ViaCount-1];
	if (InUseViaHandler->IsLocked())
		return EVH_LOCKED;
	return (InUseViaHandler->SetEncryption (EncryptSpec));
}

// Add a new 'via' system in the array
int CRouteHandler::SetupNewVia (int SendDirect,int RouteMail,int RouteFiles)
{
	CViaHandler *NewViaHandler;
	OnFrom=0;
	OnExcept=0;
	if (ViaCount>0) // Let's see if we can use the previous via...
	{
		NewViaHandler=ViasArray[ViaCount-1];
		if (NewViaHandler->IsLocked())
			NewViaHandler->Unlock();
		else
			if (NewViaHandler->IsInited()) // Need a new via
			{
				ViaCount++;
				NewViaHandler=new (CViaHandler);
				if (NewViaHandler==NULL)
					return NOMEMORY;
				ViasArray=(CViaHandler **) realloc (ViasArray,ViaCount*sizeof (class CViaHandler *));
				if (ViasArray==NULL)
					return NOMEMORY;
				ViasArray[ViaCount-1]=NewViaHandler;
			}
	}
	else // No vias yet - array must be created
	{
		ViaCount++;
		NewViaHandler=new (CViaHandler);
		if (NewViaHandler==NULL)
			return NOMEMORY;
		ViasArray=(CViaHandler **) malloc (sizeof (class CViaHandler *));
		if (ViasArray==NULL)
			return NOMEMORY;
		ViasArray[ViaCount-1]=NewViaHandler;
	}
	NewViaHandler->SetStatus (SendDirect,RouteMail,RouteFiles);
	return SUCCESS;
}

void CRouteHandler::SetTopDownMode (void)
{
	TopPriority=1;
}

int CRouteHandler::NoPack (S_FQAddress NetTo,S_FQAddress NetFrom,int FileAttach,int Encrypted)
{
	int count,Ignore=0;
	CViaHandler *InUse;
	for (count=0;count<GetViaCount ();count++)
	{
		InUse=ViasArray[count];
		if (InUse->DoesMatch (NetTo,NetFrom,FileAttach,Encrypted)==1)
		{
			if (InUse->GetDirect ()==NV_NoPack)
				Ignore=1;
			else
				Ignore=0;
		}
	}
	return Ignore;
}

int CRouteHandler::FindPackSystem (S_FQAddress NetTo,S_FQAddress NetFrom,int FileAttach,int Encrypted, S_FQAddress *storage,int *storeattrib)
{
	int count,GotAMatch=0;
	CViaHandler *InUse;
	for (count=0;count<GetViaCount ();count++)
	{
		InUse=ViasArray[count];
		if (InUse->DoesMatch (NetTo,NetFrom,FileAttach,Encrypted)==1)
		{
#ifndef DEBUG
	if (RequestInfo)
#endif
	{
		printf ("To: %u:%u/%u.%u  From: %u:%u/%u.%u matches definition %d",
				NetTo.Zone,NetTo.Net,NetTo.Node,NetTo.Point,
				NetFrom.Zone,NetFrom.Net,NetFrom.Node,NetFrom.Point,
				count);
	}
			GotAMatch=1;
			switch (InUse->GetViaSystem (storage))
			{
				case EVH_DIRECT:
#ifndef DEBUG
					if (RequestInfo)
#endif
					{
						printf (" direct.\n");
					}
					memcpy (storage,&NetTo,sizeof (struct S_FQAddress));
					break;
				case EVH_NOROUTE:
#ifndef DEBUG
					if (RequestInfo)
#endif
						printf (" no-route.\n");

					memcpy (storage,&NetTo,sizeof (struct S_FQAddress));
					storage->Point=0;
					break;
				case EVH_NOPACK:
#ifndef DEBUG
					if (RequestInfo)
#endif
					printf (" no-pack.\n");

					GotAMatch=0;
				default:
					{
#ifndef DEBUG
	if (RequestInfo)
#endif
						printf (" (via %u:%u/%u.%u)\n",
							storage->Zone,storage->Net,
							storage->Node,storage->Point);
					}
					;
			}
			*storeattrib=InUse->GetAttribType ();
		}
		if (GotAMatch && TopPriority)
			return 1;
	}
	return GotAMatch;
}
