#ifdef RPC_HDR
%#ifndef MESG_H
%#define MESG_H
%#include "portable.h"
%#include "tiexdrcom.h"
#endif /* RPC_HDR */

#ifdef RPC_XDR
%#include "port_xdr.h"
#endif /* RPC_XDR */

enum TagsType
{   NULLTAG = 0,
    TPINGREPTAG = 1,
    TPINGREQTAG = 2,
    SENDADDRTAG = 3,
    EMIGRATETAG = 4,
    BYETAG = 5
};

struct NetAddr  /* internet address */
{   N32u node;   /* internet node address */
    N16u portnb; /* socket port number, identifies process within a node */
};

struct BglTrackFlag  /* flag for migration tracking */
{   N32u node;    /* internet node address */
    N16u portnb;  /* socket port number, identifies process within a node */
    I32s session; /* unique identifier for each tracking session */
    I8s  param[8];/* parameter for tracking one genotype or one size */
};

struct ComAddr /* structure for communication addresses */
{   struct NetAddr address; /* internet address */
    I32s ranID;   /* random ID # of cell */
};

struct TPingData /* data structure for tping message */
{   I32s FecundityAvg; /* average fecundity at death, over last million */
    I32s Speed;  /* average instructions/second during last million */
    I32s NumCells;
    I32s AgeAvg; /* average inst age at death, over last million */
    I32s SoupSize;
    I32u TransitTime; /* in milliseconds */
    I32u Fresh;  /* local clock time at last refresh of this data struct */
    I32u Time;        /* clock time at node */
    I32s InstExec;
    I32s InstExecConnect;
    I32s OS; /* operating system tag */
};

struct TPrivPingData
{   I32s subnet;       /* cluster subnet */
    I32s apoctoggle;   /* cluster apocalypse toggle */
    I32s outcapupdtog; /* remaining outgoing capacity update toggle */
    I32s remoutbndcap; /* remaining outgoing capacity */
    I32s avgsize;      /* average size */
};

struct AddrBody
{   I8s name < 80 >;
};

struct EmigrateBody
{   struct NetAddr dest;
    struct BglTrackFlag BglTrack;
    Mem  mg;
    I32s ranID;
#ifdef EXEINSTSIZTIMSLC
    I32s PExeSegSiz;
#endif /* EXEINSTSIZTIMSLC */
    I32u genome_siz;
    xdrwrd genome <>;
};

struct TPingReqBody
{   struct ComAddr source;
};

struct TPingRepBody
{   struct ComAddr dest;
    struct ComAddr source;
};


union Data switch (TagsType tag)
{   case NULLTAG:
        void;
    case SENDADDRTAG:
        struct AddrBody Addr;
    case EMIGRATETAG:
        struct EmigrateBody Creature;
    case TPINGREQTAG:
        struct TPingReqBody TPingReq;
    case TPINGREPTAG:
        struct TPingRepBody TPingRep;
    default:
        void;
};


struct TMessage
{   Data info;
    I32s  dstmap;
    I32s  srcmap;
    struct TPingData PingData;
    struct TPrivPingData PrivPingData;
};

#ifdef RPC_HDR
%#endif /* MESG_H */
#endif /* RPC_HDR */
