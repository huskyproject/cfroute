/*
class C_Enc
{
	// Class stats
	int Inited;
	// Message stats
	int CurMsgEncrypted;
	int ProcessedLines;
	int EncryptedLines;
	// Configuration
	int MinLineLength; // To be encoded (10-70)
	int MinEncryptedLines; // Min 6
	int MinSpaces; // 10%
	int MinMsgLines;
	int MinPercentage;
public:
	C_Enc (void);
	int SetMinEncryptedLines (int NewMin);
	int SetMinLineLength (int NewLineLength);
	int SetMinSpaces (int NewMin);
	void NewMessage (void);
	int SetMinMsgLines (int NewMin);
	int Feed (char *line);
};

void C_Enc::NewMessage (void)
{
	CurMsgEncrypted=0;
	ProcessedLines=0;
	EncryptedLines=0;
	Inited=1;
}

C_Enc::C_Enc (void)
{
	MinLineLength=30;
	MinEncryptedLines=10;
	MinSpaces=7;
	MinMsgLines=15;
	MinPercentage=40;
	NewMessage ();
	Inited=0;
}

int C_Enc::SetMinLineLength (int NewLineLength)
{
	if (NewLineLength<10 || NewLineLength>70)
		return OUTRANGE;
	MinLineLength=NewLineLength;
	return SUCCESS;
}

int C_Enc::SetMinEncryptedLines (int NewMin)
{
	if (NewMin<6)
		return OUTRANGE;
	MinEncryptedLines=NewMin;
	return SUCCESS;
}

int C_Enc::SetMinSpaces (int NewMin)
{
	if (NewMin<0 || NewMin>10)
		return OUTRANGE;
	MinSpaces=NewMin;
	return SUCCESS;
}

int C_Enc::SetMinMsgLines (int NewMin)
{
	if (NewMin<0)
		return OUTRANGE;
	MinMsgLines=NewMin;
	return SUCCESS;
}

int C_Enc::Feed (char *line)
{
	int counter,linelength;
	if (Inited==0)
		return NOTINITED;
	if ((linelength=strlen (line))<MinLineLength)
		return SUCCESS;

 	return SUCCESS;
}
*/
