#include "tierra.h"

#ifdef NET
#if NETTYPE == TCP

#include "globals.h"

static I32s tcmtcprdmsg P_((char **msgbuf, I32s *bytesread,
    I32s *encmsglen, I32s *msgrdcmplt,
    struct timeval tcpseltimeout, struct sockaddr_in *fsin));

static I32s tcmtcprdsel P_((I32s acptsock, 
    char *rdbuf, I32s bufsiz, I32s *bytesread,
    I32s *rdcmplt, struct timeval tcpseltimeout));

/**********************************************************************
 * TieCommSendTCP - send message
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
I32s TieCommSendTCP(cominfoptr, msgptr, xdrmsgbuf,
    msglen, dsttiecommid, sufrembndcap)
TieCommInfo *cominfoptr;
TMessage *msgptr;
I8u *xdrmsgbuf;
I32u msglen;
struct TieCommID dsttiecommid;
I32s *sufrembndcap;
{   I32s errflag, rtnval, overnetcap;
    int sndsock;
    XDR msghndl;
    struct sockaddr_in sa;
    I32u encmsglen;
    N32u tcphdrint;
    struct TieCommID reldstcommid;

    errflag=0;
    (*sufrembndcap)=1;
    overnetcap=0;

    if((!(msgptr->srcmap))&&(msgptr->dstmap))
    {   reldstcommid.tiec_node=LocalClusterAddr.node;
        reldstcommid.tiec_port=LocalClusterAddr.portnb;
    }
    else
    {   reldstcommid.tiec_node=dsttiecommid.tiec_node;
        reldstcommid.tiec_port=dsttiecommid.tiec_port;
    }

    if(!opnwrtskt(cominfoptr, (struct sockaddr_in *)0,
        (I8u *)0, 0, &sndsock))
    {
        /* encode message */
        txdrmem_create(&msghndl,
            &(((I8s *)xdrmsgbuf)[sizeof(cksmhdr)]), 
            msglen-sizeof(cksmhdr),XDR_ENCODE);
        if((rtnval=txdr_TMessage(&msghndl, msgptr)))
        {   encmsglen=txdr_getpos(&msghndl)+sizeof(cksmhdr);
#ifdef MSGCHKSUM
            setchksum(xdrmsgbuf,encmsglen);
#endif /* MSGCHKSUM */
            memset((char *)&sa, 0, sizeof(sa));
            sa.sin_port=reldstcommid.tiec_port;
            sa.sin_family=cominfoptr->tci_family;
            sa.sin_addr.s_addr=((struct in_addr *)
                &(reldstcommid.tiec_node))->s_addr;

#ifdef CLSTRSRVR
            UpdRemOutBndCap();
#endif /* CLSTRSRVR */

            if(RemOutBndCap>=0)
                if((msgptr->dstmap)&&
                    (dsttiecommid.tiec_node!=
                    LocalClusterAddr.node)&&
                    (dsttiecommid.tiec_port!=
                    LocalClusterAddr.portnb)&&
                    (((I32s)(encmsglen+sizeof(
                    tcphdrint)))>RemOutBndCap))
                    overnetcap=1;

            if((!msgptr->dstmap)||((msgptr->dstmap)&&(!overnetcap)))
            {   rtnval=tconnect(sndsock, (struct sockaddr *)&sa,
                    sizeof(sa), msgptr->dstmap);
                if(!rtnval)
                {   tcphdrint=htonl(encmsglen);
                    rtnval=tsend(sndsock, (I8s *)&tcphdrint,
                        sizeof(tcphdrint), 0);
                    if(rtnval==sizeof(tcphdrint))
                    {
#ifdef CLSTRSRVR
                        if(msgptr->dstmap)
#endif /* CLSTRSRVR */
                        {   MonCom[0].SNumByte+=(I32u)sizeof(tcphdrint);
                            MonCom[msgptr->info.tag].SNumByte+=
                                (I32u)sizeof(tcphdrint);

                            if((msgptr->dstmap)&&
                                (dsttiecommid.tiec_node!=
                                LocalClusterAddr.node)&&
                                (dsttiecommid.tiec_port!=
                                LocalClusterAddr.portnb)&&
                                (RemOutBndCap>=0))
                                RemOutBndCap-=sizeof(tcphdrint);
                        }
                        rtnval=tsend(sndsock, (I8s *)
                           xdrmsgbuf, encmsglen, 0);
                        if(rtnval==(I32s)encmsglen)
                        {
#ifdef CLSTRSRVR
                            if(msgptr->dstmap)
                            {
#endif /* CLSTRSRVR */
                                MonCom[0].SNumByte+=encmsglen;
                                MonCom[msgptr->info.tag].
                                    SNumByte+=encmsglen;

                                if((msgptr->dstmap)&&
                                    (dsttiecommid.tiec_node!=
                                    LocalClusterAddr.node)&&
                                    (dsttiecommid.tiec_port!=
                                    LocalClusterAddr.portnb)&&
                                    (RemOutBndCap>=0))
                                    RemOutBndCap-=encmsglen;
#ifdef CLSTRSRVR
                            }
#endif /* CLSTRSRVR */
                        }
                        else
                           errflag=1;
                    }
                    else
                        errflag=1;
                }
                else
                    errflag=1;
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

        txdr_destroy(&msghndl);
        tclosesocket(sndsock);
    }
    else
        errflag=1;

    return errflag;
}

/********************************************************************** 
 * TieRcvIncMsgTCP - receive incoming messages - TCP
 **********************************************************************/
void TieRcvIncMsgTCP(void)
{   int exitflag;
    struct datmesgnod *msgnodptr;
    char *msgbuf;
    I32s msgrdcmplt, encmsglen;
    I32u RNumByte;
    struct sockaddr_in fsin;

    exitflag=0;
    while(!exitflag)
    {   if(!(sigioerrflag=tcmtcprdmsg(&msgbuf,
            (I32s *)(&RNumByte), &encmsglen, &msgrdcmplt,
            globcominfobuf->tci_tcplocseltimeout,&fsin)))
        {   if(msgrdcmplt)
            {
#ifdef MSGCHKSUM
                if(!getchksum(msgbuf,encmsglen))
                {
#endif /* MSGCHKSUM */
                    /* if queue is full, */
                    /* remove oldest messages */
                    while(globcominfobuf->tci_queuelen>=
                        globcominfobuf->tci_queuesize)
                    {   if((globcominfobuf->tci_msgbklog.
                            lsh_first)!=((ListNode*)
                            (&(globcominfobuf->tci_msgbklog.
                            lsh_firstlast))))
                            DelMsgNode(globcominfobuf,
                                (struct datmesgnod *)
                                (globcominfobuf->tci_msgbklog.
                                lsh_first));
                    }
            
                    /* add message node to list */
                    msgnodptr=(struct datmesgnod *)
                        thcalloc(1, sizeof(struct
                        datmesgnod), 2160);
                    (msgnodptr->msgn_rnumbyte)+=RNumByte;
                    msgnodptr->msgn_decbuf=(I8u *)msgbuf;
                    msgnodptr->msgn_len=encmsglen;
                    msgnodptr->msgn_timercvd=tietime(NULL);
                    msgnodptr->msgn_src.node=fsin.sin_addr.s_addr;
                    msgnodptr->msgn_src.portnb=fsin.sin_port;
                    AddTailNode(&(globcominfobuf->
                        tci_msgbklog),&(msgnodptr->
                        msgn_listnode));
                    globcominfobuf->tci_queuelen++;
#ifdef MSGCHKSUM
                }
#endif /* MSGCHKSUM */
            }
            else
                exitflag=1;
        }
        else
            exitflag=1;
    }
    if(sigioerrflag)
        DelMsgTree(globcominfobuf,
            &(globcominfobuf->tci_msgbklog));
}

/**********************************************************************
 * tcmtcprdmsg - retrieve incoming message header and message
 * output
 *    errflag - 0 = no error
 *              1 = error
 * input
 *    msgbuf - pointer to pointer to message buffer
 *    bytesread - pointer to count of bytes read 
 *    encmsglen - pointer to incoming message encoded length
 *    msgrdcmplt - pointer to "read complete" flag
 *    tcpseltimeout - tcp "select" timeout
 *    fsin - pointer to sockaddr_in struct containing src addr
 **********************************************************************/
static I32s tcmtcprdmsg(msgbuf, bytesread,
    encmsglen, msgrdcmplt, tcpseltimeout, fsin)
char **msgbuf;
I32s *msgrdcmplt, *bytesread, *encmsglen;
struct timeval tcpseltimeout;
struct sockaddr_in *fsin;
{   I32s acptsock, errflag, bufrdcmplt, nselect;
#ifdef AMIGA
    LONG alen;
#else
    int alen;
#endif
    N32u tcphdrint;
    fd_set rdfds;
    struct timeval tmeout;

    errflag=0;
    (*msgrdcmplt)=0;
    (*msgbuf)=0;
    if(bytesread)
        (*bytesread)=0;
    alen=sizeof(*fsin);

    FD_ZERO(&rdfds);
    FD_SET(globcominfobuf->tci_comrdhndl, &rdfds);
    tmeout.tv_sec=0;
    tmeout.tv_usec=0;
    if((nselect=tselect(Nfds, &rdfds, 0, 0, &tmeout))>0)
    {   if(FD_ISSET(globcominfobuf->tci_comrdhndl, &rdfds))
        {   if((acptsock=taccept(globcominfobuf->tci_comrdhndl,
                (struct sockaddr *)fsin, &alen))>=0)
            {   if(!(errflag=tcmtcprdsel(acptsock,
                    (char *)&tcphdrint, sizeof(tcphdrint),
                    bytesread, &bufrdcmplt, tcpseltimeout)))
                {   if(bufrdcmplt)
                    {   *encmsglen=ntohl(tcphdrint);
                        (*msgbuf)=(char *)thcalloc(
                            *encmsglen, sizeof(char), 2170);
                        /* read message */
                        if(!(errflag=tcmtcprdsel(
                            acptsock, *msgbuf, *encmsglen,
                            bytesread, &bufrdcmplt,
                            tcpseltimeout)))
                            if(bufrdcmplt)
                                (*msgrdcmplt)=1;
                        if(!(*msgrdcmplt))
                        {   tfree((*msgbuf), (*encmsglen)*
                                sizeof(char), 2170);
                            (*msgbuf)=0;
                        }
                    }
                }
                tclosesocket(acptsock);
            }
            else
                errflag=1;
        }
    }
    else
        if(nselect<0)
            errflag=1;
    if(!(*msgrdcmplt))
        if(bytesread)
            (*bytesread)=0;
    return errflag;
}

/**********************************************************************
 * tcmtcprdsel - read data from socket until expected read
 *               has completed
 * output
 *    errflag - 0 = no error
 *              1 = error
 * input
 *    acptsock - socket returned from accept() call
 *    rdbuf - pointer to message buffer
 *    bufsize - message buffer size
 *    bytesread - pointer to count of bytes read 
 *    rdcmplt - pointer to "read complete" flag
 *    tcpseltimeout - tcp "select" timeout
 **********************************************************************/
static I32s tcmtcprdsel(acptsock, rdbuf, bufsiz,
    bytesread, rdcmplt, tcpseltimeout)
I32s acptsock, bufsiz, *rdcmplt, *bytesread;
struct timeval tcpseltimeout;
char *rdbuf;
{   struct timeval tmeout;
    I32s errflag, nselect, rcvrtn, remrdlen, bufpsn;
    fd_set rdfds;

    errflag=0;
    (*rdcmplt)=0;
    remrdlen=bufsiz;
    bufpsn=0;

    for(;;)
    {   FD_ZERO(&rdfds);
        FD_SET(acptsock, &rdfds);
        tmeout=tcpseltimeout;
        if((nselect=tselect(Nfds, &rdfds, 0, 0, &tmeout))>0)
        {   if(FD_ISSET(acptsock, &rdfds))
            {   if((rcvrtn=trecv(acptsock,
                    &(rdbuf[bufpsn]), remrdlen, 0))>0)
                {   bufpsn+=rcvrtn;
                    remrdlen-=rcvrtn;
                    if(!remrdlen)
                    {   if(bytesread)
                            (*bytesread)+=bufsiz;
                        (*rdcmplt)=1;
                        break;
                    }
                }
                else
                {   if(!rcvrtn)
                        break;
                    else
                    {   errflag=1;
                        break;
                    }
                }
            }
        }
        else
        {   if(nselect<0)
                errflag=1;
            break;
        }
    }
    return errflag;
}
#endif /* NETTYPE == TCP */
#endif /* NET */
