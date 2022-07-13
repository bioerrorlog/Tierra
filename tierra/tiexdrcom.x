#ifdef RPC_HDR
%#ifndef TIEXDRCOM_H
%#define TIEXDRCOM_H
%#include "portable.h"
#endif /* RPC_HDR */

#ifdef RPC_XDR
%#include "port_xdr.h"
#endif /* RPC_XDR */

struct DynArrCtrl
{   I32s           dync_elsize;   /* element size      */
    I32s           dync_elmused;  /* elements used     */
    I32s           dync_elmaloc;  /* allocated size    */
    I32s           dync_indir;    /* indirect flag     */
};

struct Mem /* structure for allocated memory block of cell */
{   I32s p; /* location of start of cell memory */
    I32s s; /* size of cell memory */
};

struct Event /* structure for time measured in executed instructions */
{   I32s m;  /* count of millions of instructions */
    I32s i;  /* count of instructions */
};

#ifdef RPC_HDR
%#endif /* TIEXDRCOM_H */
#endif /* RPC_HDR */
