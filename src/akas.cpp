// Small class to handle our own main address and AKA
// All the addresses stored here MUST be full qualified addresses -
// no wildcards allowed here.

class CAddresses
{
	S_FQAddress Main; // Main address
	S_FQAddress **AKAs; // Rest of AKAs stored as dynamic memory
	int Inited;
	int AKACount; // Number of AKAs we have
public:
	int StoreMain (S_FQAddress MainAddress);
	int GetMain (S_FQAddress *storage);
	int StoreAKA (S_FQAddress AKAToStore);
	int GetAKA (S_FQAddress *storage,word AKANumber);
	int MainDefined ();
	int GetAKACount ();
	int IsLocalAKA (S_FQAddress TestAKA);
	int OurZone (word CheckZone);
	int OurNet (word CheckZone,word CheckNet);
	int AKAMatch (S_FQAddress compare,S_FQAddress *storage);
	S_FQAddress *GetLastAKA (void);
	CAddresses ();
	~CAddresses ();
};

S_FQAddress *CAddresses::GetLastAKA (void)
{
	if (Inited==0)
		return NULL;
	if (AKACount==0)
		return &Main;
	return AKAs[AKACount-1];
}

int CAddresses::OurNet (word CheckZone,word CheckNet)
{
	S_FQAddress work;
	int count;
	if (Inited==0)
		return EAD_NOMAIN;
	for (count=0;count<=GetAKACount();count++)
	{
		GetAKA (&work,count);
		if (work.Zone==CheckZone && work.Net==CheckNet)
			return EAD_OURNET;
	}
	return EAD_FOREIGN;
}

int CAddresses::OurZone (word CheckZone)
{
	S_FQAddress work;
	int count;
	if (Inited==0)
		return EAD_NOMAIN;
	for (count=0;count<=GetAKACount();count++)
	{
		GetAKA (&work,count);
		if (work.Zone==CheckZone)
			return EAD_OURZONE;
	}
	return EAD_FOREIGN;
}

int CAddresses::StoreMain (S_FQAddress MainAddress)
{
	if (Inited)
		return EAD_TWOMAIN;
	memcpy (&Main,&MainAddress,sizeof (struct S_FQAddress));
	Inited=1;
	return SUCCESS;
}

int CAddresses::GetMain (S_FQAddress *storage)
{
	if (!Inited)
		return EAD_NOMAIN;
	memcpy (storage,&Main,sizeof (struct S_FQAddress));
	return SUCCESS;
}

int CAddresses::StoreAKA (S_FQAddress AKAToStore)
{
	S_FQAddress *AKA;
	// Get memory for the new AKA
	if (IsLocalAKA (AKAToStore))
		return SUCCESS;
	AKA=(S_FQAddress *) malloc (sizeof (struct S_FQAddress));
	if (AKA==NULL)
		return NOMEMORY;
	// Expand or create array...
	AKACount++;
	AKAs=(S_FQAddress **) realloc (AKAs,AKACount*sizeof (struct S_FQAddress));
	if (AKAs==NULL)
		return NOMEMORY;
	memcpy (AKA,&AKAToStore,sizeof (struct S_FQAddress));
	AKAs[AKACount-1]=AKA;
	return SUCCESS;
}

int CAddresses::IsLocalAKA (S_FQAddress TestAKA)
{
	S_FQAddress Get;
	int contador;
	if (!Inited)
		return 0;
	for (contador=0;contador<=AKACount;contador++)
	{
		GetAKA (&Get,contador);
		if ((Get.Domain[0]==0 || TestAKA.Domain[0]==0) ||
			(cistrcmp (Get.Domain,TestAKA.Domain)==0))
			if (Get.Zone==TestAKA.Zone && Get.Net==TestAKA.Net &&
				Get.Node==TestAKA.Node && Get.Point==TestAKA.Point)
				return 1;
	}
	return 0;
}

int CAddresses::GetAKA (S_FQAddress *storage,word AKANumber)
{
	S_FQAddress *origin;
	if ((AKANumber>AKACount) || ( (AKANumber==0) && (!Inited) ))
		return OUTRANGE;
	if (AKANumber==0)
		origin=&Main;
	else
		origin=AKAs[AKANumber-1];
	memcpy (storage,origin,sizeof (struct S_FQAddress));
	return SUCCESS;
}

int CAddresses::MainDefined ()
{
	return Inited;
}

int CAddresses::GetAKACount ()
{
	return AKACount;
}
CAddresses::CAddresses ()
{
	Inited=0;
	AKACount=0;
	AKAs=NULL;
}

CAddresses::~CAddresses ()
{
	int counter;
	for (counter=0;counter<AKACount;counter++)
		free (AKAs[counter]);
	free (AKAs);
}

int CAddresses::AKAMatch (S_FQAddress compare,S_FQAddress *storage)
{
	int count,bestsofar,current;
	S_FQAddress work;
	if (Inited==0)
		return EAD_NOMAIN;
	GetAKA (storage,0); // Default to main
	bestsofar=GetMatchLevel (*storage,compare);
	for (count=0;count<=GetAKACount();count++)
	{
		GetAKA (&work,count);
		if ((current=GetMatchLevel (work,compare))>bestsofar)
		{
			memcpy (storage,&work,sizeof (struct S_FQAddress));
			bestsofar=current;
		}
	}
	return SUCCESS;
}