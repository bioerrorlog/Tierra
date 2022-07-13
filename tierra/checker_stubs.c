#if defined MEM_CHECKER

#include "tierra.h"

int chkr_stub_getdomainname(name, len)
char *name;
size_t len;
{   int rtnval,i;
    if(len>0)
    {   stubs_chkr_check_addr(name, len, CHKR_MW, "name");
        chkr_set_right(name, len, CHKR_RW);
    }
    if(!(rtnval=getdomainname(name, len)))
    {   for(i=0;i<len;i++)
            if(!(name[i]))
                break;
        chkr_set_right(name, len, CHKR_WO);
        if(i<(len-1))
            chkr_set_right(name, i+1, CHKR_RW);
    }
    return rtnval;
}
#endif /* MEM_CHECKER */
