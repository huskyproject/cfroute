class Macro: public C_StringList
{
	public:
		char MacroName[256];
};

class C_MacroContainer
{
	Macro **Macros;
	int MacroCount;
	int OnNewMacro;
public:
	C_MacroContainer (void);
	~C_MacroContainer (void);
	int Process (char *Token);
	Macro *FindMacro (char *Name);
	int GetMacroCount (void);
	Macro *GetMacro (int number);
};

int C_MacroContainer::GetMacroCount (void)
{
	return MacroCount;
}

Macro *C_MacroContainer::GetMacro (int number)
{
	if (number<MacroCount)
		return Macros[number];
	else
		return NULL;
}

Macro *C_MacroContainer::FindMacro (char *Name)
{
	int counter;
	for (counter=0;counter<MacroCount;counter++)
                if (cistrcmp (Name,Macros[counter]->MacroName)==0)
			return (Macros[counter]);
	return NULL;
}

C_MacroContainer::C_MacroContainer (void)
{
	Macros=NULL;
	MacroCount=0;
	OnNewMacro=1;
}

C_MacroContainer::~C_MacroContainer (void)
{
	int count;
	for (count=0;count<MacroCount;count++)
		delete (Macros[count]);
	free (Macros);
	Macros=NULL;
}

int C_MacroContainer::Process (char *Token)
{
	class Macro *NewMacro;
        if (!cistrcmp (Token,"ENDDEFINE"))
	{
		OnNewMacro=1;
		return SUCCESS;
	}
	if (OnNewMacro)
	{
		if (FindMacro (Token)!=NULL)
			return EMH_ALREADYDEFINED;
		Macros=(Macro **) realloc (Macros,(MacroCount+1)*sizeof (Macro *));
		if (Macros==NULL)
			return NOMEMORY;
		NewMacro=new (Macro);
		if (NewMacro==NULL)
			return NOMEMORY;
		Macros[MacroCount]=NewMacro;
		strcpy (NewMacro->MacroName,Token);
		MacroCount++;
		OnNewMacro=0;
	}
	else
	{
		NewMacro=Macros[MacroCount-1];
		NewMacro->AddString (Token);
	}
	return NEEDEXTRAPAR;
}
