#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "structs.hpp"

/*
 *  get_unsigned long
 *
 *  Reads in a 4 unsigned char word that is stored in little endian (Intel)
 *  notation and converts it to the local representation n an architecture-
 *  independent manner
 */

#define get_dword(ptr) \
     ((unsigned)((unsigned char)(ptr)[0]) |         \
    (((unsigned)((unsigned char)(ptr)[1])) << 8)  | \
    (((unsigned)((unsigned char)(ptr)[2])) << 16) | \
    (((unsigned)((unsigned char)(ptr)[3])) << 24))  

/*
 *  get_word
 *
 *  Reads in a 2 unsigned char word that is stored in little endian
 *  (Intel) notation and converts it to the local representation in an
 *  architecture-independent manner
 */

#define get_word(ptr)         \
    ((unsigned short)((unsigned char)(ptr)[0]) |         \
     (((unsigned short)((unsigned char)(ptr)[1])) << 8 ))

/*
 *  put_dword
 *
 *  Writes a 4 unsigned char word in little endian notation, independent
 *  of the local system architecture.
 */
#if 0                           // we don't use this function  - suppress
                                // warnings 

static void put_dword(unsigned char *ptr, unsigned long value)
{
    ptr[0] = (value & 0xFF);
    ptr[1] = (value >> 8) & 0xFF;
    ptr[2] = (value >> 16) & 0xFF;
    ptr[3] = (value >> 24) & 0xFF;
}

#endif

/*
 *  put_word
 *
 *  Writes a 4 unsigned char word in little endian notation, independent of the local
 *  system architecture.
 */

static void put_word(unsigned char *ptr, unsigned value)
{
    ptr[0] = (value & 0xFF);
    ptr[1] = (value >> 8) & 0xFF;
}

int DiskRecord::read(C_FileRead &fh)
{
    unsigned char staticbuf[1024];
    unsigned char *buffer;
    size_t structsize;
    int rv = 0;
    
    structsize = get_filesize();
    if (structsize > sizeof(staticbuf)) {
        buffer = new unsigned char[structsize];
    } else {
        buffer = staticbuf;
    }
    
    if (fh.ReadBytes(buffer, structsize) == structsize) {
        Import(buffer);
        rv = 1;
    }
    
    if (structsize > sizeof(staticbuf)) {
        delete[] buffer;
    }
    return rv;
}


int DiskRecord::read(FILE *f)
{
    unsigned char staticbuf[1024];
    unsigned char *buffer;
    size_t structsize;
    int rv = 0;
    
    structsize = get_filesize();
    if (structsize > sizeof(staticbuf)) {
        buffer = new unsigned char[structsize];
    } else {
        buffer = staticbuf;
    }
    
    if (fread(buffer, structsize, 1, f) == 1) {
        Import(buffer);
        rv = 1;
    }
    
    if (structsize > sizeof(staticbuf)) {
        delete[] buffer;
    }
    return rv;
}

int DiskRecord::write(FILE *f)
{
    unsigned char staticbuf[1024];
    unsigned char *buffer;
    size_t structsize;
    int rv = 0;

    structsize = get_filesize();
    if (structsize > sizeof(staticbuf)) {
        buffer = new unsigned char[structsize];
    } else {
        buffer = staticbuf;
    }
    
    Export(buffer);
    
    if (fwrite(buffer, structsize, 1, f) == 1) {
        rv = 1;
    }
    
    if (structsize > sizeof(staticbuf)) {
        delete[] buffer;
    }
    return rv;
}

void S_MSG::Import(const unsigned char *buffer)
{
    const unsigned char *pbuf = buffer;
    
    memmove(FromUserName, pbuf, 36);  pbuf += 36;
    memmove(ToUserName, pbuf, 36);    pbuf += 36;
    memmove(Subject, pbuf, 72);       pbuf += 72;
    memmove(DateTime, pbuf, 20);      pbuf += 20;
    TimesRead = get_word(pbuf);       pbuf += 2;
    DestNode = get_word(pbuf);        pbuf += 2;
    OrigNode = get_word(pbuf);        pbuf += 2;
    Cost = get_word(pbuf);            pbuf += 2;
    OrigNet = get_word(pbuf);         pbuf += 2;
    DestNet = get_word(pbuf);         pbuf += 2;

                                // date_written && date_arrived not used by
                                // cfroute 

    memmove(Fill, pbuf, 8);           pbuf += 8;

    ReplyTo = get_word(pbuf);         pbuf += 2;
    Attribute = get_word(pbuf);       pbuf += 2;
    NextReply = get_word(pbuf);       pbuf += 2;

    assert((size_t)(pbuf - buffer) == get_filesize());
}

void S_MSG::Export(unsigned char *buffer)
{
    unsigned char *pbuf = buffer;
    
    memmove(pbuf, FromUserName, 36); pbuf += 36;
    memmove(pbuf, ToUserName, 36);   pbuf += 36;
    memmove(pbuf, Subject, 72);      pbuf += 72;
    memmove(pbuf, DateTime, 20);     pbuf += 20;

    put_word(pbuf, TimesRead);       pbuf += 2;
    put_word(pbuf, DestNode);        pbuf += 2;

    put_word(pbuf, OrigNode);        pbuf += 2;
    put_word(pbuf, Cost);            pbuf += 2;
    put_word(pbuf, OrigNet);         pbuf += 2;
    put_word(pbuf, DestNet);         pbuf += 2;

    memmove(pbuf, Fill, 8);          pbuf += 8;

    put_word(pbuf, ReplyTo);         pbuf += 2;
    put_word(pbuf, Attribute);       pbuf += 2;

    put_word(pbuf, NextReply);       pbuf += 2;

    assert((size_t)(pbuf - buffer) == get_filesize());
}

void S_PKT::Import(const unsigned char *buffer)
{
    const unsigned char *pbuf = buffer;

    OrigNode = get_word(pbuf); pbuf+=2;
    DestNode = get_word(pbuf); pbuf+=2;
    Year     = get_word(pbuf); pbuf+=2;
    Month    = get_word(pbuf); pbuf+=2;
    Day      = get_word(pbuf); pbuf+=2;
    Hour     = get_word(pbuf); pbuf+=2;
    Minute   = get_word(pbuf); pbuf+=2;
    Second   = get_word(pbuf); pbuf+=2;
    Baud     = get_word(pbuf); pbuf+=2;
    Signature = get_word(pbuf); pbuf+=2;
    OrigNet  = get_word(pbuf); pbuf+=2;
    DestNet  = get_word(pbuf); pbuf+=2;

    ProdCode = (char)pbuf[0]; pbuf++;
    SerialNo = (char)pbuf[0]; pbuf++;
    memcpy(Password, pbuf, 8); pbuf+=8;

    OrigZone = get_word(pbuf); pbuf+=2;
    DestZone = get_word(pbuf); pbuf+=2;

    memcpy(dif.t2plus.Fill, pbuf, 2); pbuf+=2;
    dif.t2plus.CapabilitySwapped = get_word(pbuf); pbuf+=2;
    dif.t2plus.PrdCodH = (char)pbuf[0]; pbuf++;
    dif.t2plus.PVMinor = (char)pbuf[0]; pbuf++;
    dif.t2plus.Capability = get_word(pbuf); pbuf+=2;
    dif.t2plus.OrigZone   = get_word(pbuf); pbuf+=2;
    dif.t2plus.DestZone   = get_word(pbuf); pbuf+=2;
    dif.t2plus.OrigPoint  = get_word(pbuf); pbuf+=2;
    dif.t2plus.DestPoint  = get_word(pbuf); pbuf+=2;
    memcpy(dif.t2plus.ProdData, pbuf, 4); pbuf+=4;
    memcpy(EndOfPKT, pbuf, 2); pbuf+=2;
    
    assert((size_t)(pbuf - buffer) == get_filesize());
}


void S_PKT::Export(unsigned char *buffer)
{
    unsigned char *pbuf = buffer;

    put_word(pbuf, OrigNode); pbuf+=2;
    put_word(pbuf, DestNode); pbuf+=2;
    put_word(pbuf, Year    ); pbuf+=2;
    put_word(pbuf, Month   ); pbuf+=2;
    put_word(pbuf, Day     ); pbuf+=2;
    put_word(pbuf, Hour    ); pbuf+=2;
    put_word(pbuf, Minute  ); pbuf+=2;
    put_word(pbuf, Second  ); pbuf+=2;
    put_word(pbuf, Baud    ); pbuf+=2;
    put_word(pbuf, Signature); pbuf+=2;
    put_word(pbuf, OrigNet ); pbuf+=2;
    put_word(pbuf, DestNet ); pbuf+=2;

    pbuf[0] = (unsigned char)ProdCode; pbuf++;
    pbuf[0] = (unsigned char)SerialNo; pbuf++;
    memcpy(pbuf, Password, 8); pbuf+=8;

    put_word(pbuf, OrigZone); pbuf+=2;
    put_word(pbuf, DestZone); pbuf+=2;

    memcpy(pbuf, dif.t2plus.Fill, 2); pbuf+=2;
    put_word(pbuf, dif.t2plus.CapabilitySwapped); pbuf+=2;

    pbuf[0] = (unsigned char)dif.t2plus.PrdCodH; pbuf++;
    pbuf[0] = (unsigned char)dif.t2plus.PVMinor; pbuf++;

    put_word(pbuf, dif.t2plus.Capability); pbuf+=2;
    put_word(pbuf, dif.t2plus.OrigZone);   pbuf+=2;
    put_word(pbuf, dif.t2plus.DestZone);   pbuf+=2;
    put_word(pbuf, dif.t2plus.OrigPoint);  pbuf+=2;
    put_word(pbuf, dif.t2plus.DestPoint);  pbuf+=2;

    memcpy(pbuf, dif.t2plus.ProdData, 4); pbuf+=4;
    memcpy(pbuf, EndOfPKT, 2); pbuf+=2;
    
    assert((size_t)(pbuf - buffer) == get_filesize());
}


void S_Packed::Export(unsigned char *buffer)
{
    unsigned char *pbuf = buffer;

    put_word(pbuf, Signature); pbuf+=2;
    put_word(pbuf, OrigNode); pbuf+=2;
    put_word(pbuf, DestNode); pbuf+=2;
    put_word(pbuf, OrigNet); pbuf+=2;
    put_word(pbuf, DestNet); pbuf+=2;
    put_word(pbuf, Attrib); pbuf+=2;
    put_word(pbuf, Cost); pbuf+=2;

    memcpy(pbuf, DateTime, 20); pbuf+=20;

    assert((size_t)(pbuf - buffer) == get_filesize());
}

void S_Packed::Import(const unsigned char *buffer)
{
    const unsigned char *pbuf = buffer;

    Signature=get_word(pbuf); pbuf+=2;
    OrigNode=get_word(pbuf); pbuf+=2;
    DestNode=get_word(pbuf); pbuf+=2;
    OrigNet=get_word(pbuf); pbuf+=2;
    DestNet=get_word(pbuf); pbuf+=2;
    Attrib=get_word(pbuf); pbuf+=2;
    Cost=get_word(pbuf); pbuf+=2;

    memcpy(DateTime, pbuf, 20); pbuf+=20;

    assert((size_t)(pbuf - buffer) == get_filesize());
}


void DateTime::Import(const unsigned char *buffer)
{
    const unsigned char *pbuf = buffer;

    hours = pbuf[0]; pbuf++;
    minutes = pbuf[0]; pbuf++;
    seconds = pbuf[0]; pbuf++;
    hundredths = pbuf[0]; pbuf++;
    day = pbuf[0]; pbuf++;
    month = pbuf[0]; pbuf++;
    year = get_word(pbuf); pbuf+=2;
    timezone = get_word(pbuf); pbuf+=2;
    weekday = pbuf[0]; pbuf++;

    assert((size_t)(pbuf - buffer) == get_filesize());
}

void DateTime::Export(unsigned char *buffer)
{
    unsigned char *pbuf = buffer;

    pbuf[0] = hours;
    pbuf[1] = minutes;
    pbuf[2] = seconds;
    pbuf[3] = hundredths;
    pbuf[4] = day;
    pbuf[5] = month;
    pbuf +=6;

    put_word(pbuf, year); pbuf+=2;
    put_word(pbuf, timezone); pbuf+=2;
    pbuf[0] = weekday; pbuf++;

    assert((size_t)(pbuf - buffer) == get_filesize());
}
