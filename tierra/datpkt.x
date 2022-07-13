#ifdef RPC_HDR
%#ifndef DATPKT_H
%#define DATPKT_H
%#include "portable.h"
#endif /* RPC_HDR */

#ifdef RPC_XDR
%#include "port_xdr.h"
#endif /* RPC_XDR */

struct pkthdr
{   I32u     pkth_msgid;
    I32u     pkth_msglen;
    I32u     pkth_offset;
    I32u     pkth_len;
};
struct datpkt
{   struct pkthdr     dpkt_hdr;
    xdrwrd            dpkt_data <>;
};

#ifdef RPC_HDR
%#endif /* DATPKT_H */
#endif /* RPC_HDR */
