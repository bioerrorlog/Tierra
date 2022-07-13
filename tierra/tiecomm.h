#ifdef NET

/**********************************************************************
 * tiecomm.h - Tierra communications API header
 **********************************************************************/

/**********************************************************************
 * #defines
 **********************************************************************/

#define UDP      0
#define UDPASM   1
#define TCP      2

#define FLUSHBUFSIZE 1024

#if NETTYPE == TCP
#define TIESOCKTYPE    SOCK_STREAM
#define INETPROTOCOL   "tcp"
#endif /* NETTYPE == TCP */

#if NETTYPE == UDP || NETTYPE == UDPASM
#define TIESOCKTYPE    SOCK_DGRAM
#define INETPROTOCOL   "udp"
#endif /* NETTYPE == UDP || NETTYPE == UDPASM */

#if NETTYPE == UDP || NETTYPE == TCP || NETTYPE == UDPASM
typedef int TieCommHndl;
#endif /* NETTYPE == UDP || NETTYPE == TCP || NETTYPE == UDPASM */

/**********************************************************************
 * network structure headers
 **********************************************************************/

#if NETTYPE == UDPASM

/* packet node */
struct datpktnod
{   ListNode pktn_listnode;
    I32u     pktn_offset;
    I32u     pktn_len;
};

#endif /* NETTYPE == UDPASM */

/**********************************************************************
 * Tierra communications API structure definitions
 **********************************************************************/

/* message node */
struct datmesgnod
{   ListNode  msgn_listnode;

#if NETTYPE == UDPASM
    I32u      msgn_msgid;
    I32u      msgn_size;
    ListHead  msgn_listhead;
    I32u      msgn_rnumpack;
#endif /* NETTYPE == UDPASM */

    I32u      msgn_rnumbyte;
    I32u      msgn_len;
    I32u      msgn_msgstrlen;
    I32s      msgn_timercvd;
    I32s      msgn_encoded;
    I8u      *msgn_decbuf;
    I8u      *msgn_datmesgbuf;
    NetAddr   msgn_src;
};

struct TieCommID
{   I32u tiec_node;
    I16u tiec_port;
};

typedef struct TieCommInfo TieCommInfo;
struct TieCommInfo
{   I32u               tci_comopen;
    TieCommHndl        tci_comrdhndl;
    struct TieCommID   tci_srctiecommid;
    I32u               tci_maxmsgage;
    ListHead           tci_msgbklog;
    I32u               tci_queuesize;
    I32u               tci_queuelen;
    I32s               tci_proto;

#if NETTYPE == UDPASM || NETTYPE == UDP || NETTYPE == TCP
    short              tci_family;
#endif /* NETTYPE == UDPASM || NETTYPE == UDP || NETTYPE == TCP */

#if NETTYPE == UDPASM || NETTYPE == UDP
    I32u               tci_pktmtu;
#endif /* NETTYPE == UDPASM || NETTYPE == UDP */

#if NETTYPE == TCP
    struct timeval     tci_tcplocseltimeout; /* tcp local select timeout */
    struct timeval     tci_tcpremseltimeout; /* tcp remote select timeout */
#endif /* NETTYPE == TCP */
};

/**********************************************************************
 * Tierra communications API function prototypes
 **********************************************************************/

void TieCommInit P_((TieCommInfo *cominfoptr));

int TieCommSend P_(( TieCommInfo *cominfoptr,
    TMessage *msgptr, struct TieCommID dsttiecommid,
    I32s *sufrembndcap
));

I32s TieCommOpen P_((
    TieCommInfo *cominfoptr,
    I32u hostport, I32u maxqueuesize,
    I32u pktmtu, I32u maxmsgage,
    I32u tcplocseltmesec, I32u tcplocseltmeusec,
    I32u tcpremseltmesec, I32u tcpremseltmeusec,
    I8s *protocol
));

int TieCommProcMsgBkLog P_((
    TieCommInfo *cominfobuf,
    int (*procmsg)(TMessage *,NetAddr *, I32u)
));

void TieCommClose P_((TieCommInfo *cominfobuf));

void TieCommOnOffLine P_((I32s onlinereq, I32s startup));

void TieCommChgSubNet P_((
    I32s oldsubnet, I32s startup, I32s newsoup, I32s goalter));

void SetLocalNodeSubNet P_((NetAddr *address, I32s newsubnet));

void TieRcvIncMsg P_((void));

/**********************************************************************
 * global general communications function prototypes
 **********************************************************************/

void DelMsgTree P_((TieCommInfo *cominfobuf, ListHead *msgtrert));

void DelMsgNode P_((TieCommInfo *cominfobuf, struct datmesgnod *msgnodptr));

I32s opnwrtskt P_((TieCommInfo *cominfobuf,
    struct sockaddr_in *sa, I8u *nodename, I32u portno, int *sock
));

I32s rdhndlflsh P_((int rdhndl));

/**********************************************************************
 * UDPASM communications function prototypes
 **********************************************************************/

#if NETTYPE == UDPASM

I32s TieCommSendUDPASM P_(( TieCommInfo *cominfoptr, TMessage *msgptr,
    I8u *xdrmsgbuf, I32u msglen, struct TieCommID dsttiecommid,
    I32s *sufrembndcap));

extern void TieRcvIncMsgUDPASM P_((void));

void DelPktTree P_((TieCommInfo *cominfobuf, ListHead *msgtrert));

#endif /* NETTYPE == UDPASM */

/**********************************************************************
 * TCP communications function prototypes
 **********************************************************************/

#if NETTYPE == TCP

extern I32s TieCommSendTCP P_((TieCommInfo *cominfoptr, TMessage *msgptr,
    I8u *xdrmsgbuf, I32u msglen, struct TieCommID dsttiecommid,
    I32s *sufrembndcap));

extern int TieRcvIncMsgTCP P_((void));

#endif /* NETTYPE == TCP */
#endif /* NET */
