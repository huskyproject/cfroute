/* This module contains the parameter parses. All functions
   have the same calling parameters. */

#ifndef OS_2
#pragma warn -par
#endif

int Packet2Except=0;

int ProcParCheckFile (char *Token, S_FQAddress Main)
{
	return (CheckFiles.AddString (Token));
}

int ProcParPassword (char *Token, S_FQAddress Main)
{
	if (PasswordHandler.Process (Token)!=SUCCESS)
	{
		printf ("Fatal: %s can't be parsed.\n",Token);
		return FATAL;
	}
	return SUCCESS;
}

int ProcParNetmail (char *Token, S_FQAddress Main)
{
	int Result=SUCCESS;
	char NetmailDirectory[256];
	strcpy (NetmailDirectory,Token);
	if (NetmailDirectory[strlen (NetmailDirectory)-1]!=':' &&
		NetmailDirectory[strlen (NetmailDirectory)-1]!=DIRSEPC)
	strcat (NetmailDirectory,DIRSEPS);
	SL_NetmailDir.AddString (NetmailDirectory);
#ifdef DEBUG
	printf ("Added Netmail directory %s.\n",NetmailDirectory);
#endif
	return Result;
}

#ifdef SQUISHCFS
int ProcParNetSquish (char *Token, S_FQAddress Main)
{
	int Result=SUCCESS;
	char SquishBase[256];
	strcpy (SquishBase,"!");
	strcat (SquishBase,Token);
	SL_NetmailDir.AddString (SquishBase);
	UseSquish=1;
#ifdef DEBUG
	printf ("Added Squishbase %s.\n",SquishBase);
#endif
	return Result;
}
#endif

int ProcParOutboundDir (char *Token, S_FQAddress Main)
{
	if (OutboundDirSetup)
	{
		printf ("Warning: already had outbound directory (%s)\n",OutboundDirectory);
		WarningCount++;
	}
	strcpy (OutboundDirectory,Token);
	if (OutboundDirectory[strlen (OutboundDirectory)-1]==DIRSEPC)
		OutboundDirectory[strlen (OutboundDirectory)-1]=0;
#ifdef DEBUG
	printf ("Outbound directory is %s.\n",OutboundDirectory);
#endif
	OutboundDirSetup=1;
	return SUCCESS;
}

int ProcParQQQOutboundDir (char *Token, S_FQAddress Main)
{
	if (QQQOutboundDirSetup)
	{
		printf ("Warning: already had QQQ outbound directory (%s)\n",QQQOutboundDirectory);
		WarningCount++;
	}
	strcpy (QQQOutboundDirectory,Token);
	if (QQQOutboundDirectory[strlen (QQQOutboundDirectory)-1]==DIRSEPC)
		QQQOutboundDirectory[strlen (QQQOutboundDirectory)-1]=0;
#ifdef DEBUG
	printf ("QQQ Outbound directory is %s.\n",QQQOutboundDirectory);
#endif
	QQQOutboundDirSetup=1;
	return SUCCESS;
}

        


int InitComPassword (S_FQAddress Main)
{
	PasswordHandler.NewPassword ();
	return SUCCESS;
}

int InitComTopdown (S_FQAddress Main)
{
	RouteHandler.SetTopDownMode ();
	return SUCCESS;
}

// Route-to
int InitComRouteTo (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Routed,True,True)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int InitComRouteMail (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Routed,True,False)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int InitComRouteFiles (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Routed,False,True)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}
// Nopack
int InitComNoPack (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_NoPack,True,True)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int InitComNoPackMail (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_NoPack,True,False)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int InitComNoPackFiles (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_NoPack,False,True)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

// Direct
int InitComDirect (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Direct,True,True)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int InitComDirectMail (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Direct,True,False)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int InitComDirectFiles (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Direct,False,True)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

// Route-boss
int InitComNoroute (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Noroute,True,True)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int InitComNorouteMail (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Noroute,True,False)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int InitComNorouteFiles (S_FQAddress Main)
{
	if (RouteHandler.SetupNewVia (NV_Noroute,False,True)!=SUCCESS)
		return FATAL;
	return SUCCESS;
}

int ProcParRoute (char *Token, S_FQAddress Main)
{
	S_Codes Info;
	GetTokenInfo (Token,&Info);
	switch (Info.Group)
	{
		case PT_DOW:
			return (RouteHandler.SetDayOfTheWeek (Token));
		case PT_ADDRESS:
			switch (RouteHandler.Process (Token))
			{
				case EVH_CANTRESOLVE:
					printf ("Error: %s is not correct as via system address - route-to ignored.\n",Token);
					return WARNING;
				case EVH_INVALID:
					printf ("Error: %s is not a valid address - ignored.\n",Token);
					return WARNING;
				case WARNING:
					return WARNING;
				case FATAL:
					return FATAL;
			}
			return SUCCESS;
		case PT_ENC:
			if (RouteHandler.SetEncryption (Info.Code)!=SUCCESS)
			{
				printf ("Warning: Nonsense encryption construction");
				return WARNING;
			}
			break;
		case PT_FLAVOUR:
			if (RouteHandler.SetAttrib (Info.Code)!=SUCCESS)
			{
				printf ("Unexpected error handling attribute %s",Token);
				return WARNING;
			}
			break;
		case PT_FROM:
			RouteHandler.SetOnFrom ();
			break;
		case PT_EXCEPT:
			RouteHandler.SwitchExcept ();
			break;
		default:
			printf ("%s can't be used with route statements ",Token);
			return WARNING;
	}
	return SUCCESS;
}

int ProcParLogFilename (char *Token, S_FQAddress Main)
{
	if (Log.SetLogFileName (Token)!=SUCCESS)
	{
		printf ("Warning: Log filename already established.\n");
		WarningCount++;
	}
	else
	if (Log.OpenLog ()!=SUCCESS)
	{
		printf ("Warning: Unable to open log.\n");
		WarningCount++;
	}
	return SUCCESS;
}

int ProcParRecode (char *Token, S_FQAddress Main)
{
	return getctab (outtab, Token);
}

int ProcParInclude (char *Token, S_FQAddress Main)
{
	return (ReadRouteFile (Token));
}

int InitComPacket2 (S_FQAddress Main)
{
	Packet2Except=0;
	return SUCCESS;
}

int InitComProductInfo (S_FQAddress Main)
{
	IDENTString[0]=0;
	return SUCCESS;
}

int ProcParProductInfo (char *Token,S_FQAddress Main)
{
	if (strlen (IDENTString)+strlen (Token)<80)
		strcat (IDENTString,Token);
	strcat (IDENTString," ");
	return SUCCESS;
}

int ProcParPacket2 (char *Token,S_FQAddress Main)
{
	S_WAddress Packet2work;
	S_Codes Info;
	GetTokenInfo (Token,&Info);
	switch (Info.Group)
	{
		case PT_ADDRESS:
			if (Packet2Handler.WAHandler.ParseAddress (Token,&Packet2work)!=SUCCESS)
			{
				printf ("Warning: Unable to use %s as packet2 system",Token);
				return WARNING;
			}
			else
			{
				Packet2work.Reverse=Packet2Except;
				Packet2Handler.AddNewRoutedAddress (Packet2work,0);
			}
			break;
		case PT_EXCEPT:
			Packet2Except=!Packet2Except;
			break;
		default:
			printf ("%s Can't be used with Packet2",Token);
			return WARNING;
	}
	return SUCCESS;
}

int ProcParMain (char *Token,S_FQAddress Main)
{
	S_FQAddress OnProcess;
	if (GetFullQualifiedAddress (Token,&OnProcess,&Main)!=SUCCESS)
	{
		printf ("%s is not a correct specification for a main address",Token);
		return WARNING;
	}
	else
	{
		if (AddressHandler.StoreMain (OnProcess)!=SUCCESS)
		{
			printf ("Error: Two main addresses\n");
			return FATAL;
		}
	}
	return SUCCESS;
}

int ProcParAKA (char *Token,S_FQAddress Main)
{
	S_FQAddress OnProcess;
	if (GetFullQualifiedAddress (Token,&OnProcess,AddressHandler.GetLastAKA())!=SUCCESS)
	{
		printf ("%s is not a complete address. Make sure you have defined your main aka.\n",Token);
		return FATAL;
	}
	else
		if (AddressHandler.StoreAKA (OnProcess)!=SUCCESS)
		{
			printf ("Error: Failed to store AKA %s",Token);
			return FATAL;
		}
	return SUCCESS;
}

int ProcParDefine (char *Token,S_FQAddress Main)
{
	return (MacroHandler.Process (Token));
}

int ProcParFEConfig (char *Token,S_FQAddress Main)
{
	strcpy (FEConfDir,Token);
	return SUCCESS;
}

int InitComEndDefine (S_FQAddress Main)
{
	printf ("Fatal: Unexpected ENDDEFINE.\n");
	return FATAL;
}

int InitComAssumePoints (S_FQAddress Main)
{
	RouteHandler.SetAssumePoints (1);
	return SUCCESS;
}

int InitComAssumeWaypoints (S_FQAddress Main)
{
	RouteHandler.SetAssumeWaypoints (1);
	return SUCCESS;
}

int InitComEOLEndsCommand (S_FQAddress Main)
{
	EOLEndsCommand=1;
	return SUCCESS;
}

int InitComViaBossHold (S_FQAddress Main)
{
	RouteBossHold=1;
	return SUCCESS;
}

int InitComViaBossDirect (S_FQAddress Main)
{
	RouteBossDirect=1;
	return SUCCESS;
}

int InitComIgnoreBSY (S_FQAddress Main)
{
	IgnoreBSY=1;
	return SUCCESS;
}

int InitComKillInTransit (S_FQAddress Main)
{
	KillInTransit=1;
	return SUCCESS;
}

int InitComLogpath (S_FQAddress Main)
{
	PathLogStyle=1;
	return SUCCESS;
}

int InitComFastechoPack (S_FQAddress Main)
{
    FastechoPack=1;
    return SUCCESS;
}

int ProcParFastechoPack (char *Token, S_FQAddress Main)
{
	S_Codes Info;
	GetTokenInfo (Token,&Info);
	switch (Info.Group)
	{
		case PT_QQQMODE:
                        FastechoPack = 2;
                        return SUCCESS;
                        break;
		default:
			printf ("%s can't be used as parameter to "
                                "FASTECHOPACK ",Token);
			return WARNING;
	}
	return SUCCESS;
}



int InitComNoDomainDir (S_FQAddress Main)
{
	NoDomainDir=1;
	return SUCCESS;
}

int InitComIgnoreUnknownAttribs (S_FQAddress Main)
{
    IgnoreUnknownAttribs=1;
    return SUCCESS;
}

int InitComFEConfig (S_FQAddress Main)
{
	ReadFEConfig=1;
	FEConfDir[0]=0;
	return SUCCESS;
}

#ifndef OS_2
#pragma warn +par
#endif
