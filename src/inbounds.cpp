// Small class for storing the various inbound directories

#include "inbounds.hpp"
#include "errors.hpp"

#include <stdlib.h>
#include <string.h>

int CInbounds::GetInboundCount (void)
{
    return currentcount;
}

size_t CInbounds::GetLongestString(void)
{
    return longest;
}

char *CInbounds::GetInbound (int number)
{
    if (number>=currentcount)
        return NULL;

    return inboundArray[number];
}

CInbounds::CInbounds (void)
{
    currentcount=0;
    inboundArray=NULL;
    longest=0;
}

CInbounds::~CInbounds (void)
{
    int i;

    for (i = 0; i < currentcount; i++)
    {
        free (inboundArray[i]);
    }
    if (currentcount)
    {
        free (inboundArray);
        currentcount = 0;
    }
}
    

int CInbounds::AddItem (char *inbound)
{
    size_t l = strlen(inbound);
    
    if (!currentcount)
    {
        inboundArray = (char **)malloc(sizeof(char *));
        currentcount = 1;
    }
    else
    {
        currentcount++;
        inboundArray = (char **)realloc(inboundArray,currentcount * sizeof(char *));
    }

    if (inboundArray == NULL)
        return NOMEMORY;

    inboundArray[currentcount - 1] = (char *)malloc(l + 1);

    if (inboundArray[currentcount - 1] == NULL)
    {
        currentcount--;
        return NOMEMORY;
    }

    memcpy(inboundArray[currentcount - 1], inbound, l + 1);

    if (l > longest)
        longest = l;
    
    return SUCCESS;
}

