// Small password-handling class

struct PassAdd
{
	char Password[9];
	S_WAddress Address;
};

class C_Password
{
	PassAdd **PasArray;
	int inited;
	int currentinited,currentcount;
	char LastPassword[9];
public:
    int AddItem (S_WAddress address,char *password);
	C_Password (void);
	int NewPassword (void);
	int Process (char *token);
	int GetPasswordCount (void);
	int GetPair (PassAdd *storage,int number);
	int GetPassword (S_FQAddress address, char *storage);
};

int C_Password::GetPasswordCount (void)
{
	return currentcount;
}

int C_Password::GetPair (PassAdd *storage,int number)
{
	if (number>=currentcount)
		return OUTRANGE;
	memcpy (storage,PasArray[number],sizeof (struct PassAdd));
	return SUCCESS;
}

C_Password::C_Password (void)
{
	inited=0;
	currentinited=0;
	currentcount=0;
	PasArray=NULL;
}

int C_Password::AddItem (S_WAddress address,char *password)
{
	PassAdd *NewItem;
	PasArray=(PassAdd **) realloc (PasArray,(currentcount+1)*sizeof (struct PassAdd *));
	if (PasArray==NULL)
		return NOMEMORY;
	NewItem=(PassAdd *) malloc (sizeof (struct PassAdd));
	if (NewItem==NULL)
		return NOMEMORY;
	strcpy ((char *) &NewItem->Password,password);
	memcpy (&NewItem->Address,&address,sizeof (struct S_WAddress));
	PasArray[currentcount]=NewItem;
	currentcount++;
	return SUCCESS;
}

int C_Password::NewPassword (void)
{
	currentinited=0;
	return SUCCESS;
}

int C_Password::Process (char *token)
{
	static S_WAddress Proc,Last,*point;
	if (currentinited==0) // It is a password
	{
		if (strlen (token)>8)
			return EPH_TOOLONG;
		strcpy (LastPassword,token);
		currentinited=1;
		point=NULL;
	}
	else
	{
		if (SplitWildcardedAddress (token,&Proc,point)!=SUCCESS)
			return EPH_INVALID;
        memcpy (&Last,&Proc,sizeof (struct S_WAddress));
		point=&Last;
		return (AddItem (Proc,LastPassword));
	}
	return SUCCESS;
}

int C_Password::GetPassword (S_FQAddress address, char *storage)
{
	int count;
#ifdef DEBUG
	printf ("GetPassword: Requested password for %u:%u/%u.%u...",
		address.Zone,address.Net,address.Node,address.Point);
#endif
	for (count=0;count<currentcount;count++)
		if  (CompareBit (address.Zone,PasArray[count]->Address.Zone) &&
			CompareBit (address.Net,PasArray[count]->Address.Net) &&
			CompareBit (address.Node,PasArray[count]->Address.Node) &&
			CompareBit (address.Point,PasArray[count]->Address.Point))
		{
#ifdef DEBUG
	printf ("found %s.\n",PasArray[count]->Password);
#endif
                        if (storage != NULL)
                        	memcpy (storage,PasArray[count]->Password,
                                        strlen (PasArray[count]->Password));
			return SUCCESS;
		}
#ifdef DEBUG
	printf ("not found.\n");
#endif

    return EPH_NOTFOUND;
}
