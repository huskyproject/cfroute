#ifndef __INBOUNDS_HPP
#define __INBOUNDS_HPP

#include <stdlib.h>

class CInbounds
{
    char **inboundArray;
    int currentcount;
    size_t longest;

public:
    int AddItem (char *inbound);
    CInbounds (void);
    ~CInbounds (void);
    int GetInboundCount (void);
    size_t GetLongestString(void);
    char *GetInbound (int n);
};

#endif
