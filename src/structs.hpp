#ifndef __STRUCTS__H
#define __STRUCTS__H

#include <stdlib.h>             // size_t

                                // Class for reading and writing structures
                                // that reside on disk in a way that is more
                                // portable than just doing
                                // fread(..,sizeof(sturcture),..).

#include "buffer.hpp"

class DiskRecord
{
public:
    
    DiskRecord() {} ; 
    virtual ~DiskRecord() {} ;
    
    virtual size_t get_filesize(void) = 0;
    virtual void   Import(const unsigned char *buffer) = 0;
    virtual void   Export(unsigned char *buffer) = 0;
    
    int    read(FILE *f);
    int    read(C_FileRead &);
    int    write(FILE *f);
};

class S_MSG: public DiskRecord
{
public:
    
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
    unsigned short Attribute;
    unsigned short NextReply;
    
                                // member functions for endian-safe and
                                // architecture-independent reading and writing
            
    size_t get_filesize(void) { return 190; }
    void   Import(const unsigned char *buffer);
    void   Export(unsigned char *buffer);
};

#define S_Private(x)              (((x) & 1) ? 1 : 0)
#define S_Crash(x)                (((x) & 2) ? 1 : 0)
#define S_Recd(x)                 (((x) & 4) ? 1 : 0)
#define S_Sent(x)                 (((x) & 8) ? 1 : 0)
#define S_FileAttached(x)         (((x) & 16) ? 1 : 0)
#define S_InTransit(x)            (((x) & 32) ? 1 : 0)
#define S_Orphan(x)               (((x) & 64) ? 1 : 0)
#define S_KillSent(x)             (((x) & 128) ? 1 : 0)
#define S_Local(x)                (((x) & 256) ? 1 : 0)
#define S_Hold(x)                 (((x) & 512) ? 1 : 0)
#define S_Unused(x)               (((x) & 1024) ? 1 : 0)
#define S_FileRequest(x)          (((x) & 2048) ? 1 : 0)
#define S_ReturnReceiptRequest(x) (((x) & 4096) ? 1 : 0)
#define S_IsReturnReceipt(x)      (((x) & 8192) ? 1 : 0)
#define S_AuditRequest(x)         (((x) & 16384) ? 1 : 0)
#define S_FileUpdateReq(x)        (((x) & 32768U) ? 1 : 0)

#define Set_Private(x,y)              {if (y) (x)|=     1; else (x) &= ~1;    }
#define Set_Crash(x,y)                {if (y) (x)|=     2; else (x) &= ~2;    }
#define Set_Recd(x,y)                 {if (y) (x)|=     4; else (x) &= ~4;    }
#define Set_Sent(x,y)                 {if (y) (x)|=     8; else (x) &= ~8;    }
#define Set_FileAttached(x,y)         {if (y) (x)|=    16; else (x) &= ~16;   }
#define Set_InTransit(x,y)            {if (y) (x)|=    32; else (x) &= ~32;   }
#define Set_Orphan(x,y)               {if (y) (x)|=    64; else (x) &= ~64;   }
#define Set_KillSent(x,y)             {if (y) (x)|=   128; else (x) &= ~128;  }
#define Set_Local(x,y)                {if (y) (x)|=   256; else (x) &= ~256;  }
#define Set_Hold(x,y)                 {if (y) (x)|=   512; else (x) &= ~512;  }
#define Set_Unused(x,y)               {if (y) (x)|=  1024; else (x) &= ~1024; }
#define Set_FileRequest(x,y)          {if (y) (x)|=  2048; else (x) &= ~2048; }
#define Set_ReturnReceiptRequest(x,y) {if (y) (x)|=  4096; else (x) &= ~4096; }
#define Set_IsReturnReceipt(x,y)      {if (y) (x)|=  8192; else (x) &= ~8192; }
#define Set_AuditRequest(x,y)         {if (y) (x)|= 16384; else (x) &= ~16384;}
#define Set_FileUpdateReq(x,y)        {if (y) (x)|= 32768; else (x) &= ~32768;}

#define S_ArchiveSent(x)          (((x) & 1) ? 1 : 0)
#define S_KillFileSent(x)         (((x) & 2) ? 1 : 0)
#define S_Direct(x)               (((x) & 4) ? 1 : 0)
#define S_Zonegate(x)             (((x) & 8) ? 1 : 0)
#define S_Hub(x)                  (((x) & 16) ? 1 : 0)
#define S_Immediate(x)            (((x) & 32) ? 1 : 0)
#define S_XMA(x)                  (((x) & 64) ? 1 : 0)
#define S_Lock(x)                 (((x) & 128) ? 1 : 0)
#define S_Truncate(x)             (((x) & 256) ? 1 : 0)
#define S_HiRes(x)                (((x) & 512) ? 1 : 0)
#define S_CoverLetter(x)          (((x) & 1024) ? 1 : 0)
#define S_Signature(x)            (((x) & 2048) ? 1 : 0)
#define S_LetterHead(x)           (((x) & 4096) ? 1 : 0)
#define S_Fax(x)                  (((x) & 8192) ? 1 : 0)
#define S_ForcePickup(x)          (((x) & 16384) ? 1 : 0)
#define S_Unused2(x)              (((x) & 32768U) ? 1 : 0)

#define Set_ArchiveSent(x,y)          {if (y) (x)|=     1; else (x) &= ~1;    }
#define Set_KillFileSent(x,y)         {if (y) (x)|=     2; else (x) &= ~2;    }
#define Set_Direct(x,y)               {if (y) (x)|=     4; else (x) &= ~4;    }
#define Set_Zonegate(x,y)             {if (y) (x)|=     8; else (x) &= ~8;    }
#define Set_Hub(x,y)                  {if (y) (x)|=    16; else (x) &= ~16;   }
#define Set_Immediate(x,y)            {if (y) (x)|=    32; else (x) &= ~32;   }
#define Set_XMA(x,y)                  {if (y) (x)|=    64; else (x) &= ~64;   }
#define Set_Lock(x,y)                 {if (y) (x)|=   128; else (x) &= ~128;  }
#define Set_Truncate(x,y)             {if (y) (x)|=   256; else (x) &= ~256;  }
#define Set_HiRes(x,y)                {if (y) (x)|=   512; else (x) &= ~512;  }
#define Set_CoverLetter(x,y)          {if (y) (x)|=  1024; else (x) &= ~1024; }
#define Set_Signature(x,y)            {if (y) (x)|=  2048; else (x) &= ~2048; }
#define Set_LetterHead(x,y)           {if (y) (x)|=  4096; else (x) &= ~4096; }
#define Set_Fax(x,y)                  {if (y) (x)|=  8192; else (x) &= ~8192; }
#define Set_ForcePickup(x,y)          {if (y) (x)|= 16384; else (x) &= ~16384;}
#define Set_Unused2(x,y)              {if (y) (x)|= 32768; else (x) &= ~32768;}

class S_PKT: public DiskRecord
{
public:
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

                                // member functions for endian-safe and
                                // architecture-independent reading and writing
            
    size_t get_filesize(void) { return 60 ; }
    void   Import(const unsigned char *buffer);
    void   Export(unsigned char *buffer);
};


class S_Packed: public DiskRecord
{
public:
    unsigned short Signature;
    unsigned short OrigNode;
    unsigned short DestNode;
    unsigned short OrigNet;
    unsigned short DestNet;
    unsigned short Attrib;
    unsigned short Cost;
    char DateTime[20];
                                // member functions for endian-safe and
                                // architecture-independent reading and writing
            
    size_t get_filesize(void) { return 34 ; }
    void   Import(const unsigned char *buffer);
    void   Export(unsigned char *buffer);

};



class DateTime: public DiskRecord
{
public:
    unsigned char hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned char hundredths;
    unsigned char day;
    unsigned char month;
    unsigned short year;
    short 	 timezone;
    unsigned char weekday;

                                // arch-dependent function to fill the struct

    void getCurrentTime(void);

                                // member functions for endian-safe and
                                // architecture-independent reading and writing
            
    size_t get_filesize(void) { return 11 ; }
    void   Import(const unsigned char *buffer);
    void   Export(unsigned char *buffer);

};
#endif
