#include "tierra.h"

#ifdef NET
#if NETTYPE == UDPASM

#include "globals.h"

/**********************************************************************
 * static prototypes
 **********************************************************************/

static struct datpktnod *srchpkttree P_((
    ListHead *pkttrert, 
    I32u pktoffset
));

static struct datmesgnod *srchmsgtree P_((
    ListHead *msgtrert, I32u msgid
));

/**********************************************************************
 * TieCommSendUDPASM - send message
 * output
 *    errflag - 0 = no error
 *              1 = error
 * input
 *    cominfoptr - TieCommInfo buffer
 *    msgptr - pointer to message buffer
 *    xdrmsgbuf - pointer to XDR encode/decode buffer.
 *    msglen - encoded message length
 *             (xdr encode length for UDP, UDPASM, and TCP)
 *    dsttiecommid - destination node id
 *    sufrembndcap - pointer to :
 *                   0 - insufficient remaining bandwidth noted
 *                   1 - insufficient remaining bandwidth not noted
 **********************************************************************/
I32s TieCommSendUDPASM(cominfoptr, msgptr, xdrmsgbuf,
    msglen, dsttiecommid, sufrembndcap)
TieCommInfo *cominfoptr;
TMessage *msgptr;
I8u *xdrmsgbuf;
I32u msglen;
struct TieCommID dsttiecommid;
I32s *sufrembndcap;
{
    I32s errflag, rtnval, overnetcap;
    int sendrtn, xdrpktsize;
    XDR msghndl, pkthndl;
    struct sockaddr_in sa;
    struct datpkt pktbuf;
    I32u maxpktdatlen, mesgremlen,i;
    char *xdrpktbuf;
    struct TieCommID reldstcommid;

    errflag=0;
    (*sufrembndcap)=1;
    overnetcap=0;
    pktbuf.dpkt_hdr.pkth_msgid=tuintrand();
    maxpktdatlen=(((cominfoptr->
        tci_pktmtu)-(sizeof(cksmhdr)+
        (prcencsize.pec_pkthdr)+sizeof(u_int)))/
        sizeof(xdrwrd))*sizeof(xdrwrd);

    if((!(msgptr->srcmap))&&(msgptr->dstmap))
    {   reldstcommid.tiec_node=LocalClusterAddr.node;
        reldstcommid.tiec_port=LocalClusterAddr.portnb;
    }
    else
    {   reldstcommid.tiec_node=dsttiecommid.tiec_node;
        reldstcommid.tiec_port=dsttiecommid.tiec_port;
    }

    /* encode message */
    txdrmem_create(&msghndl,
        &(((I8s *)xdrmsgbuf)[sizeof(cksmhdr)]),
        msglen-sizeof(cksmhdr),XDR_ENCODE);
    if((rtnval=txdr_TMessage(&msghndl, msgptr)))
    {   pktbuf.dpkt_hdr.pkth_msglen=
            txdr_getpos(&msghndl)+sizeof(cksmhdr);
#ifdef MSGCHKSUM
        setchksum(xdrmsgbuf,pktbuf.dpkt_hdr.pkth_msglen);
#endif /* MSGCHKSUM */
        memset((char *)&sa, 0, sizeof(sa));
        sa.sin_port=reldstcommid.tiec_port;
        sa.sin_family=cominfoptr->tci_family;
        sa.sin_addr.s_addr=((struct in_addr *)
            &(reldstcommid.tiec_node))->s_addr;

        for(i=0,mesgremlen=pktbuf.
            dpkt_hdr.pkth_msglen;
            (mesgremlen)&&(!errflag);)
        {
            /* packet offset in message */
            pktbuf.dpkt_hdr.pkth_offset=i;

            /* encoded message length in bytes */
            pktbuf.dpkt_hdr.pkth_len=
               (mesgremlen>maxpktdatlen)?
               maxpktdatlen:mesgremlen;

            /* encoded mesg seg len in xdrwrds */
            pktbuf.dpkt_data.dpkt_data_len=
                ByteXdrWrdSize(pktbuf.dpkt_hdr.pkth_len);

            /* allocate message seg buffer */
            pktbuf.dpkt_data.dpkt_data_val=
                (xdrwrd *)thcalloc((pktbuf.
                dpkt_data.dpkt_data_len),
                sizeof(xdrwrd), 2130);

            /* copy message segment to packet */
            memcpy(pktbuf.dpkt_data.dpkt_data_val,
                &(xdrmsgbuf[i]), pktbuf.dpkt_hdr.pkth_len);
            i+=(pktbuf.dpkt_hdr.pkth_len);
            mesgremlen-=(pktbuf.dpkt_hdr.pkth_len);

            /* allocate packet encode buffer */
            xdrpktbuf=(char *)thcalloc(
                XDRBufMaxSize, sizeof(char), 2140);

            /* encode packet */
            txdrmem_create(&pkthndl,(I8s *)(&(xdrpktbuf[
                sizeof(cksmhdr)])), XDRBufMaxSize-
                sizeof(cksmhdr), XDR_ENCODE);
            if((rtnval=txdr_datpkt(&pkthndl, &pktbuf)))
            {   xdrpktsize=txdr_getpos(
                    &pkthndl)+sizeof(cksmhdr);
#ifdef CLSTRSRVR
                UpdRemOutBndCap();
#endif /* CLSTRSRVR */
                if(RemOutBndCap>=0)
                    if((msgptr->dstmap)&&
                        (dsttiecommid.tiec_node!=
                        LocalClusterAddr.node)&&
                        (dsttiecommid.tiec_port!=
                        LocalClusterAddr.portnb)&&
                        (xdrpktsize>RemOutBndCap))
                    {   overnetcap=1;
                        mesgremlen=0;
                    }
                if((!(msgptr->dstmap))||
                    ((msgptr->dstmap)&&(!overnetcap)))
                {
                    /* send packet */
#ifdef PKTCHKSUM
                    setchksum(xdrpktbuf,xdrpktsize);
#endif /* PKTCHKSUM */
                    sendrtn=tsendto(globcominfobuf->
                        tci_comrdhndl,(I8s *)xdrpktbuf,
                        (int)xdrpktsize, 0,
                        (struct sockaddr *)&sa, sizeof(sa));

                    if(sendrtn<0)
                        errflag=1;
                    else /* count messages */
                    {   if(PktSndDelay)
                           tusleep(PktSndDelay);
#ifdef CLSTRSRVR
                        if(msgptr->dstmap)
#endif /* CLSTRSRVR */
                        {   MonCom[0].SNumPack++;
                            MonCom[0].SNumByte+=(I32u)xdrpktsize;
                            MonCom[msgptr->info.tag].SNumPack++;
                            MonCom[msgptr->info.tag].SNumByte+=
                                (I32u)xdrpktsize;
#ifdef TIERRA
                            if(msgptr->dstmap)
#endif /* TIERRA */
                                if(RemOutBndCap>=0)
                                    RemOutBndCap-=xdrpktsize;
                        }
                    }
                }
#ifdef TIERRA
                else
                    if((overnetcap)&&(msgptr->dstmap))
                    {   (*sufrembndcap)=0;
                        errflag=1;
                    }
#endif /* TIERRA */
            }
            else
                errflag=1;
            txdr_destroy(&pkthndl);
            tfree(xdrpktbuf, XDRBufMaxSize*sizeof(char), 2140);
            tfree(pktbuf.dpkt_data.dpkt_data_val, (pktbuf.
                dpkt_data.dpkt_data_len)*sizeof(xdrwrd), 2130);
        }
    }
    else
        errflag=1;
    txdr_destroy(&msghndl);
    return errflag;
}

/**********************************************************************
 * DelPktTree - delete packet tree
 * input
 *    cominfoptr - TieCommInfo buffer
 *    pkttrert- pointer to packet tree
 **********************************************************************/
void DelPktTree(cominfoptr, pkttrert)
TieCommInfo *cominfoptr;
ListHead *pkttrert;
{   struct datpktnod *listend;
    struct datpktnod *curpktnode;

    listend=(struct datpktnod *)
        (&(pkttrert->lsh_firstlast));

    /* while not at list end */
    for(curpktnode=(struct datpktnod *)
        pkttrert->lsh_first;curpktnode!=listend;
        curpktnode=(struct datpktnod *)
        pkttrert->lsh_first)
    {   
        /* delete packet node from list */
        RemHeadNode(pkttrert);
 
        /* deallocate packet node */
        tfree(curpktnode, sizeof(struct datpktnod), 2180);
    }
}

/********************************************************************** 
 * TieRcvIncMsgUDPASM - receive incoming messages - UDPASM
 **********************************************************************/
#ifdef __TURBOC__
int TieRcvIncMsgUDPASM()
#endif /* __TURBOC__ */
#ifdef __MWERKS__
void TieRcvIncMsgUDPASM()
I32s sig;
#endif /* __MWERKS__ */
#ifdef unix
void TieRcvIncMsgUDPASM()
#endif /* unix */
#ifdef DECVAX
void TieRcvIncMsgUDPASM()
#endif /* DECVAX */
#ifdef _WIN32
void TieRcvIncMsgUDPASM()
#endif /* _WIN32 */
#ifdef AMIGA
void TieRcvIncMsgUDPASM()
#endif /* AMIGA */
{   int rcvrtn, exitflag;
    char *xdrpktptr;
    XDR pkthndl;
    struct datpkt pktbuf;
    struct datmesgnod *msgnodptr;
    struct datpktnod *pktnodptr;
    fd_set rdfds;
    struct timeval tmeout;
    struct sockaddr_in from;
    I32s fromlen;
#ifdef _WIN32
   int wsaerrtn;
#endif /* _WIN32 */

    exitflag=0;

    /* allocate packet decode buffer */
    xdrpktptr=(char *)thcalloc(globcominfobuf->
        tci_pktmtu, sizeof(char), 2150);
    while(!exitflag)
    {   FD_ZERO(&rdfds);
        FD_SET(globcominfobuf->tci_comrdhndl, &rdfds);
        tmeout.tv_sec=0;
        tmeout.tv_usec=0;
        tselect((globcominfobuf->tci_comrdhndl)+1,
            &rdfds, 0, 0, &tmeout);
        if(FD_ISSET(globcominfobuf->tci_comrdhndl, &rdfds))
        {
            /* read packet */
            fromlen=sizeof(from);
            rcvrtn=trecvfrom(globcominfobuf->
                tci_comrdhndl, (I8s *)xdrpktptr,
                globcominfobuf->tci_pktmtu,
                0, (struct sockaddr*)(&from), &fromlen);

            if(rcvrtn>0)
            {
#ifdef PKTCHKSUM
                if(!getchksum(xdrpktptr,rcvrtn))
                {
#endif /* PKTCHKSUM */
                    /* decode packet */
                    txdrmem_create(&pkthndl,
                        (I8s *)&(xdrpktptr[sizeof(cksmhdr)]),
                        (globcominfobuf->tci_pktmtu)-
                        sizeof(cksmhdr),XDR_DECODE);
    
                    pktbuf.dpkt_data.dpkt_data_val=0;
                    if(txdr_datpkt(&pkthndl, &pktbuf))
                    {
                        /* if message is not listed */
                        if(!(msgnodptr=srchmsgtree(
                            &(globcominfobuf->tci_msgbklog),
                            pktbuf.dpkt_hdr.pkth_msgid)))
                        {
                            /* if queue is full, */
                            /* remove oldest messages */
                            while((globcominfobuf->
                                tci_queuelen)>=
                                (globcominfobuf->
                                tci_queuesize))
                                if((globcominfobuf->tci_msgbklog.
                                    lsh_first)!=((ListNode*)
                                    (&(globcominfobuf->tci_msgbklog.
                                    lsh_firstlast))))
                                    DelMsgNode(globcominfobuf,
                                        (struct datmesgnod *)
                                        (globcominfobuf->tci_msgbklog.
                                        lsh_first));
    
                            /* add message node to list */
                            msgnodptr=(struct datmesgnod *)
                                thcalloc(1, sizeof(
                                struct datmesgnod), 2160);
                            msgnodptr->msgn_msgid=
                                pktbuf.dpkt_hdr.pkth_msgid;
                            msgnodptr->msgn_len=
                                pktbuf.dpkt_hdr.pkth_msglen;
                            msgnodptr->msgn_timercvd=tietime(NULL);
                            msgnodptr->msgn_src.node=from.sin_addr.s_addr;
                            msgnodptr->msgn_src.portnb=from.sin_port;
                            InitListHead(&(msgnodptr->msgn_listhead));
    
                            /* alloc encoded mesg buff */
                            msgnodptr->msgn_decbuf=(I8u *)thcalloc(
                                (pktbuf.dpkt_hdr.pkth_msglen),
                                sizeof(char), 2170);
                            AddTailNode(&(globcominfobuf->
                                tci_msgbklog),&(msgnodptr->
                                msgn_listnode));
                            (globcominfobuf->tci_queuelen)++;
                        }
                        if(!exitflag)
                        {   if((msgnodptr->msgn_size)!=
                                (msgnodptr->msgn_len))
                            {
                                /* if packet is not listed */
                                if(!srchpkttree(&(msgnodptr->
                                    msgn_listhead),pktbuf.
                                    dpkt_hdr.pkth_offset))
                                {   (msgnodptr->msgn_rnumpack)++;
                                    (msgnodptr->msgn_rnumbyte)+=
                                        (I32u)rcvrtn;
    
                                    /* copy packet data to message */
                                    memcpy(&((msgnodptr->
                                        msgn_decbuf)[pktbuf.
                                        dpkt_hdr.pkth_offset]),
                                        pktbuf.dpkt_data.
                                        dpkt_data_val, pktbuf.
                                        dpkt_hdr.pkth_len);
    
                                    msgnodptr->msgn_size+=
                                        pktbuf.dpkt_hdr.pkth_len;
    
                                    /* add packet node */
                                    /* to list */
                                    pktnodptr=(struct
                                        datpktnod *)thcalloc(
                                        1, sizeof(struct
                                        datpktnod), 2180);
                                    pktnodptr->pktn_offset=
                                        pktbuf.dpkt_hdr.
                                        pkth_offset;
                                    pktnodptr->pktn_len=
                                        pktbuf.dpkt_hdr.
                                        pkth_len;
                                    AddTailNode(&(
                                        msgnodptr->
                                        msgn_listhead),
                                        &(pktnodptr->
                                        pktn_listnode));
            
                                    /* if message is */
                                    /* complete */
                                    if((msgnodptr->msgn_size)==
                                        (msgnodptr->msgn_len))
                                    {
#ifdef MSGCHKSUM
                                        if(!getchksum(msgnodptr->
                                            msgn_decbuf,
                                            msgnodptr->msgn_len))
#endif /* MSGCHKSUM */
                                            DelPktTree(globcominfobuf,
                                                &(msgnodptr->msgn_listhead));
#ifdef MSGCHKSUM
                                        else
                                            DelMsgNode(globcominfobuf,
                                                msgnodptr);
#endif /* MSGCHKSUM */
                                    }
                                }
                            }
                        }
                        txdr_free((xdrproc_t)xdr_datpkt, (I8s *)&pktbuf);
                    }
                    else
                        exitflag=1;
    
                    txdr_destroy(&pkthndl);
#ifdef PKTCHKSUM
                }
#endif /* PKTCHKSUM */
            }
            else
            {   if(rcvrtn<0)
                {   
#ifdef _WIN32
                    wsaerrtn=WSAGetLastError();
#endif /* _WIN32 */

#ifdef _WIN32
                    if(!((wsaerrtn==WSAECONNREFUSED)||
                         (wsaerrtn==WSAECONNRESET)))
#else /* _WIN32 */
                    if(!((errno==ECONNREFUSED)||
                         (errno==ECONNRESET)))
#endif /* WIN32 */
                    {   sigioerrflag=1;
                        exitflag=1;
                    }
                }
            }
        }
        else
            exitflag=1;
    }
    tfree(xdrpktptr, (globcominfobuf->tci_pktmtu), 2150);
    if(sigioerrflag)
        DelMsgTree(globcominfobuf,
            &(globcominfobuf->tci_msgbklog));
}

/**********************************************************************
 * srchpkttree - search packet tree for specific packet node.
 * input
 *    pkttrert - pointer message tree root ListHead structure
 *    pkt - message offset of packet to search for
 * output
 *    datpktnodptr - pointer to datpktnod structure.
 **********************************************************************/
static struct datpktnod *srchpkttree(pkttrert, pktoffset)
ListHead *pkttrert;
I32u pktoffset;
{   struct datpktnod *listend, *curpktnode, *datpktnodptr;
    
    datpktnodptr=(struct datpktnod *)0;
    listend=(struct datpktnod *)(&(pkttrert->lsh_firstlast));

    for(curpktnode=(struct datpktnod *)
        (pkttrert->lsh_first);curpktnode!=listend;
        curpktnode=(struct datpktnod *)
        curpktnode->pktn_listnode.lnd_next)
        if(curpktnode->pktn_offset==pktoffset)
        {   datpktnodptr=curpktnode;
            break;
        }
    return datpktnodptr;
}

/**********************************************************************
 * srchmsgtree - search message tree for specific message node.
 * input
 *    msgtrert - pointer message tree root ListHead structure
 *    msgid - id of message to search for
 * output
 *    datmesgnodptr - pointer to datmesgnod structure.
 **********************************************************************/
static struct datmesgnod *srchmsgtree(msgtrert, msgid)
ListHead *msgtrert;
I32u msgid;
{   struct datmesgnod *listend;
    struct datmesgnod *curmsgnode;
    struct datmesgnod *datmesgnodptr;

    datmesgnodptr=(struct datmesgnod *)0;
    listend=(struct datmesgnod *)(&(msgtrert->lsh_firstlast));

    /* while not at list end */
    for(curmsgnode=(struct datmesgnod *)
        msgtrert->lsh_first;curmsgnode!=listend;
        curmsgnode=(struct datmesgnod *)
        curmsgnode->msgn_listnode.lnd_next)
        /* if message node has given message id */
        if(curmsgnode->msgn_msgid==msgid)
        {   datmesgnodptr=curmsgnode;
            break;
        }
    return datmesgnodptr;
}
#endif /* NETTYPE == UDPASM */
#endif /* NET */
