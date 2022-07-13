/* MemProf labels: #02XX, in use: 0200-0201 */

#ifdef NET
#include "tierra.h"
#include "globals.h"

/**********************************************************************
 * static function prototypes
 **********************************************************************/

static I32s initopnskt P_((TieCommInfo *cominfobuf,
    struct sockaddr_in *sa, char *nodename, I32u portno, int *sock
));

static I32s initsktaddr P_(( char *nodename,
    struct sockaddr_in *sa, I32u portno
));

static I32s opnrdskt P_((TieCommInfo *cominfobuf,
    struct sockaddr_in *sa, I32u portno, int *sock
));

/* FEError labels: #25XX, in use: 2500 */

/**********************************************************************
 * TieCommInit - Initialize communications module.
 * input
 *    cominfoptr - pointer to TieCommInfo buffer to be filled out.
 **********************************************************************/
void TieCommInit(cominfoptr)
TieCommInfo *cominfoptr;
{   cominfoptr->tci_comopen=0;
    cominfoptr->tci_comrdhndl=0;
    cominfoptr->tci_family=0;
    cominfoptr->tci_srctiecommid.tiec_node=0;
    cominfoptr->tci_srctiecommid.tiec_port=0;
    sigioerrflag=0;
    InitListHead(&(cominfoptr->tci_msgbklog));
    cominfoptr->tci_queuesize=0;
    cominfoptr->tci_queuelen=0;

#if NETTYPE == UDPASM
    cominfoptr->tci_pktmtu=0;
#endif /* NETTYPE == UDPASM */

#if NETTYPE == TCP
    cominfoptr->tci_tcplocseltimeout.tv_sec=0;
    cominfoptr->tci_tcplocseltimeout.tv_usec=0;
    cominfoptr->tci_tcpremseltimeout.tv_sec=0;
    cominfoptr->tci_tcpremseltimeout.tv_usec=0;
#endif /* NETTYPE == TCP */

    cominfoptr->tci_maxmsgage=0;
}

/**********************************************************************
 * TieCommSend - send message
 * output
 *    errflag - 0 = no error
 *              1 = error
 * input
 *    cominfobuf - TieCommInfo buffer
 *    msgptr - pointer to message buffer
 *    dsttiecommid - destination node id
 *    sufrembndcap - pointer to :
 *                   0 - insufficient remaining bandwidth noted
 *                   1 - insufficient remaining bandwidth not noted
 **********************************************************************/
int TieCommSend(cominfoptr, msgptr, dsttiecommid, sufrembndcap)
TieCommInfo *cominfoptr;
TMessage *msgptr;
struct TieCommID dsttiecommid;
I32s *sufrembndcap;
{   int  errflag;
    I8u *xdrmsgbuf;

    errflag=0;
    (*sufrembndcap)=1;

#ifdef CLSTRSRVR
    msgptr->srcmap=1;
#else /* CLSTRSRVR */
    msgptr->srcmap=0;
#endif /* CLSTRSRVR */

#ifdef TIERRA
    if(((!ClusterServerUp)&&(!(msgptr->dstmap)))||(ClusterServerUp))
    {
#endif /* TIERRA */
        xdrmsgbuf=(I8u *)thcalloc(XDRBufMaxSize, sizeof(I8u), 200);
#if NETTYPE == UDPASM
        errflag = TieCommSendUDPASM(cominfoptr, msgptr,
            xdrmsgbuf, XDRBufMaxSize, dsttiecommid, sufrembndcap);
#endif /* NETTYPE == UDPASM */

#if NETTYPE == TCP
        errflag = TieCommSendTCP(cominfoptr, msgptr,
            xdrmsgbuf, XDRBufMaxSize, dsttiecommid, sufrembndcap);
#endif /* NETTYPE == TCP */

        if(!errflag)
        {
#ifdef CLSTRSRVR
            if(msgptr->dstmap)
            {
#endif /* CLSTRSRVR */
                MonCom[0].SNumMess++;
                MonCom[msgptr->info.tag].SNumMess++;
#ifdef CLSTRSRVR
            }
#endif /* CLSTRSRVR */
        }
        tfree(xdrmsgbuf, XDRBufMaxSize*sizeof(char), 200);
#ifdef TIERRA
    }
#endif /* TIERRA */
    return errflag;
}

/**********************************************************************
 * TieCommOpen - open communications module.
 * input
 *    cominfoptr - pointer to TieCommInfo buffer to be filled out.
 *    hostport - host port number for incoming messages.
 *    maxqueuesize - maximum backlog queue size
 *    pktmtu - node MTU
 *    maxmsgage - maximum message age in seconds
 *    tcplocseltmesec - tcp select timeout in sec
 *    tcplocseltmeusec - tcp select timeout in usec
 *    tcpremseltmesec - tcp select timeout in sec
 *    tcpremseltmeusec - tcp select timeout in usec
 *    protocol - protocol name
 **********************************************************************/
I32s TieCommOpen(cominfoptr, hostport,
    maxqueuesize, pktmtu, maxmsgage,
    tcplocseltmesec, tcplocseltmeusec,
    tcpremseltmesec, tcpremseltmeusec,
    protocol)
TieCommInfo *cominfoptr;
I32u hostport;
I32u maxqueuesize, pktmtu, maxmsgage;
I32u tcplocseltmesec, tcplocseltmeusec;
I32u tcpremseltmesec, tcpremseltmeusec;
I8s *protocol;
{   I32s errflag;
    struct sockaddr_in sa;
    struct protoent *protoentbuf;

    errflag=0;
    if((protoentbuf=tgetprotobyname(protocol)))
    {   cominfoptr->tci_proto=protoentbuf->p_proto;
        errflag=opnrdskt(cominfoptr, &sa,
            hostport, (int *)&(cominfoptr->tci_comrdhndl));

#if NETTYPE == TCP
        if(!errflag)
            listen(cominfoptr->tci_comrdhndl, SOMAXCONN);
#endif /* NETTYPE == TCP */

        if(!errflag)
        {   cominfoptr->tci_srctiecommid.tiec_node=sa.sin_addr.s_addr;
            cominfoptr->tci_srctiecommid.tiec_port=sa.sin_port;
            cominfoptr->tci_family=sa.sin_family;
            cominfoptr->tci_queuesize=maxqueuesize;

#if NETTYPE == UDP || NETTYPE == UDPASM
            cominfoptr->tci_pktmtu=pktmtu;
#endif /* NETTYPE == UDP || NETTYPE == UDPASM */

#if NETTYPE == TCP
            cominfoptr->tci_tcplocseltimeout.tv_sec=tcplocseltmesec;
            cominfoptr->tci_tcplocseltimeout.tv_usec=tcplocseltmeusec;
            cominfoptr->tci_tcpremseltimeout.tv_sec=tcpremseltmesec;
            cominfoptr->tci_tcpremseltimeout.tv_usec=tcpremseltmeusec;
#endif /* NETTYPE == TCP */

            cominfoptr->tci_comopen=1;
            cominfoptr->tci_maxmsgage=maxmsgage;
        }
        globcominfobuf=cominfoptr;
    }
    else
        errflag=1;

    return errflag;
}

/**********************************************************************
 * TieCommProcMsgBkLog - process message backlog
 * output
 *    errflag - 0 = no error
 *              1 = error
 * input
 *    cominfobuf - TieCommInfo buffer
 *    procmsg - pointer to routine for processing each message.
 **********************************************************************/
int TieCommProcMsgBkLog(cominfobuf, procmsg)
TieCommInfo *cominfobuf;
int (*procmsg) P_((TMessage *,NetAddr *,I32u msgid));
{   int errflag, procmsgflg, delmsgflg;
    struct datmesgnod *nxtmsgnodptr, *curmsgnodptr, *listend;
#ifdef SIGBLOCK
    sigset_t SigIOSaveSet;
#endif /* SIGBLOCK */
    XDR msghndl;
    TMessage *mesg;

#ifdef SIGBLOCK
    SigIOSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    errflag=0;
    listend=(struct datmesgnod *)
        (&(cominfobuf->tci_msgbklog.lsh_firstlast));

    /* start with first message node */
    curmsgnodptr=(struct datmesgnod *)
        cominfobuf->tci_msgbklog.lsh_first;

    /* while not at list end  */
    while((curmsgnodptr!=listend)&&(!errflag))
    {   delmsgflg=0;

        /* save pointer to next node */
        nxtmsgnodptr=(struct datmesgnod *)
            (curmsgnodptr->msgn_listnode.lnd_next);

        if((tietime(NULL)-(curmsgnodptr->msgn_timercvd))<=
            ((I32s)(cominfobuf->tci_maxmsgage)))
        {
#if NETTYPE == UDPASM
            /* if message is complete */
            if((curmsgnodptr->msgn_size)==
                (curmsgnodptr->msgn_len))
            {
#endif /* NETTYPE == UDPASM */

                if(!(curmsgnodptr->msgn_encoded))
                {   (curmsgnodptr->msgn_datmesgbuf)=(I8u *)
                        thcalloc(1, sizeof(TMessage), 201);
                    txdrmem_create(&msghndl,
                        (I8s *)&(((char *)(curmsgnodptr->
                        msgn_decbuf))[sizeof(cksmhdr)]),
                        (u_int)((curmsgnodptr->msgn_len)-
                        sizeof(cksmhdr)),XDR_DECODE);
                    if(txdr_TMessage(&msghndl, (TMessage *)
                        (curmsgnodptr->msgn_datmesgbuf)))
                    {   curmsgnodptr->msgn_encoded=1;
                        tfree(curmsgnodptr->msgn_decbuf,
                            (curmsgnodptr->msgn_len), 2170);
                        curmsgnodptr->msgn_decbuf=0;
                    }
                    else
                        delmsgflg=1;
                    txdr_destroy(&msghndl);
                }
                if(curmsgnodptr->msgn_encoded)
                {   mesg=(TMessage *)(curmsgnodptr->msgn_datmesgbuf);
                    procmsgflg=1;
#ifndef CLSTRSRVR
                    if(((int)(mesg->info.tag))==EMIGRATETAG)
                        if(ImmigLimFrac>=0)
                            if(tdrand()>=ImmigLimFrac)
                                procmsgflg=0;
#endif /* CLSTRSRVR */
                    if(procmsgflg)
                    {
#ifdef CLSTRSRVR
                        if(mesg->srcmap)
                        {
#endif /* CLSTRSRVR */
                            (MonCom[0].RNumMess)++;
#if NETTYPE == UDPASM
                            (MonCom[0].RNumPack)+=
                                (curmsgnodptr->msgn_rnumpack);
#endif /* NETTYPE == UDPASM */
                            (MonCom[0].RNumByte)+=
                                (curmsgnodptr->msgn_rnumbyte);

                            (MonCom[mesg->info.tag].RNumMess)++;
#if NETTYPE == UDPASM
                            (MonCom[mesg->info.tag].RNumPack)+=
                                (curmsgnodptr->msgn_rnumpack);
#endif /* NETTYPE == UDPASM */
                            (MonCom[mesg->info.tag].RNumByte)+=
                                (curmsgnodptr->msgn_rnumbyte);
#ifdef CLSTRSRVR
                        }
#endif /* CLSTRSRVR */

                        mesg->PingData.Fresh=
                            curmsgnodptr->msgn_timercvd;

                        /* process message */
                        errflag=(*procmsg)(mesg,
                            &(curmsgnodptr->msgn_src),
#if NETTYPE == UDPASM
                            curmsgnodptr->msgn_msgid
#else /* NETTYPE == UDPASM */
                            0
#endif /* NETTYPE == UDPASM */
                            );
                        delmsgflg=1;
                    }
                }
#if NETTYPE == UDPASM
            }
#endif /* NETTYPE == UDPASM */
        }
        else
            delmsgflg=1;

        if(delmsgflg)
            DelMsgNode(cominfobuf, curmsgnodptr);

        /* continue with next message */
        curmsgnodptr=nxtmsgnodptr;
    }
    if(!errflag)
        TieRcvIncMsg();

#ifdef SIGBLOCK
    TSigRelease(SigIOSaveSet);
#endif /* SIGBLOCK */

    return errflag;
}

/**********************************************************************
 * TieCommClose
 * input
 *    cominfobuf - TieCommInfo buffer
 **********************************************************************/
void TieCommClose(cominfobuf)
TieCommInfo *cominfobuf;
{   DelMsgTree(cominfobuf, &(cominfobuf->tci_msgbklog));
    if(cominfobuf->tci_comrdhndl)
        tclosesocket(cominfobuf->tci_comrdhndl);
    TieCommInit(cominfobuf);
}

/**********************************************************************
 * TieCommOnOffLine
 * input
 *    onlinereq - request - 0 - offline, 1 - online
 *    startup - 0 - not called at startup, 1 - called at startup
 **********************************************************************/
void TieCommOnOffLine(onlinereq, startup)
I32s onlinereq, startup;
{   if(onlinereq)
    {   if(((OnLineStat)&&(startup))||(!OnLineStat))
        {   if(!TieCommOpen(&globtcibuf,
                (I32s)LocalPort, TieMsgBkLog,
#if NETTYPE == UDP || NETTYPE == UDPASM
                TieMTU,
#else /* NETTYPE == UDP || NETTYPE == UDPASM */
                0,
#endif /* NETTYPE == UDP || NETTYPE == UDPASM */
                TieMsgMaxAge,
#if NETTYPE == TCP
                TCPLocSelTmeSec, TCPLocSelTmeUSec,
                TCPRemSelTmeSec, TCPRemSelTmeUSec,
                "tcp"
#else /* NETTYPE == TCP */
                0, 0,
                0, 0, (I8s *)"udp"
#endif /* NETTYPE == TCP */
            ))
            {
#ifdef CLSTRSRVR
                SetRemOutBndCap();
#endif /* CLSTRSRVR */
                Addr_send(0);
#ifdef CLSTRSRVR
                Addr_send(1);
#else /* CLSTRSRVR */
                if(LocalClusterAddr.node)
                    Addr_send_mesg(LocalClusterAddr,1,-1);
#endif /* CLSTRSRVR */
                OnLineStat=1;
            }
            else
                FEError(-2500, NOEXIT, NOWRITE,
                    "unable to go online");
        }
    }
    else
        if((OnLineStat)&&(!startup))
        {   if(globtcibuf.tci_comopen)
            {   Bye_send(0);
#ifdef CLSTRSRVR
                Bye_send(1);
#else /* CLSTRSRVR */
                if(LocalClusterAddr.node)
                    Bye_send_mesg(LocalClusterAddr,1, -1);
#endif /* CLSTRSRVR */
            }
            TieCommClose(&globtcibuf);
            OnLineStat=0;
        }
}

/**********************************************************************
 * TieCommChgSubNet
 * input
 *    oldsubnet - old value for subnet.
 *    startup - 0 = not called at startup, 1 = called at startup
 *    newsoup - 0 = resume, 1 = new soup
 *    goalter - 0 = not manual parm change, 1 = manual parm change
 **********************************************************************/
void TieCommChgSubNet(oldsubnet, startup, newsoup, goalter)
I32s oldsubnet, startup, newsoup, goalter;
{
#ifndef CLSTRSRVR
    I32s subnetpop, sbntnodct;
    double sizdivrat;
    I32s subnetavgsiz;
#endif /* CLSTRSRVR */

    UpdateOwnMapNode();
#ifdef TIERRA
    if(!(LocalClusterAddr.node))
    {
#endif /* TIERRA */
        if(((startup)&&(newsoup))||((!startup)&&(!goalter)))
        {   if(BasSubNetChgFrq)
            {   TieSubNet=(I32s)(SubNetCnt*tdrand());
                SubNetChgTime=((I32s)
                    (SubNetChgFrqFixFrac*BasSubNetChgFrq))+
                    ((I32s)((((I32s)(SubNetChgFrqRanFrac*
                    BasSubNetChgFrq))+1)*tdrand()));
#ifdef CLSTRSRVR
                SubNetChgTime*=NumNodesC;
#endif /* CLSTRSRVR */
            }
            else
                if(TieSubNet>=0)
                {   if(TieSubNet>=SubNetCnt)
                        TieSubNet=SubNetCnt-1;
                }
                else
                    TieSubNet=0;
        }
#ifndef CLSTRSRVR
        subnetpop=CalcSubNetPop(0,TieSubNet,
            &sbntnodct,&subnetavgsiz);
        sizdivrat=(subnetavgsiz+AverageSize)?(
            ((double)abs(subnetavgsiz-AverageSize))/
            ((double)(subnetavgsiz+AverageSize))):0;
#endif /* CLSTRSRVR */
        if((!startup)&&(((goalter)&&(oldsubnet!=TieSubNet))||
            ((!goalter)&&((tdrand()<SubNetChgApocProb)
#ifndef CLSTRSRVR
            ||(sizdivrat<MinSubNetApoRat)
#endif /* CLSTRSRVR */
            ))))
        {
#ifdef CLSTRSRVR
            IPMapTPing_send(0);
#else /* CLSTRSRVR */
            if(sbntnodct)
                if(subnetpop>(NumCellsMin*sbntnodct))
                    Apocalypse();
#endif /* CLSTRSRVR */
        }

#ifdef TIERRA
        if(!(LocalClusterAddr.node))
            if((!startup)&&(((goalter)&&
                (oldsubnet!=TieSubNet))||(!goalter)))
                Addr_send(0);
    }
#endif /* TIERRA */
}

#ifndef CLSTRSRVR
/**********************************************************************
 * SetLocalNodeSubNet
 * input
 *    address - source of incoming migration message
 *    newsubnet - subnet of source
 **********************************************************************/
void SetLocalNodeSubNet(address, newsubnet)
NetAddr *address;
I32s newsubnet;
{   if(LocalClusterAddr.node)
        if((address->node==LocalClusterAddr.node)&&
            (address->portnb==LocalClusterAddr.portnb))
        {   if((tdrand()<SubNetChgApocProb)&&
                (TieSubNet!=newsubnet))
            {   if(SrvSubNetSet)
                {   SrvrApocFlag=1;
                    TieSleep(0, SrvrApocSleep);
                }
            }
            TieSubNet=newsubnet;
            SrvSubNetSet=1;
        }
}
#endif /* CLSTRSRVR */

#ifdef TIERRA
void TieRcvIncMsg()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

#if NETTYPE == UDPASM
    TieRcvIncMsgUDPASM();
#endif /* NETTYPE == UDPASM */

#if NETTYPE == TCP
    TieRcvIncMsgTCP();
#endif /* NETTYPE == TCP */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}
#endif /* TIERRA */

/**********************************************************************
 * DelMsgTree - delete message tree
 * input
 *    cominfobuf - pointer to TieCommInfo buffer
 *    msgtrert - pointer to packet tree
 **********************************************************************/
void DelMsgTree(cominfobuf, msgtrert)
TieCommInfo *cominfobuf;
ListHead *msgtrert;
{   struct datmesgnod *listend;
    struct datmesgnod *curmsgnode;

    listend=(struct datmesgnod *)
        (&(msgtrert->lsh_firstlast));
    for(curmsgnode=(struct datmesgnod *)
        msgtrert->lsh_first;curmsgnode!=listend;
        curmsgnode=(struct datmesgnod *)msgtrert->lsh_first)
        DelMsgNode(cominfobuf, curmsgnode);
}

/**********************************************************************
 * DelMsgNode - delete message node
 * input
 *    msgnodptr - pointer to message node
 **********************************************************************/
void DelMsgNode(cominfobuf, msgnodptr)
TieCommInfo *cominfobuf;
struct datmesgnod *msgnodptr;
{
    /* delete message node from list */
    RemListNode((ListNode *)msgnodptr);

    /* free message buffers */
    if(msgnodptr->msgn_decbuf)
        tfree(msgnodptr->msgn_decbuf,
            msgnodptr->msgn_len, 2170);
    if(msgnodptr->msgn_datmesgbuf)
    {   if(msgnodptr->msgn_encoded)
            txdr_free((xdrproc_t)xdr_TMessage, (I8s *)
                (msgnodptr->msgn_datmesgbuf));
        tfree(msgnodptr->msgn_datmesgbuf, sizeof(TMessage), 201);
    }

#if NETTYPE == UDPASM
    /* delete packet tree */
    DelPktTree(cominfobuf, &(msgnodptr->msgn_listhead));
#endif /* NETTYPE == UDPASM */

    /* free message node */
    tfree(msgnodptr, sizeof(struct datmesgnod), 2160);

    (cominfobuf->tci_queuelen)--;
}

/**********************************************************************
 * opnwrtskt - open write socket
 * input
 *    cominfobuf - pointer to TieCommInfo buffer
 *    sa - pointer to socket address to be filled out
 *    nodename - name of host for socket address
 *    portno - port number for socket address
 *    sock - pointer to socket to be opened.
 **********************************************************************/
I32s opnwrtskt(cominfobuf, sa, nodename, portno, sock)
TieCommInfo *cominfobuf;
I8u *nodename;
struct sockaddr_in *sa;
I32u portno;
int *sock;
{   return initopnskt(cominfobuf, sa, (char *)nodename, portno, sock);
}

/**********************************************************************
 * rdhndlflsh - flush read handle
 * input
 *     rdhndl - read file handle
 **********************************************************************/
I32s rdhndlflsh(rdhndl)
int rdhndl;
{   I32s nselect, errflag, rcvrtn;
    fd_set rdfds;
    struct timeval tmeout;
    int flushbuflen, optlen;
    I8s *flushbuf;

    errflag=0;
    FD_ZERO(&rdfds);
    FD_SET(rdhndl, &rdfds);
    tmeout.tv_sec=0;
    tmeout.tv_usec=0;

    optlen=sizeof(flushbuflen);
    if(tgetsockopt(rdhndl, SOL_SOCKET, SO_RCVBUF,
        (char *)&flushbuflen, &optlen)>=0)
    {   flushbuf=(I8s *)thcalloc(
            (I32u)flushbuflen, (I32u)sizeof(I8u), 0);
        while(!errflag)
            if((nselect=tselect(Nfds, &rdfds, 0, 0, &tmeout))>0)
                if(FD_ISSET(rdhndl, &rdfds))
                {   if((rcvrtn=trecv(rdhndl,
                        flushbuf, (I32s)flushbuflen, 0))<0)
                        errflag=1;
                    else
                        if(!rcvrtn)
                            break;
                }
                else
                    break;
            else
                if(!nselect)
                    break;
                else
                    errflag=1;
        tfree(flushbuf, (I32s)(flushbuflen*sizeof(I8u)), 0);
    }
    else
        errflag=1;

    return errflag;
}

/**********************************************************************
 * initopnskt - open socket and initialize socket address buffer
 * output
 *    errflag - 0 = no error
 *              1 = error
 * input
 *    cominfobuf - pointer to TieCommInfo buffer
 *    sa - pointer to socket address to be filled out
 *         0 - socket address not needed
 *    nodename - name of host for socket address
 *    portno - port number for socket address
 *    sock - pointer to socket to be opened.
 **********************************************************************/
static I32s initopnskt(cominfobuf, sa, nodename, portno, sock)
TieCommInfo *cominfobuf;
char *nodename;
struct sockaddr_in *sa;
I32u portno;
int *sock;
{   I32s errflag;
    
    errflag=0;
    (*sock)=-1;
    (*sock)=tsocket(PF_INET, TIESOCKTYPE, cominfobuf->tci_proto);
    if((*sock)>=0)
    {   if(sa)
        {   errflag=initsktaddr(nodename, sa, portno);
            if(errflag)
            {   tclosesocket(*sock);
                (*sock)=-1;
            }
        }
    }
    else
        errflag=1;
    return errflag;
}

/**********************************************************************
 * initsktaddr - initialize sockaddr_in structure
 * input
 *    nodename - name of host for socket address
 *    sa - pointer to socket address to be filled out
 *    portno - port number for socket address
 * output
 *    errflag - 0 - no error
 *              1 - error
 **********************************************************************/
static I32s initsktaddr(nodename, sa, portno)
char *nodename;
struct sockaddr_in *sa;
I32u portno;
{   I32s errflag;
    struct hostent *hostentbuf;

    errflag=0;
    memset((char *)sa, 0, sizeof(*sa));
    sa->sin_port=htons((short)portno);
    hostentbuf=tgethostbyname((I8s *)nodename);
    if(hostentbuf)
    {   sa->sin_family=hostentbuf->h_addrtype;
        sa->sin_addr.s_addr=((struct in_addr *)
            &(hostentbuf->h_addr_list[0][0]))->s_addr;
    }
    else
        errflag=1;
    return errflag;
}

/**********************************************************************
 * opnrdskt - open read socket
 * input
 *    cominfobuf - pointer to TieCommInfo buffer
 *    sa - pointer to socket address to be filled out
 *    portno - port number for socket address
 *    sock - pointer to socket to be opened.
 **********************************************************************/
static I32s opnrdskt(cominfobuf, sa, portno, sock)
TieCommInfo *cominfobuf;
struct sockaddr_in *sa;
I32u portno;
int *sock;
{   I32s errflag=1;
    char hostname[MAXHOSTNAME+1];

    if(!tgethostname((I8s *)(&(hostname[0])),sizeof(hostname)))
        if(!(initopnskt(cominfobuf,sa,&(hostname[0]),portno, sock)))
        {   if((!tbind(*sock,(struct sockaddr *)sa, sizeof(*sa))))
                errflag=0;
            if(errflag)
                tclosesocket(*sock);
        }
    return errflag;
}
#endif /* NET */
