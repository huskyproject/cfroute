typedef unsigned long dword;

#define MODE_SETPASSIVE 1
#define MODE_KEEPLAST   2
#define RET_OK		    0
#define RET_SETPASSIVE  1

#ifndef __BORLANDC__
struct date
{
  int da_year;
  char da_day;
  char da_mon;
};
#endif

struct std_type
{
  unsigned day: 5;
  unsigned mon: 4;
  unsigned year: 7;
};

union dfd
{
        struct std_type std;
	unsigned short number;
};

#include "fecfg145.h"

CONFIG FEConfig;

int FastEchoConfig (void)
{
	SysAddress OurAKAs;
	S_FQAddress AddrNat;
	S_WAddress WildAddr;
	FILE *readin;
	char *feenv,configpath[80];
	ExtensionHeader EH;
	Node NewNode;
	dword offset;
	word count;
	int control;
	if (FEConfDir[0]==0)
	{
		feenv=getenv ("FE");
		if (feenv)
			strcpy (configpath,feenv);
		else
			strcpy (configpath,"."DIRSEPS);
	}
	else
		strcpy (configpath,FEConfDir);
	if (configpath[strlen (configpath)-1]!=DIRSEPC)
		strcat (configpath,DIRSEPS);
	strcat (configpath,"FASTECHO.CFG");
        adaptcase(configpath);
	readin=fopen (configpath,"rb");
	if (readin==NULL)
		return ECH_OPENFAILED;
	fread (&FEConfig,1,sizeof (FEConfig),readin);
	if (!OutboundDirSetup)
	{
		strcpy (OutboundDirectory,FEConfig.OutBound);
		OutboundDirSetup=1;
#ifndef DEBUG
	if (RequestInfo)
#endif
		printf ("From FE: Outbound: %s\n",FEConfig.OutBound);
	}
	if (SL_NetmailDir.GetStringCount()==0)
	{
		SL_NetmailDir.AddString (FEConfig.NetMPath);
#ifndef DEBUG
	if (RequestInfo)
#endif
		printf ("From FE: Netmail: %s\n",FEConfig.NetMPath);
	}
	offset=0;
	while (offset<FEConfig.offset)
	{
		fread (&EH,sizeof (ExtensionHeader),1,readin);
		switch (EH.type)
		{
			case EH_AKAS:
				for (count=0;count<FEConfig.AkaCnt;count++)
				{
					fread (&OurAKAs,1,sizeof (SysAddress),readin);
					if (OurAKAs.main.zone)
					{
						AddrNat.Zone=OurAKAs.main.zone;
						AddrNat.Net=OurAKAs.main.net;
						AddrNat.Node=OurAKAs.main.node;
						AddrNat.Point=OurAKAs.main.point;
						strcpy (AddrNat.Domain,OurAKAs.domain);
						if (AddressHandler.MainDefined ()==0)
							control=AddressHandler.StoreMain (AddrNat);
						else
							control=AddressHandler.StoreAKA (AddrNat);
						if (control==SUCCESS)
						{
#ifndef DEBUG
	if (RequestInfo)
#endif
								printf ("From FE: AKA: %u:%u/%u.%u@%s\n",OurAKAs.main.zone,
								OurAKAs.main.net,OurAKAs.main.node,
								OurAKAs.main.point,OurAKAs.domain);
						}
						else
							printf ("Failed to add AKA.\n");
					}
				}
				break;
			default:
				fseek (readin,EH.offset,SEEK_CUR);
		}
		offset=offset+EH.offset+sizeof (EH);
	}
	if (FEConfig.offset!=offset)
	{
		printf ("Configuration probably processed incorrectly, terminating (errorlevel 3)\n");
		exit (3);
	}
	count=0;
	while (count<FEConfig.NodeCnt)
	{
		fread (&NewNode,1,sizeof (Node),readin);
		count++;
		sprintf (WildAddr.Zone,"%hu",NewNode.addr.zone);
		sprintf (WildAddr.Net,"%hu",NewNode.addr.net);
		sprintf (WildAddr.Node,"%hu",NewNode.addr.node);
		sprintf (WildAddr.Point,"%hu",NewNode.addr.point);
		AddrNat.Domain[0]=0;
		PasswordHandler.AddItem (WildAddr,NewNode.password);
#ifndef DEBUG
	if (RequestInfo)
#endif
			printf ("From FE: Password for %s:%s/%s.%s: %s\n",
			WildAddr.Zone,WildAddr.Net,WildAddr.Node,WildAddr.Point,
			NewNode.password);
       fseek (readin,FEConfig.NodeRecSize-sizeof (Node),SEEK_CUR);
	}
	fclose (readin);
	return 0;
}
