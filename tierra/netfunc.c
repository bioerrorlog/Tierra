/* @(#)netfunc.c    1.17 12/7/94 */
/* netfunc.c 30-3-98 network functions for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #10XX, in use: 1000 1002-1004 1019 */
/* FEError labels: #10XX, in use: 1009-1021 */

#include "license.h"
#include "tierra.h"
#include "globals.h"

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

#ifdef NET

#define SENDSIZ 2048

#define TPINGDATATTL 1

#ifdef DECVAX
#include "multinet_root:[multinet.include.netinet]in.h"
#else /* DECVAX */
#if defined(unix)||defined(AMIGA)
#include <netinet/in.h>
#endif /* defined(unix)||defined(AMIGA) */
#endif /* DECVAX */

#ifdef DECVAX
#include "multinet_root:[multinet.include.sys]socket.h"
#else /* DECVAX */
#ifdef _WIN32
#include <winsock.h>
#else /* _WIN32 */
#include <sys/socket.h>
#endif /* _WIN32 */
#ifdef __MWERKS__
#include <machine/endian.h>
#endif /* __MWERKS__ */
#endif /* DECVAX */

#ifndef CLSTRSRVR
/*
 * RandIP - return random index to local Map node array
 */
I32s RandIP()
{   I32s ip;

    if (NumNodesC != 0)
        ip = (tlrand()) % NumNodesC;
    else
        ip = -1;
    return ip;
}

/*
 * HammingDist - determine the Hamming distance between
 *               two IP addresses
 *
 * a, b - IP addresses
 *
 * Returns: d - Hamming distance
 */
HAMMINGDIST HammingDist(a, b)
I32u a, b;
{   I32u i,e=1,c;
    HAMMINGDIST d=0;

    for(i=0,c=a^b;i<(sizeof(c)*BITSPERBYTE);i++,e<<=1)
        d+=(c&e)?1:0;
    return d;
}

/*
 * ChooseIP - returns the index of the site in IPMap whose
 *            IP address is identical or nearest to addr
 *            (from an Hamming distance point of view)
 *            Or verifies that it is a valid IP in case of strict = 1
 *
 * addr - suggested IP address
 * port - suggested port
 * whichmap - 0 = IPMapC, 1 = IPMapS
 * strict - 0 = use hamming distance, 1 = require strict match of IP
 *
 * Returns: index_min - IPMap index
 *
 */
I32s ChooseIP(addr, port, whichmap, strict)
Reg addr, port;
I32s whichmap,strict;
{   HAMMINGDIST dist_min_node, dist_node;
#ifdef USE_PORT 
    HAMMINGDIST dist_min_portnb, dist_portnb;
#endif /* USE_PORT */
    I32s NumNodes, index_min, index, newmin;
    MapNode  *IPMap;

    dist_min_node=((sizeof(Reg)>sizeof(N32u))?sizeof(Reg):sizeof(N32u))+1;
#ifdef USE_PORT
    dist_min_portnb=((sizeof(Reg)>sizeof(I16u))?sizeof(Reg):sizeof(I16u))+1;
#endif /* USE_PORT */

    (whichmap)?(NumNodes=NumNodesS,IPMap=IPMapS):
        (NumNodes=NumNodesC,IPMap=IPMapC);
    index_min=(-1);

    for(index=0;index<NumNodes;index++)
    {   dist_node=HammingDist((I32u)addr, (I32u)(IPMap[index].address.node));
#ifdef USE_PORT
        dist_portnb=HammingDist((I32u)port,
            (I32u)(IPMap[index].address.portnb));
#endif /* USE_PORT */

#ifdef USE_PORT
        if(!(dist_node||dist_portnb))
#else /* USE_PORT */
        if(!dist_node)
#endif /* USE_PORT */
        {   index_min=index;
            break;
        }
        else
        {   if(!strict)
            {   newmin=0;
                if(index_min>=0)
                {   if(((dist_node)<dist_min_node)||
                        ((dist_node==dist_min_node)&&
                        ((addr^(IPMap[index].address.node))<
                        (addr^(IPMap[index_min].address.node)))))
#ifdef USE_PORT
                        if(((dist_portnb)<dist_min_portnb)||
                            ((dist_portnb==dist_min_portnb)&&
                            ((port^(IPMap[index].address.portnb))<
                            (port^(IPMap[index_min].address.portnb)))))
#endif /* USE_PORT */
                            newmin=1;
                }
                else
                   newmin=1;
                if(newmin)
                {   index_min=index;
                    dist_min_node=dist_node;
#ifdef USE_PORT
                    dist_min_portnb=dist_portnb;
#endif /* USE_PORT */
                }
            }
        }
    }
    return index_min;
}

/*
 * SaveIPMap - save IPMap to file
 *
 * whichmap - 0 = IPMapC, 1 = IPMapS
 *
 * Returns:
 */
void SaveIPMap(whichmap)
    I32s  whichmap;
{   I8s ReadBuf[80], *host_name, *check;
    struct in_addr addr;
    I16u port;
    I32s i;
#ifdef __MWERKS__
    struct in_addr addressNode;
#endif /* __MWERKS__ */
    I8s *tmp_fn = (I8s *)"ipmap.tmp";
    FILE *map_fp, *tmp_fp;
    I8s *fgetsrtn, *mappath, *tmpmappath;
    struct hostent *hostentbuf;
    I32s NumNodes;
    MapNode  *IPMap;
    I8s  *map_fn;

    if (whichmap)
    {   NumNodes = NumNodesS;
        IPMap = IPMapS;
        map_fn = map_fnS;
    }
    else
    {   NumNodes = NumNodesC;
        IPMap = IPMapC;
        map_fn = map_fnC;
    }

    if(NumNodes)
    {   mappath=(I8s *)tcalloc(strlen(
            (const char *)(&(GenebankPath[0])))+
            strlen((const char *)(&(map_fn[0])))+1,
            sizeof(I8s), 1005);
        tmpmappath=(I8s *)tcalloc(strlen(
            (const char *)(&(GenebankPath[0])))+
            strlen((const char *)(&(tmp_fn[0])))+1,
            sizeof(I8s), 1006);


        tsprintf((char *)(&(mappath[0])), "%s%s", GenebankPath, map_fn);
        tsprintf((char *)(&(tmpmappath[0])), "%s%s", GenebankPath, tmp_fn);

        if ((map_fp = tfopen(mappath, (I8s *)"r")) == NULL)
            FEError(-1020, EXIT, WRITE, "WriteSoup() could not open MapFile");
        if ((tmp_fp = tfopen(tmpmappath, (I8s *)"w")) == NULL)
            FEError(-1021, EXIT, WRITE, "WriteSoup() could not open tmp file");

        check = (I8s *) tcalloc(NumNodes, sizeof(I8s), 1002);
        for(;;)
        {   fgetsrtn=tfgets(ReadBuf, sizeof(ReadBuf), map_fp);
            if(!fgetsrtn)
                break;
            host_name=(I8s *)strtok((char *)(&(ReadBuf[0])),
                (const char *)":");
#ifdef __MWERKS__
            addressNode = inet_addr(strtok(NULL, ":"));
            addr.s_addr = addressNode.s_addr;
#else /* __MWERKS__ */
#ifdef __SOLARIS2__
            {   char addr_s_sddr_buf[64];
                tsprintf(addr_s_sddr_buf, "%s", strtok(NULL, ":"));
                addr.s_addr = inet_addr(addr_s_sddr_buf);
            }
#else /* __SOLARIS2__ */
            addr.s_addr = inet_addr(strtok(NULL, ":"));
#endif /* __SOLARIS2__ */
#endif /* __MWERKS__ */
            port = atoi(strtok(NULL, ";"));
            for (i = 0; i < NumNodes; i++)
            {   if ((addr.s_addr == IPMap[i].address.node)
                    && (port == IPMap[i].address.portnb)
                    && (!check[i]))
                {   tfprintf(tmp_fp,
                        "%s:%s:%hu;\n", host_name, inet_ntoa(addr), port);
                    check[i] = 1;
                    break;
                }
            }
        }
        for (i = 0; i < NumNodes; i++)
        {   if (!check[i])
            {   if((((addr.s_addr = IPMap[i].address.node) != 0)
                    && (IPMap[i].address.portnb != 0)))
                {   if((hostentbuf=tgethostbyaddr((I8s *)(&addr),
                        (I32s)sizeof(struct in_addr), (I32s)AF_INET)))
                        tfprintf(tmp_fp, "%s:%s:%hu;\n",
                            hostentbuf->h_name, inet_ntoa(addr),
                            ntohs(IPMap[i].address.portnb));
                    else
                        tfprintf(tmp_fp,
                            "name unknown:%s:%hu;\n", inet_ntoa(addr),
                            ntohs(IPMap[i].address.portnb));
                }
            }
        }
        tfree(check, NumNodes * sizeof(MapNode), 1002);

        tfclose(tmp_fp);
        tfclose(map_fp);
#ifndef DECVAX
#ifndef _WIN32  /* This is crap wrt a microcomputer... */
        chmod((const char *)(&(tmp_fn[0])),
            S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
                         /* need something better! */
#endif /* _WIN32 */
#endif /* DECVAX */
    
        remove((const char *)mappath);
        rename((const char *)tmpmappath,
            (const char *)mappath);
        remove((const char *)tmpmappath);
    
        tfree(mappath, strlen((const char *)(
            &(GenebankPath[0])))+strlen(
            (const char *)(&(map_fn[0])))+1, 1005);
        tfree(tmpmappath, strlen((const char *)
            (&(GenebankPath[0])))+strlen(
            (const char *)(&(tmp_fn[0])))+1, 1006);
    }
}

/*
 * Remot_from_IPMap - fill in remot structure and RemotXXXs
 *                    with data from IPMap array element
 *                    with given index
 *
 * index - given IPMap array index
 * whichmap - 0 = IPMapC, 1 = IPMapS
 *
 * Returns: 0 on success, -1 on failure
 *
 */
I32s Remot_from_IPMap(index, whichmap)
    I32s index, whichmap;
{   MapNode  *IPMap;

    if (whichmap)
        IPMap = IPMapS;
    else
        IPMap = IPMapC;

    if ((IPMap[index].address.node != 0)
        && (IPMap[index].address.portnb != 0))
    {   RemotPort = ntohs(IPMap[index].address.portnb);
        memset((I8s *) & remot, 0, sizeof(remot));
        remot.sin_family = AF_INET;
        remot.sin_addr.s_addr = IPMap[index].address.node;
        remotlen = sizeof(remot);
        remot.sin_port = htons(RemotPort);
        RemotHostAddr=(I8s *)inet_ntoa(remot.sin_addr);
        return 0;
    }
    else
        return -1;
}

/*
 * TPing_recv - process a TPing reply
 *
 * Returns: 0 - message processed, -2 - message cannot be processed
 */
I32s TPing_recv(mesg)
    TMessage *mesg;
{   TPingData *data;
    Pcells cp = TopReap;
    I32s  i, found = 0;
    IOS *recv;

    if (mesg->info.Data_u.TPingRep.dest.ranID == -1)
        return (-2);
    for (i = 0; i < NumCells; i++)
    {   if (cp->d.RanID == mesg->info.Data_u.TPingRep.dest.ranID)
        {   found = 1;
            break;
        }
        cp = &cells[cp->q.n_reap.a][cp->q.n_reap.i];
    }
    if (!found)
    {   

#ifdef TIERRA
#if FRONTEND == BASIC
        if(!HistoInEffect())
        {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
            FEPrintf(MSG_X, MSG_Y, 0, 0, "Sender died\n");

#ifdef TIERRA
#if FRONTEND == BASIC
        }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
        return (-2);
    }
    recv = GetFreeIOS(cp);  /* find Free IOS to be filled in IO buffer */
    IncrIOSnio(cp);  /* increment cells's IOS buffer */
    recv->s = mesg->info.Data_u.TPingRep.source;
    recv->e = mesg->info.Data_u.TPingRep.dest;
    recv->t = mesg->info.tag;  /* fill in socket address info */
    recv->d.len = sizeof(TPingData);
    recv->d.d = (I8s *) tcalloc(recv->d.len, sizeof(I8s), 702);
    data = (TPingData *) recv->d.d;
    *(data) = mesg->PingData;
    addsignal(TPINGREPTAG, cp);
    return 0;
}

/*
 * STPingUpdateReq - send a tping request to every node in IPMapC
 *                   (local cluster machines)
 *
 * Returns: none
 */
void STPingUpdateReq()
{   ComAddr cell_addr;
    I32s prop, sufrembndcap,dropok;
    TMessage mesg;
    struct in_addr source;
    struct TieCommID dstcommid;
    struct timeval now;

    prop = (InstExe.i * NumNodesC) / 1000000L;
    if (prop >= (I32s)NumNodesC)
        prop = 0;
    if (prop == PipmapC)
        return;
    if (PipmapC >= (I32s)NumNodesC)
        PipmapC = 0;

    SelfID(NULL, &cell_addr);  /* fill in IP and port number */
    memset((char *) &mesg, 0, sizeof(TMessage));
    mesg.info.tag = TPINGREQTAG;
    mesg.info.Data_u.TPingReq.source = cell_addr;
    mesg.PingData.TransitTime = 0;
    GetTPingData(&(mesg.PingData), &(mesg.PrivPingData), -1);
    tgettimeofday(&now, NULL);
    mesg.dstmap=0;
    mesg.srcmap=0;

    while(PipmapC != prop) /* send to every site in IPMapC */
    {   if(((IPMapC[PipmapC].address.node)!=(LocalNetAddr.node))||
            ((IPMapC[PipmapC].address.portnb)!=(LocalNetAddr.portnb)))
        {   if (Remot_from_IPMap(PipmapC, 0) == 0)
                /*read address info from IPMapC*/
            {   /* if we already have PendReqMax pending (and unanswered)
                 * TPing requests, we will zero the speed of this node
                 */
                if((((IPMapC[PipmapC].PendReq)>=PendReqMax)&&
                    (PendReqMax))||((((I32s)((now.tv_sec)-
                    (IPMapC[PipmapC].data.Fresh)))>=
                    SpdZeroTime)&&(SpdZeroTime)))
                {   IPMapC[PipmapC].data.Speed = 0;
                    IPMapC[PipmapC].PrivPingData.subnet=(-1);
                    /* if we haven't heard from this node in
                     * PendReqTime, we will remove this node
                     */
                    if((((I32s)((now.tv_sec)-(IPMapC[PipmapC].
                        data.Fresh)))>=PendReqTime)&&(PendReqTime))
                    {   source.s_addr = IPMapC[PipmapC].address.node;

                        dropok=1;
#ifndef CLSTRSRVR
                        if(LocalClusterAddr.node)
                            if(IPMapC[PipmapC].address.node==
                                LocalClusterAddr.node)
                                if(IPMapC[PipmapC].address.portnb==
                                    LocalClusterAddr.portnb)
                                    dropok=0;
#endif /* CLSTRSRVR */
                        if(dropok)
                        {   if(AutoRemIPMap)
                            {
#ifdef TIERRA
#if FRONTEND == BASIC
                                if(!HistoInEffect())
                                {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                                    tsprintf((char *)(&(Fbuf[0])),
                                        "Node %s:%hu leaving the net\n",
                                        inet_ntoa(source),
                                        ntohs(IPMapC[PipmapC].
                                        address.portnb));
                                    FEPrintf(MSG_X, MSG_Y, 0, 0,
                                        (char *)(&(Fbuf[0])));
#ifdef BGL
                                    TBgl_make_femessages(&(Fbuf[0]),
                                        strlen((const char *)(&(Fbuf[0]))));
#endif /* BGL */
#ifdef TIERRA
#if FRONTEND == BASIC
                                }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                                RemoveIPMap(IPMapC[PipmapC].address, 0);
                            }
                        }
                        if (prop >= (I32s)NumNodesC)
                            prop = 0;
                        if (PipmapC >= (I32s)NumNodesC)
                            PipmapC = 0;
                        continue;
                    }
                }
                dstcommid.tiec_node=IPMapC[PipmapC].address.node;
                dstcommid.tiec_port=IPMapC[PipmapC].address.portnb;
                if(!TieCommSend(&globtcibuf, &mesg, dstcommid, &sufrembndcap))
                    IPMapC[PipmapC].PendReq++;
            }
            else
            {   
#ifdef TIERRA
#if FRONTEND == BASIC
                if(!HistoInEffect())
                {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                    tsprintf((char *)(&(Fbuf[0])),
                        "STPingUpdateReq() index got null address\n");
                    FEPrintf(MSG_X, MSG_Y, 0, 0,&(Fbuf[0]));
#ifdef TIERRA
#if FRONTEND == BASIC
                }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
            }
        }
        else
        {   IPMapC[PipmapC].data = mesg.PingData;
            IPMapC[PipmapC].data.TransitTime = 0;
            IPMapC[PipmapC].PendReq = 0;
        }
        PipmapC++;
        if (PipmapC >= (I32s)NumNodesC)
            PipmapC = 0;
    }
}

/*
 * connectTCP - connect to <service/port> on <host> using tcp protocol
 *
 * host - host to connect to
 * service - service name or port number to connect to
 *
 * Returns: socket descriptor
 */
I32s connectTCP(host, service)
    I8s *host;
    I8s *service;
{   return connectsock(host, service, (I8s *)"tcp");
}

/*
 * connectUDP - connect to <service/port> on <host> using udp protocol
 *
 * host - host to connect to
 * service - service name or port number to connect to
 *
 * Returns: socket descriptor
 */
I32s connectUDP(host, service)
    I8s *host;
    I8s *service;
{   return connectsock(host, service, (I8s *)"udp");
}

/*
 * connectsock - connect to <service/port> on <host> using <protocol>
 *
 * host - host to connect to
 * service - service name or port number to connect to
 * protocol - name of protocol (ex tcp, udp) to use 
 *
 * Returns: socket descriptor
 */
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif /* INADDR_NONE */

I32s connectsock(host, service, protocol)
    I8s *host;
    I8s *service;
    I8s *protocol;
{   struct hostent *phe;
    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;
    I32s s, type;
#ifdef __MWERKS__
    struct in_addr addressNode;
#endif /* __MWERKS__ */

    memset((char *) &sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    if ((pse = tgetservbyname(service, protocol)))
        sin.sin_port = pse->s_port;
    else if((sin.sin_port=htons((u_short)atoi((const char *)service)))==0)
    {   tsprintf((char *)(&(Fbuf[0])),
            "can't get \"%s\" service entry", service);
        FEError(-1009, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't get \"%s\" service entry\n", service); */
    }

    if ((phe = tgethostbyname(host)))
        memcpy((char *) &sin.sin_addr, phe->h_addr, phe->h_length);
#ifdef __MWERKS__
    else if ((sin.sin_addr.s_addr = (addressNode = inet_addr(host)).s_addr)
        == htonl(INADDR_NONE))
    {
#else
    else if ((sin.sin_addr.s_addr=inet_addr(
        (const char *)host))==htonl(INADDR_NONE))
    {
#endif /* __MWERKS__ */
        tsprintf((char *)(&(Fbuf[0])), "can't get \"%s\" host entry", host);
        FEError(-1010, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't get \"%s\" host entry\n",host); */
    }

    if ((ppe = tgetprotobyname(protocol)) == 0)
    {   tsprintf((char *)(&(Fbuf[0])),
            "can't get \"%s\" protocol entry", protocol);
        FEError(-1011, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't get \"%s\" protocol entry\n", protocol); */
    }

    if (strcmp((const char *)protocol, (const char *)"udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = tsocket(PF_INET, type, ppe->p_proto);
    if (s < 0)
    {   tsprintf((char *)(&(Fbuf[0])), "can't create socket");
        FEError(-1012, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't create socket: %s\n", sys_errlist[errno]); */
    }

    if (tconnect(s, (struct sockaddr *) &sin, sizeof(sin), 1) < 0)
    {   tsprintf((char *)(&(Fbuf[0])),
            "can't connect to %s.%s", host, service);
        FEError(-1013, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't connect to %s.%s: %s\n", host, service,
           sys_errlist[errno]); */
    }
    return s;
}
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/* 
 * errexit.c
 */
/* ##################   Not Used ###############
   Problem of #include <varargs.h> in tierra.h
   #include <varargs.h>
   #include <stdio.h>
   I32s errexit(format, va_alist)
   I8s *format;
   va_dcl 
   {   va_list args;
   va_start(args);
   _doprnt(format, args, stderr);
   va_end(args);
   exit(1);
   }
   ############################################## */


/*
 * passiveTCP - set up receiving tcp socket at given <service/port>
 *              with given "listen" queue length
 *
 * service - service name or port number to connect to
 * qlen - given "listen" queue length
 *
 * Returns: socket descriptor
 */
I32s passiveTCP(service, qlen)
    I8s *service;
    I32s qlen;
{   return passivesock(service, (I8s *)"tcp", qlen);
}

/*
 * passiveUDP - set up receiving udp socket at given <service/port>
 *
 * service - service name or port number to connect to
 *
 * Returns: socket descriptor
 */
I32s passiveUDP(service)
    I8s *service;
{   return passivesock(service, (I8s *)"udp", 0);
}

/*
 * passivesock - set up receiving socket at given <service/port>
 *               using given <protocol> (ex tcp/udp)
 *               if tcp, set listen queue length to given in qlen
 *
 * Returns: return socket descriptor
 */

#ifndef BGL
I16u portbase = 0;
#endif /* BGL */
I32s passivesock(service, protocol, qlen)
    I8s *service;
    I8s *protocol;
    I32s qlen;
{   struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;
    I32s s, type;
    char thisname[MAXHOSTNAME + 1];
    struct hostent *hp;
#ifdef BGL
    I16u portbase = 0;
#endif /* BGL */

    tgethostname((I8s *)(&(thisname[0])), MAXHOSTNAME);
    hp = tgethostbyname((I8s *)(&(thisname[0])));
    memset((char *) &sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = *((uint32_t *)(hp->h_addr));

    if ((pse = tgetservbyname(service, protocol)))
        sin.sin_port=htons(ntohs((I16u)((pse->s_port)+portbase)));
    else if((sin.sin_port=htons((u_short)atoi((const char *)service)))==0)
    {   tsprintf((char *)(&(Fbuf[0])),
            "can't get \"%s\" service entry", service);
        FEError(-1014, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't get \"%s\" service entry\n", service); */
    }

    if ((ppe = tgetprotobyname(protocol)) == 0)
    {   tsprintf((char *)(&(Fbuf[0])),
            "can't get \"%s\" protocol entry", protocol);
        FEError(-1015, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't get \"%s\" protocol entry\n", protocol); */
    }

    if (strcmp((const char *)protocol, (const char *)"udp")==0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = tsocket(PF_INET, type, ppe->p_proto);
    if (s < 0)
    {   tsprintf((char *)(&(Fbuf[0])), "can't create socket");
        FEError(-1016, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't create socket: %s\n",sys_errlist[errno]); */
    }

    if (tbind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0)
    {   tsprintf((char *)(&(Fbuf[0])), "can't bind %s port\n", service);
        FEError(-1017, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't bind %s port: %s\n", service,
           sys_errlist[errno]); */
    }
    if (type == SOCK_STREAM && listen(s, qlen) < 0)
    {   tsprintf((char *)(&(Fbuf[0])), "can't listen on %s port\n", service);
        FEError(-1018, EXIT, WRITE, (char *)(&(Fbuf[0])));
        /* errexit("can't listen on %s port: %s\n", service,
           sys_errlist[errno]); */
    }
    return s;
}

#ifndef DECVAX

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s MonInitMesFunc(fd)
I32s fd;
#else
I32s MonInitMesFunc(I32s fd)
#endif
{   I32s cc;
    I8s RecvMonBuf[80];
    I8s SendMonBuf[80];

    cc=(I32s)tread(fd, RecvMonBuf, sizeof(RecvMonBuf));
    if (cc >= 0)
    {   if (SuMonFlg == 0)
        {   SuMonFlg = 1;
            tsprintf((char *)(&(SendMonBuf[0])), "0,0,0,0,T");
        }
        else
            tsprintf((char *)(&(SendMonBuf[0])), "0,0,0,0,F");
      twrite(fd,(char *)(&(SendMonBuf[0])),strlen(
          (const char *)(&(SendMonBuf[0])))+1);
    }
    return cc;
}
#ifdef BGL
/****************************************************************************
 * Migration Tracking Code
 ****************************************************************************
 */

/*
 * MigrMigrateMesg - sends message to Beagle announcing the migration of
 *                   this cell
 */
void MigrMigrateMesg(cr, index)
     Pcells cr;
     I32s  index;
{
  I32s TimeNow;
  ComAddr cell_addr;
  I8s label[4];
  I32s size;
    
  TimeNow = (I32s) tietime(NULL);
  SelfID(ce, &cell_addr);
  /* init the data to send to beagle */
  tsprintf((char *)(&(BglMigrationDataBuf.
           localparameter[0])), "%.4d%3s",
           cr->d.gen.size, (GeneBnker)?
           (cr->d.gen.label):((I8s *)"---"));
  BglMigrationDataBuf.storelparam = 0;
  BglMigrationDataBuf.session = cr->d.BglTrack.session;
  BglMigrationDataBuf.information = 'M';
  BglMigrationDataBuf.ranID = cell_addr.ranID;
  BglMigrationDataBuf.tierraTime = TimeNow;
  BglMigrationDataBuf.beagleTime = 0;
  BglMigrationDataBuf.fecundity = cr->d.fecundity;

  /* check if the creature mutated */
  if ((sscanf((const char *)(&(cr->d.BglTrack.param[0])),
      (const char *)"%d%3s", &size, label) == 2)
      && (label[0] <= 'z') && (label[0] >= 'a')
      && (label[1] <= 'z') && (label[1] >= 'a')
      && (label[2] <= 'z') && (label[2] >= 'a')
      && TrackNormLike
      && (strcmp((const char *)(&(cr->d.BglTrack.param[0])),
      (const char *)(&(BglMigrationDataBuf.localparameter[0])))))
    strcpy((char *)(&(cr->d.BglTrack.param[0])),
        (const char *)"Mutated");

  /* init data for connexion */
  BglMigrationConnectBuf.node    = cr->d.BglTrack.node;
  BglMigrationConnectBuf.portnb  = cr->d.BglTrack.portnb;
  BglMigrationConnectBuf.session = cr->d.BglTrack.session;
  strcpy((char *)(&(BglMigrationConnectBuf.
      param[0])), (const char *)(&(cr->d.BglTrack.param[0])));

  /* send data to beagle and abort migration tracking if problems */
  switch (TBglMigration()){
  case 2:
    MigrStopGet(cr->d.BglTrack);
    break;
  case 1: /* Beagle doesn't respond */
    MigrStopGet(cr->d.BglTrack);
    cr->d.BglTrack.node = 0;
    cr->d.BglTrack.portnb = 0;
    cr->d.BglTrack.session = 0;
    cr->d.BglTrack.param[0] = '\0';
  default:
    break;
  }
  MigrationTrackCount--;
}

/*
 * MigrArrivalMesg - sends message to Beagle announcing the arrival
 *                   of this cell
 */
void MigrArrivalMesg(cp, mesg)
     Pcells cp;
     TMessage *mesg;
{
    I32s TimeNow;
    ComAddr cell_addr;
    int genetrack = 0, sizetrack = 0;
    I8s label[4];
    I32s size;
    GList *tgl;

    TimeNow = (I32s) tietime(NULL);
    SelfID(cp, &cell_addr);

    /* Examine the creature which arrived */
    BglMigrationDataBuf.storelparam = 0;
    if (mesg->info.Data_u.Creature.BglTrack.param[0])
    {   if(((genetrack=(((sizetrack = sscanf((const char *)
            (&(mesg->info.Data_u.Creature.BglTrack.param[0])),
            "%d%3s", &size, label)) == 2)
            && (label[0] <= 'z') && (label[0] >= 'a')
            && (label[1] <= 'z') && (label[1] >= 'a')
            && (label[2] <= 'z') && (label[2] >= 'a')))))
        {   if (TrackNormLike)
            {   BglMigrationDataBuf.storelparam=1;
                if (!GeneBnker)
                    OpenGeneBanker();
                tgl = sl[cp->d.gen.size]->g[cp->d.gi];
                if (!IsBit(tgl->bits, 0))
                {   SetBit(&tgl->bits, 0, 1);
                    SetBit(&tgl->bits, 1, 0);
                    extract(cp->d.gen.size, 0, cp->d.gi, 0);
                }
            }
        }
    }

    /* init the data to send to beagle */
    tsprintf((char *)(&(BglMigrationDataBuf.
        localparameter[0])), "%.4d%3s",
        cp->d.gen.size, (GeneBnker)?
        (cp->d.gen.label):((I8s *)"---"));
    BglMigrationDataBuf.session=
        mesg->info.Data_u.Creature.BglTrack.session;
    BglMigrationDataBuf.information = 'I';
    BglMigrationDataBuf.ranID       = cell_addr.ranID;
    BglMigrationDataBuf.tierraTime  = TimeNow;
    BglMigrationDataBuf.beagleTime  = 0;
    BglMigrationDataBuf.fecundity   = cp->d.fecundity;

    /* fprintf(stderr, "%d\n", cp->d.hash); */

    /* init data for connexion */
    BglMigrationConnectBuf.node =
        mesg->info.Data_u.Creature.BglTrack.node;
    BglMigrationConnectBuf.portnb =
        mesg->info.Data_u.Creature.BglTrack.portnb;
    BglMigrationConnectBuf.session =
        mesg->info.Data_u.Creature.BglTrack.session;
    if (genetrack && TrackNormLike)
        tsprintf((char *)(&(BglMigrationConnectBuf.param[0])),
        "%.4d%3s", cp->d.gen.size, cp->d.gen.label);
    else
        strcpy((char *)(&(BglMigrationConnectBuf.param[0])),
            (const char *)(&(mesg->info.Data_u.
            Creature.BglTrack.param[0])));

    /* send data to beagle and abort migration tracking if problems */
    switch (TBglMigration())
    {   case 1:
            cp->d.BglTrack.node = 0;
            cp->d.BglTrack.portnb = 0;
            cp->d.BglTrack.session = 0;
            cp->d.BglTrack.param[0] = '\0';
            break;
        case 2:
            MigrStopPut(mesg->info.Data_u.
                Creature.BglTrack); /* termination */
        default:      
            cp->d.BglTrack.node= mesg->info.
                Data_u.Creature.BglTrack.node;
            cp->d.BglTrack.portnb = mesg->
                info.Data_u.Creature.BglTrack.portnb;
            cp->d.BglTrack.session = mesg->
                info.Data_u.Creature.BglTrack.session;
            if (genetrack && TrackNormLike)
                tsprintf((char *)(&((cp->
                    d.BglTrack.param)[0])), "%.4d%3s",
                    cp->d.gen.size, cp->d.gen.label);
            else
                strcpy((char *)(&(cp->d.BglTrack.param[0])),
                    (const char *)(&(mesg->info.Data_u.
                    Creature.BglTrack.param[0])));
            MigrationTrackCount++;
    }
}


/*
 * MigrSleepMesg - sends message to Beagle announcing sleep on this node
 */
void MigrSleepMesg()
{
  I32s   i;
  Pcells cr;

  cr = TopReap;
  for (i = 0; i < NumCells; i++){
#ifdef NET
    if (cr->d.BglTrack.node){ /* this cell currently being tracked */
      /* Send message to Beagle, announcing sleep
       * and delete flag */
      MigrToolTerminateMesg(cr, M_sleep);
      cr->d.BglTrack.node = 0;
      cr->d.BglTrack.portnb = 0;
      cr->d.BglTrack.session = 0;
      cr->d.BglTrack.param[0] = '\0';
      MigrationTrackCount--;
    }
#endif /* NET */
    if (!MigrationTrackCount)            break;
    cr = &cells[cr->q.n_reap.a][cr->q.n_reap.i];
  }
}

/*
 * MigrApocalypseMesg - sends message to Beagle announcing apocalypse
 *                      on this node
 */
void MigrApocalypseMesg(cr)
     Pcells cr;
{
  /* Send message to Beagle, announcing apocalypse
   * and delete flag */
  MigrToolTerminateMesg(cr, M_apocalypse);
  cr->d.BglTrack.node = 0;
  cr->d.BglTrack.portnb = 0;
  cr->d.BglTrack.session = 0;
  cr->d.BglTrack.param[0] = '\0';
  MigrationTrackCount--;
}

/*
 * MigrToolRemotbirthMesg - This fonction send remote birth information
 *                          to Beagle
 */
void MigrToolRemotbirthMesg(cr)
     Pcells cr;
{   I32s TimeNow;
    ComAddr cell_addr;

    TimeNow = (I32s) tietime(NULL);
    SelfID(cr, &cell_addr);
    /* init the data to send to beagle */
    tsprintf((char *)(&(BglMigrationDataBuf.
        localparameter[0])), "%.4d%3s",
        cr->d.gen.size, (GeneBnker)?
        (cr->d.gen.label):((I8s *)"---"));
    BglMigrationDataBuf.storelparam = 0;
    BglMigrationDataBuf.session = cr->d.BglTrack.session;
    BglMigrationDataBuf.information= 'R';
    BglMigrationDataBuf.ranID= cell_addr.ranID;
    BglMigrationDataBuf.tierraTime= TimeNow;
    BglMigrationDataBuf.beagleTime= 0;
    BglMigrationDataBuf.fecundity= cr->d.fecundity;

    /* init data for connexion */
    BglMigrationConnectBuf.node    = cr->d.BglTrack.node;
    BglMigrationConnectBuf.portnb  = cr->d.BglTrack.portnb;
    BglMigrationConnectBuf.session = cr->d.BglTrack.session;
    strcpy((char *)(&(BglMigrationConnectBuf.param[0])),
        (const char *)(&(cr->d.BglTrack.param[0])));

    /* send data to beagle and abort migration tracking if problems */
    switch (TBglMigration())
    {   case 2:
            MigrStopGet(cr->d.BglTrack);
            break;
        case 1:
            MigrStopGet(cr->d.BglTrack);
            cr->d.BglTrack.node = 0;
            cr->d.BglTrack.portnb = 0;
            cr->d.BglTrack.session = 0;
            cr->d.BglTrack.param[0] = '\0';
            MigrationTrackCount--;
        default:
            break;
    }
}



/*
 * MigrToolBirthMesg - This fonction sends birth information to Beagle 
 */
void MigrToolBirthMesg(cr)
     Pcells cr;
{   I32s  TimeNow;
    ComAddr cell_addr;

    TimeNow = (I32s) tietime(NULL);
    SelfID(cr, &cell_addr);
    /* init the data to send to beagle */
    tsprintf((char *)(&(BglMigrationDataBuf.
        localparameter[0])), "%.4d%3s",
        cr->d.gen.size, (GeneBnker)?
        (cr->d.gen.label):((I8s *)"---"));
    BglMigrationDataBuf.storelparam= 0;
    BglMigrationDataBuf.session= cr->d.BglTrack.session;
    BglMigrationDataBuf.information= 'B';
    BglMigrationDataBuf.ranID= cell_addr.ranID;
    BglMigrationDataBuf.tierraTime= TimeNow;
    BglMigrationDataBuf.beagleTime= 0;
    BglMigrationDataBuf.fecundity= cr->d.fecundity;

    /* init data for connexion */
    BglMigrationConnectBuf.node= cr->d.BglTrack.node;
    BglMigrationConnectBuf.portnb= cr->d.BglTrack.portnb;
    BglMigrationConnectBuf.session= cr->d.BglTrack.session;
    strcpy((char *)(&(BglMigrationConnectBuf.param[0])),
        (const char *)(&(cr->d.BglTrack.param[0])));

    /* send data to beagle and abort migration tracking if problems */
    switch (TBglMigration())
    {   case 2:
            MigrStopPut(cr->d.BglTrack); /* termination */
            break;
        case 1: /* Beagle doesn't respond */
            MigrStopGet(cr->d.BglTrack);
            cr->d.BglTrack.node= 0;
            cr->d.BglTrack.portnb= 0;
            cr->d.BglTrack.session= 0;
            cr->d.BglTrack.param[0]= '\0';
            MigrationTrackCount--;
        default:
            break;
    }
}


/*
 * MigrDeathMesg - sends message to Beagle announcing the death of this cell
 */
void MigrDeathMesg(cr)
     Pcells cr;
{   I32s TimeNow;
    ComAddr cell_addr;
    int genetrack;
    I8s label[4];
    I32s size;

    TimeNow = (I32s) tietime(NULL);
    SelfID(cr, &cell_addr);
    /* init the data to send to beagle */
    tsprintf((char *)(&(BglMigrationDataBuf.
        localparameter[0])), "%.4d%3s",
        cr->d.gen.size, (GeneBnker)?
        (cr->d.gen.label):((I8s *)"---"));
    BglMigrationDataBuf.storelparam= 0;
    BglMigrationDataBuf.session= cr->d.BglTrack.session;
    BglMigrationDataBuf.information= 'D';
    BglMigrationDataBuf.ranID= cell_addr.ranID;
    BglMigrationDataBuf.tierraTime= TimeNow;
    BglMigrationDataBuf.beagleTime= 0;
    BglMigrationDataBuf.fecundity= cr->d.fecundity;

    /* check if the code mutated */
    if((genetrack=(sscanf((const char *)
        (&(cr->d.BglTrack.param[0])),
        (const char *)"%d%3s", &size, label) == 2)&&
        (label[0] <= 'z') && (label[0] >= 'a')&&
        (label[1] <= 'z') && (label[1] >= 'a')&&
        (label[2] <= 'z') && (label[2] >= 'a'))&&
        TrackNormLike &&(strcmp((const char *)
        (&(cr->d.BglTrack.param[0])),
        (const char *)(&(BglMigrationDataBuf.
        localparameter[0])))))
        strcpy((char *)(&(cr->d.BglTrack.param[0])),
            (const char *)"Mutated");

    /* init data for connexion */
    BglMigrationConnectBuf.node    = cr->d.BglTrack.node;
    BglMigrationConnectBuf.portnb  = cr->d.BglTrack.portnb;
    BglMigrationConnectBuf.session = cr->d.BglTrack.session;
    strcpy((char *)(&(BglMigrationConnectBuf.param[0])),
        (const char *)(&(cr->d.BglTrack.param[0])));

    /* send data to beagle and abort migration tracking if problems */
    switch (TBglMigration())
    {   case 2:
            MigrToolTerminateMesg(cr, M_normal);
        case 1: /* Beagle doesn't respond */
            MigrStopGet(cr->d.BglTrack);
            cr->d.BglTrack.node= 0;
            cr->d.BglTrack.portnb= 0;
            cr->d.BglTrack.session= 0;
            cr->d.BglTrack.param[0]= '\0';
            MigrationTrackCount--;
            break;
        default:
            if (genetrack && !TrackNormLike)
            {   MigrToolTerminateMesg(cr, M_normal);
                MigrStopGet(cr->d.BglTrack);
            }
            else
            {   if (!strcmp((const char *)(
                    &(cr->d.BglTrack.param[0])),
                    (const char *)"Mutated"))
                    MigrToolTerminateMesg(cr, M_mutated);

                if (!MigrStopGet(cr->d.BglTrack))
                    MigrStartPut(cr->d.BglTrack);
                else
                    MigrToolTerminateMesg(cr, M_normal);
            }
            cr->d.BglTrack.node= 0;
            cr->d.BglTrack.portnb= 0;
            cr->d.BglTrack.session= 0;
            cr->d.BglTrack.param[0]= '\0';
    }
}


/* This function terminate one tracking for the cell cr
    with the status status */
void MigrToolTerminateMesg(cr, status)
Pcells cr;
MigrStatus status;
{   I32s  TimeNow;
    ComAddr cell_addr;

    TimeNow = (I32s) tietime(NULL);

    SelfID(cr, &cell_addr);
    /* init the data to send to beagle */
    tsprintf((char *)(&(BglMigrationDataBuf.
        localparameter[0])), "%.4d%3s",
        cr->d.gen.size, (GeneBnker)?
        (cr->d.gen.label):((I8s *)"---"));
    BglMigrationDataBuf.storelparam= 0;
    BglMigrationDataBuf.session= -cr->d.BglTrack.session;/* stop session */
    switch(status)
    {   case M_sleep :
            BglMigrationDataBuf.information= 'S';
            break;
        case M_apocalypse :
            BglMigrationDataBuf.information= 'A';
            break;
        default :
            BglMigrationDataBuf.information= 'n';
            break;
    }
    BglMigrationDataBuf.ranID= cell_addr.ranID;
    BglMigrationDataBuf.tierraTime= TimeNow;
    BglMigrationDataBuf.beagleTime= 0;
    BglMigrationDataBuf.fecundity= cr->d.fecundity;

    /* init data for connexion */
    BglMigrationConnectBuf.node    = cr->d.BglTrack.node;
    BglMigrationConnectBuf.portnb  = cr->d.BglTrack.portnb;
    BglMigrationConnectBuf.session = -cr->d.BglTrack.session;
    strcpy((char *)(&(BglMigrationConnectBuf.param[0])),
        (const char *)(&(cr->d.BglTrack.param[0])));

    MigrStopGet(cr->d.BglTrack);

    /* send data to beagle */
    TBglMigration();
}
#endif /* BGL */
#endif /* CLSTRSRVR */

/*
 * pr_inet - print array of in_addr type internet addresses
 *
 * listptr - pointer to list of addresses
 * length - unused
 *
 * returns: Node_id - last address printed as I32u
 *
 */
#ifndef DECVAX
I32u pr_inet(listptr, length)
I8s **listptr;
I32s length;
#else /* DECVAX */
I32u pr_inet(I8s ** listptr, I32s length)
#endif /* DECVAX */
{   struct in_addr *ptr;
    I32u Node_id=0;

    while ((ptr = (struct in_addr *) *listptr++) != NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "     Internet address: %s\n", inet_ntoa(*ptr));
        FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
        Node_id = ptr->s_addr;
    }
    return (Node_id);
}

/*
 * InIPMap - if given address/port is in IPMap array,
 *           returns index, else returns -1
 *
 * addr - given IP address and port
 * whichmap - 0 = IPMapC, 1 = IPMapS
 *
 * Returns: IP map index or -1 if given address is not present
 *
 */
I32s InIPMap(addr, whichmap)
    NetAddr addr;
    I32s  whichmap;
{   I32s index, NumNodes;
    MapNode  *IPMap;

    NumNodes=(whichmap)?NumNodesS:NumNodesC;
    IPMap=(whichmap)?IPMapS:IPMapC;
    for (index = 0; index < NumNodes; index++)
    {   if ((addr.node == IPMap[index].address.node)
            && (addr.portnb == IPMap[index].address.portnb))
            return index;
    }
    return -1;
}

/*
 * AddIPMap - add new node to IPMap array of MapNode structures
 *
 * new - new MapNode structure
 * whichmap - 0 = IPMapC, 1 = IPMapS
 *
 */
void AddIPMap(new, whichmap)
    MapNode new;
    I32s whichmap;
{   I32s siz=sizeof(MapNode);
    MapNode **IPMap;
    I32u *NumNodes;
#ifdef CLSTRSRVR
    Event msgevent;
    I32s newNumCells;
#endif /* CLSTRSRVR */

#ifdef CLSTRSRVR
    msgevent.i=0;
#endif /* CLSTRSRVR */

    IPMap=(whichmap)?(&IPMapS):(&IPMapC);
    NumNodes=(whichmap)?(&NumNodesS):(&NumNodesC);
    if(!(*IPMap))
    {   (*IPMap)=(MapNode *)tcalloc(1, siz, 1000);
        (*NumNodes)=0;
    }
    else
        (*IPMap)=(MapNode *)trecalloc((void *)(*IPMap),
            ((*NumNodes)+1)*siz, (*NumNodes)*siz, 1000);
#ifdef CLSTRSRVR
    if(!whichmap)
    {   newNumCells=NumCells+(new.data.NumCells);
        FecundityAvg=(newNumCells)?(((FecundityAvg*((double)NumCells))+
            ((((double)(new.data.FecundityAvg))/100.0)*
            ((double)(new.data.NumCells))))/((double)newNumCells)):0.0;
        FpAgeAvg=((FpAgeAvg*((double)(*NumNodes)))+((double)
            (new.data.AgeAvg)))/((double)((*NumNodes)+1));
        AgeAvg=(I32s)FpAgeAvg;
        Speed+=(new.data.Speed);
        NumCells+=(new.data.NumCells);
        SoupSize+=(new.data.SoupSize);
        msgevent.m=new.data.InstExec;
        AddEvent(&InstExe, &msgevent, &InstExe);
        msgevent.m=new.data.InstExecConnect;
        AddEvent(&InstExeConnect, &msgevent, &InstExeConnect);
    }
#endif /* CLSTRSRVR */
    (*IPMap)[*NumNodes]=new;
#ifndef CLSTRSRVR
#ifdef BGL
    if(TBgl_TPingS_Mode==1)
        TBglAddIPMap(*NumNodes, whichmap);
#endif /* BGL */
#endif /* CLSTRSRVR */
    (*NumNodes)++;
}

/*
 * RemoveIPMap - remove dead node from list
 *
 * dead - IP/port of node to be removed
 * whichmap - 0 = IPMapC, 1 = IPMapS
 *
 * Returns: >=0, no error
 *          < 0, error
 */
I32s RemoveIPMap(dead, whichmap)
    NetAddr dead;
    I32s whichmap;
{   I32u i, j=0, NumNodes;
    I32s index = -1;
#ifndef CLSTRSRVR
    I32s ar, ci, *ipi;
    Pcells tc;
#else /* CLSTRSRVR */
    Event msgevent;
    I32s newNumCells;
#endif /* CLSTRSRVR */
    MapNode  *IPMap, *new_map;

#ifdef CLSTRSRVR
    msgevent.i=0;
#endif /* CLSTRSRVR */

    NumNodes=(whichmap)?NumNodesS:NumNodesC;
    IPMap=(whichmap)?IPMapS:IPMapC;

    for(i=0;i<NumNodes;i++)
    {   if((IPMap[i].address.node==dead.node)&&
            (IPMap[i].address.portnb==dead.portnb))
        {   index=i;
            break;
        }
    }
    if(index>=0)
    {
#ifdef CLSTRSRVR
        if(!whichmap)
        {   newNumCells=NumCells-(IPMap[index].data.NumCells);

            FecundityAvg=(newNumCells)?(((FecundityAvg*NumCells)-
                ((((double)(IPMap[index].data.FecundityAvg))/100.0)*
                (IPMap[index].data.NumCells)))/((double)newNumCells)):0.0;

            FpAgeAvg=(NumNodes>1)?(((FpAgeAvg*
                ((double)NumNodes))-((double)
                (IPMap[index].data.AgeAvg)))/
                ((double)(NumNodes-1))):0.0;
            AgeAvg=(I32s)FpAgeAvg;

            Speed-=(IPMap[index].data.Speed);

            NumCells-=(IPMap[index].data.NumCells);

            SoupSize-=(IPMap[index].data.SoupSize);

            msgevent.m=IPMap[index].data.InstExec;
            SubEvent(&InstExe, &msgevent, &InstExe);

            msgevent.m=IPMap[index].data.InstExecConnect;
            SubEvent(&InstExeConnect, &msgevent, &InstExeConnect);
        }
#endif /* CLSTRSRVR */
        new_map=(MapNode *)tcalloc(NumNodes-1, sizeof(MapNode), 1019);
        for(i=0;i<NumNodes;i++)
            if((I32s)i!=index)
            {   new_map[j]=IPMap[i];
                j++;
            }
        tfree(IPMap, NumNodes*sizeof(MapNode), 1019);
        IPMap=new_map;
        NumNodes--;

#ifndef CLSTRSRVR
        for(ar=0;ar<NumCelAr;ar++)
            for(ci=0;ci<CelArSiz;ci++)
                if(ar||(ci>1))
                {   tc=&(cells[ar][ci]);
                    if(tc->ld)
                    {   ipi=(whichmap)?(&(tc->c.io.ipis)):(&(tc->c.io.ipic));
                        if((*ipi)>index)
                            (*ipi)--;
                        else
                            if((*ipi)==index)
                                (*ipi)=0;
                    }
                }
#ifdef BGL
        if(((whichmap)?(TBgl_TPingS_Mode):(TBgl_TPingC_Mode))==1)
            TBglRemoveIPMap(index, whichmap);
#endif /* BGL */
#endif /* CLSTRSRVR */

    }

    if(whichmap)
    {   NumNodesS=NumNodes;
        IPMapS=IPMap;
    }
    else
    {   NumNodesC=NumNodes;
        IPMapC=IPMap;
    }
    return index;
}

/*
 * GetIPMap - copy contents of MapFile to IPMap array
 *
 * fp - file descriptor for map file
 * whichmap - 0 = IPMapC, 1 = IPMapS
 *
 */
void GetIPMap(fp, whichmap)
    FILE *fp;
    I32s  whichmap;
{   MapNode current;
    I8s ReadBuf[80], *token;
    struct timeval now;

#ifdef __MWERKS__
    struct in_addr addressNode;
#endif /* __MWERKS__ */

    if (whichmap)
    {   if (IPMapS)
            tfree(IPMapS, NumNodesS * sizeof(MapNode), 1000);
        IPMapS = NULL;
    }
    else
    {   if (IPMapC)
            tfree(IPMapC, NumNodesC * sizeof(MapNode), 1000);
        IPMapC = NULL;
    }

    tgettimeofday(&now, NULL);
    while (tfgets(ReadBuf, sizeof(ReadBuf), fp) != NULL)
    {   token=(I8s *)strtok((char *)(&(ReadBuf[0])),
            (const char *)":");
        memset(&current, 0, sizeof(MapNode));
#ifdef __MWERKS__
        addressNode = inet_addr(strtok(NULL, ":"));
        current.address.node = addressNode.s_addr;
#else
#ifdef __SOLARIS2__
        {   char courrent_address_node_buf[64];
            tsprintf(courrent_address_node_buf, "%s", strtok(NULL, ":"));
            current.address.node = inet_addr(courrent_address_node_buf);
        }
#else /* __SOLARIS2__ */
        current.address.node=inet_addr((const char *)(token=
            (I8s *)strtok((char *)NULL, (const char *)":")));
#endif /* __SOLARIS2__ */
#endif /* __MWERKS__ */
        current.address.portnb=htons((I16u)atoi(
            (const char *)(token=(I8s *)strtok(
            (char *)NULL,(const char *)";"))));
        current.data.Fresh = now.tv_sec;
        current.data.Speed=200;
#ifndef CLSTRSRVR
        current.data.SoupSize=SoupSize;
#endif /* CLSTRSRVR */
        current.data.NumCells = 1;
        current.PrivPingData.subnet=(-1);
        AddIPMap(current, whichmap);
    }
}

/*
 * LdIPMapTPingDat - load IPMap array with TPing data
 *
 * whichmap - 0 = IPMapC, 1 = IPMapS
 */
void LdIPMapTPingDat(whichmap)
I32s whichmap;
{   FILE *filptr;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
    MapNode *IPMap;
    I32s NumNodes,i;
    I8s *map_fn;

    NumNodes=(whichmap)?NumNodesS:NumNodesC;
    map_fn=(whichmap)?(&(map_fnS[0])):(&(map_fnC[0]));
    tsprintf((char *)(&(Buff[0])),
        "%s%s.dat",GenebankPath,map_fn);
    if((filptr=tfopen(&(Buff[0]),"r")))
    {   for(i=0;i<NumNodes;i++)
        {   IPMap=(whichmap)?&(IPMapS[i]):&(IPMapC[i]);
            if(tfgets(&(Fbuf[0]),sizeof(Fbuf),filptr))
            {
#ifdef SIGBLOCK
                SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            
                sscanf(&(Fbuf[0]),"%d %d %d %d %d %d",
                    &(IPMap->data.FecundityAvg),
                    &(IPMap->data.Speed),
                    &(IPMap->data.NumCells),
                    &(IPMap->data.AgeAvg),
                    &(IPMap->data.SoupSize),
                    &(IPMap->data.InstExec));
                IPMap->data.Time=IPMap->data.Fresh;
                IPMap->data.InstExecConnect=IPMap->data.InstExec;
            
#ifdef SIGBLOCK
                TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
            }
            else
                break;
        }
        tfclose(filptr);
    }
}

/*
 * SelfID - fill ComAddr structure with id data for given cell
 *
 * cp - given cell
 * cell_addr - pointer to ComAddr structure
 *
 * Returns:
 */
void SelfID(cp, cell_addr)
void *cp;
ComAddr *cell_addr;
{   (cell_addr->ranID)=
#ifndef CLSTRSRVR
        ((Pcells)cp)?(((Pcells)cp)->d.RanID):(-1);
#else /* CLSTRSRVR */
        (-1);
#endif /* CLSTRSRVR */
    cell_addr->address.portnb=htons(LocalPort);
    cell_addr->address.node=LocalNetAddr.node;
}

/*
 * GetTPingData - fill out TPingData and TPrivPingData
 *                structures from own global variables
 *
 * data - pointer to TPingData structure
 * PrivPingData - pointer to TPrivPingData structure
 *
 */
void GetTPingData(data, PrivPingData, index)
TPingData *data;
TPrivPingData *PrivPingData;
I32s index;
{   struct timeval now;

    data->FecundityAvg = (I32s) (FecundityAvg * 100.0);
    data->Speed = Speed;
    data->NumCells = NumCells;
    data->AgeAvg = AgeAvg;
    data->SoupSize = SoupSize;  /* prepare information */
    data->InstExec = InstExe.m;
    data->InstExecConnect = InstExeConnect.m;
    data->OS = OpSys;
    tgettimeofday(&now, NULL);
    data->Time = data->Fresh = now.tv_sec;
    if (data->TransitTime == 0)
        data->TransitTime = ((now.tv_sec % 1000000L) * 1000)
            + (now.tv_usec / 1000);
    PrivPingData->subnet=TieSubNet;
    PrivPingData->apoctoggle=ApocToggle;
    PrivPingData->outcapupdtog=OutCapUpdTog;
#ifdef CLSTRSRVR
    (PrivPingData->remoutbndcap)=(index>=0)?
        (IPMapC[index].PrivPingData.remoutbndcap):0;
    PrivPingData->avgsize=0;
#else /* CLSTRSRVR */
    PrivPingData->remoutbndcap=0;
    PrivPingData->avgsize=AverageSize;
#endif /* CLSTRSRVR */
}

/*
 * TreatTPingData - update TPing data in IP map from incoming
 *                  message TPing data based on given node address/port
 *
 * mesg - incoming message
 * address - given IP address and port
 * reply - 0 - incoming message is not TPing reply
 *         1 - "        "       "  TPing reply
 * index - pointer to IPMap array index for TPing data
 */
void TreatTPingData(mesg, address, reply, index)
TMessage *mesg;
NetAddr *address;
I32s reply;
I32s *index;
{   I32s tnow;
    MapNode new;
    struct timeval now;
    MapNode  *IPMap;
#ifdef CLSTRSRVR
    Event msgevent, svInstExe, chgInstExe;
    I32s newNumCells;
    I32s svremoutbndcap;
#endif /* CLSTRSRVR */

#ifdef CLSTRSRVR
    msgevent.i=0;
#endif /* CLSTRSRVR */
    IPMap=(mesg->srcmap)?IPMapS:IPMapC;
    (*index)=InIPMap(*address, mesg->srcmap);
    if(((*index)>=0)||AutoAddIPMap)
    {   tgettimeofday(&now, NULL);
        mesg->PingData.Fresh=now.tv_sec;
        if(reply && mesg->PingData.TransitTime)
        {   tnow=((now.tv_sec%1000000L)*1000)+(now.tv_usec/1000);
            if(tnow<(I32s)mesg->PingData.TransitTime)
                tnow+=1000000000L;
            mesg->PingData.TransitTime=
                tnow-mesg->PingData.TransitTime;
        }
        else
            mesg->PingData.TransitTime=
                ((*index)!=(-1))?(IPMap[*index].
                data.TransitTime):0;
#ifndef CLSTRSRVR
        if(LocalClusterAddr.node)
        {   if((address->node==LocalClusterAddr.node)&&
                (address->portnb==LocalClusterAddr.portnb))
            {   if((mesg->PrivPingData.apoctoggle)!=ApocToggle)
                {   ApocalypseNow=1;
                    ApocToggle=mesg->PrivPingData.apoctoggle;
                    SrvrApocFlag=1;
                }
                if((mesg->PrivPingData.outcapupdtog)!=OutCapUpdTog)
                {   OutCapUpdTog=mesg->PrivPingData.outcapupdtog;
                    RemOutBndCap=mesg->PrivPingData.remoutbndcap;
                }
            }
        }
        SetLocalNodeSubNet(address, mesg->PrivPingData.subnet);
#endif /* CLSTRSRVR */
    }

    if((*index)>=0)
    {
#ifdef CLSTRSRVR
        if(!(mesg->srcmap))
        {   newNumCells=NumCells-
                (IPMap[*index].data.NumCells)+
                (mesg->PingData.NumCells);

            FecundityAvg=(newNumCells)?(((FecundityAvg*((double)NumCells))-
                ((((double)(IPMap[*index].data.FecundityAvg))/100.0)*
                ((double)(IPMap[*index].data.NumCells)))+
                ((((double)(mesg->PingData.FecundityAvg))/100.0)*
                ((double)(mesg->PingData.NumCells))))/
                ((double)(newNumCells))):0.0;

            FpAgeAvg=(NumNodesC)?((((
                FpAgeAvg*((double)NumNodesC))-
                ((double)(IPMap[*index].data.AgeAvg)))+
                ((double)(mesg->PingData.AgeAvg)))/
                ((double)NumNodesC)):0.0;
            AgeAvg=(I32s)FpAgeAvg;

            Speed=Speed-(IPMap[*index].data.Speed)+
                (mesg->PingData.Speed);

            NumCells=newNumCells;

            SoupSize=SoupSize-(IPMap[*index].data.SoupSize)+
                (mesg->PingData.SoupSize);

            svInstExe=InstExe;

            msgevent.m=IPMap[*index].data.InstExec;
            SubEvent(&InstExe, &msgevent, &InstExe);
            msgevent.m=mesg->PingData.InstExec;
            AddEvent(&InstExe, &msgevent, &InstExe);

            SubEvent(&InstExe, &svInstExe, &chgInstExe);
            if((SubNetCnt>1)&&(BasSubNetChgFrq))
            {   SubNetChgTime-=chgInstExe.m;
                SubNetChgTime=(SubNetChgTime<0)?0:SubNetChgTime;
            }
            if(SrvrApocOK)
                if(ApocalypseFreq)
                {   ApocalypseNowFreq-=chgInstExe.m;
                    ApocalypseNowFreq=(ApocalypseNowFreq<0)?
                        0:ApocalypseNowFreq;
                    if(!ApocalypseNowFreq)
                        ApocalypseNow=1;
                }
            msgevent.m=IPMap[*index].data.InstExecConnect;
            SubEvent(&InstExeConnect, &msgevent, &InstExeConnect);
            msgevent.m=mesg->PingData.InstExecConnect;
            AddEvent(&InstExeConnect, &msgevent, &InstExeConnect);
        }
#endif /* CLSTRSRVR */

        IPMap[*index].data=mesg->PingData;
#ifdef CLSTRSRVR
        svremoutbndcap=IPMap[*index].PrivPingData.remoutbndcap;
#endif /* CLSTRSRVR */
        IPMap[*index].PrivPingData=mesg->PrivPingData;
#ifdef CLSTRSRVR
        IPMap[*index].PrivPingData.remoutbndcap=svremoutbndcap;
#endif /* CLSTRSRVR */
        IPMap[*index].PendReq=0;
#ifndef CLSTRSRVR
#ifdef BGL
        if(((mesg->srcmap)&&(TBgl_TPingS_Mode==1))||
            ((!mesg->srcmap)&&(TBgl_TPingC_Mode==1)))
            TBglAlterIPMap(*index, mesg->srcmap);
#endif /* BGL */
#endif /* CLSTRSRVR */
    }
    else if(AutoAddIPMap)
    {   memset(&new, 0, sizeof(MapNode));
        new.address=*address;
        new.data=mesg->PingData;
        new.PrivPingData=mesg->PrivPingData;
        AddIPMap(new, mesg->srcmap);
        (*index)=((mesg->srcmap)?NumNodesS:NumNodesC)-1;
    }
#ifndef CLSTRSRVR
    if((!ClusterServerUp)&&(LocalClusterAddr.node)&&
        (address->node==LocalClusterAddr.node)&&
        (address->portnb==LocalClusterAddr.portnb))
        ClusterServerUp=1;
#endif /* CLSTRSRVR */
    return;
}

/*
 * UpdateOwnMapNode - update own node in IP Map or create it
 *                    if it does not exist
 */
void UpdateOwnMapNode()
{   I32s index;
    MapNode new;
    struct timeval now;
    MapNode  *IPMap;

#ifdef CLSTRSRVR
    IPMap=IPMapS;
    index=InIPMap(LocalNetAddr, 1);
#else /* CLSTRSRVR */
    IPMap=IPMapC;
    index=InIPMap(LocalNetAddr, 0);
#endif /* CLSTRSRVR */

    tgettimeofday(&now, NULL);
    if(index!=(-1))
    {   GetTPingData(&(IPMap[index].data),
            &(IPMap[index].PrivPingData), -1);
        IPMap[index].data.Fresh=now.tv_sec;
        IPMap[index].data.TransitTime = 0;
#ifdef BGL
        if (TBgl_TPingC_Mode == 1)
            TBglAlterIPMap(index, 0);
#endif /* BGL */
    }
    else
    {   memset(&new, 0, sizeof(MapNode));
        GetTPingData(&(new.data),&(new.PrivPingData), -1);
        new.address=LocalNetAddr;
        new.data.Fresh=now.tv_sec;
#ifdef CLSTRSRVR
        AddIPMap(new, 1);
#else /* CLSTRSRVR */
        AddIPMap(new, 0);
#endif /* CLSTRSRVR */
    }
}

/*
 * TPing_send - send a TPing message from cell with given
 *              CommAddr to Tierra node in IP map with given index
 *
 * index - given IP map index
 * cell_addr - given ComAddr of cell
 * whichmap - 0 = IPMapC, 1 = IPMapS
 *
 * Returns: 0 - success, -2 - failure
 */
I32s TPing_send(index, cell_addr, whichmap)
I32s index;
ComAddr *cell_addr;
I32s whichmap;
{   TMessage mesg;
    struct TieCommID dstcommid;
    MapNode *IPMap;
    I32s sufrembndcap;
    I32s rtnval;

    IPMap=(whichmap)?IPMapS:IPMapC;
    if ((IPMap[index].address.node)
        && (IPMap[index].address.portnb))
    {   memset((char *) &mesg, 0, sizeof(TMessage));
        mesg.info.tag = TPINGREQTAG;
        mesg.info.Data_u.TPingReq.source = *cell_addr;
        mesg.PingData.TransitTime = 0;
        mesg.dstmap=whichmap;

#ifdef CLSTRSRVR
        mesg.srcmap=1;
        GetTPingData(&(mesg.PingData), &(mesg.PrivPingData),
            (whichmap)?(-1):index);
#else /* CLSTRSRVR */
        mesg.srcmap=0;
        GetTPingData(&(mesg.PingData), &(mesg.PrivPingData), -1);
#endif /* CLSTRSRVR */
        
        dstcommid.tiec_node=IPMap[index].address.node;
        dstcommid.tiec_port=IPMap[index].address.portnb;
        if (!TieCommSend(&globtcibuf, &mesg, dstcommid, &sufrembndcap))
            rtnval=0;
        else
            rtnval=(-2);
    }
    else
        rtnval=(-2);

    return rtnval;
}

/*
 * TPing_reply - reply with TPing data to messages of the following types:
 *                  SENDADDRTAG, EMIGRATETAG, TPINGREQTAG
 * mesg_recv - pointer to received message
 * msgsrc - pointer to NetAddr structure with message source addr
 *
 * returns: 0 - reply successful, reply failed
 */
I32s TPing_reply(mesg_recv, index, msgsrc)
TMessage *mesg_recv;
I32s index;
NetAddr *msgsrc;
{   TMessage mesg_send;
    struct TieCommID dstcommid;
    I32s sufrembndcap;

    memset((char *) &mesg_send, 0, sizeof(TMessage));
    mesg_send.info.tag = TPINGREPTAG;

    mesg_send.dstmap=mesg_recv->srcmap;

#ifdef CLSTRSRVR
    mesg_send.srcmap=1;
    GetTPingData(&(mesg_send.PingData), &(mesg_send.PrivPingData),
        (mesg_send.dstmap)?(-1):index);
#else /* CLSTRSRVR */
    mesg_send.srcmap=0;
    GetTPingData(&(mesg_send.PingData), &(mesg_send.PrivPingData), -1);
#endif /* CLSTRSRVR */

    SelfID(NULL, &mesg_send.info.Data_u.TPingRep.source);
    if (mesg_recv->info.tag == SENDADDRTAG)
    {   mesg_send.info.Data_u.TPingRep.dest.address = *msgsrc;
        mesg_send.info.Data_u.TPingRep.dest.ranID = -1;
    }
    if (mesg_recv->info.tag == EMIGRATETAG)
    {   mesg_send.info.Data_u.TPingRep.dest.address = *msgsrc;
        mesg_send.info.Data_u.TPingRep.dest.ranID = -1;
    }
    if (mesg_recv->info.tag == TPINGREQTAG)
        mesg_send.info.Data_u.TPingRep.dest =
            mesg_recv->info.Data_u.TPingReq.source;
    mesg_send.PingData.TransitTime = mesg_recv->PingData.TransitTime;

    switch(mesg_recv->info.tag)
    {   case SENDADDRTAG:
            dstcommid.tiec_port=msgsrc->portnb;
            dstcommid.tiec_node=msgsrc->node;
            break;
        case EMIGRATETAG:
            dstcommid.tiec_port=msgsrc->portnb;
            dstcommid.tiec_node=msgsrc->node;
            break;
        case TPINGREQTAG:
            dstcommid.tiec_port=mesg_recv->info.
                Data_u.TPingReq.source.address.portnb;
            dstcommid.tiec_node=mesg_recv->info.
                Data_u.TPingReq.source.address.node;
            break;
        default:
            break;
    }
    if (!TieCommSend(&globtcibuf, &mesg_send, dstcommid, &sufrembndcap))
        return 0;
    else
        return (-1);
}

/*
 * Addr_send - send local name, address and port number to all
 *             nodes in selected IPMap
 *
 * whichmap - selected IPMap 0 = IPMapC(local cluster), 
 *                           1 = IPMapS(cluster servers)
 *
 * Returns: 0 = send sucessful, -1 = send failed
 */
I32s Addr_send(whichmap)
I32s whichmap; /* 0 = IPMapC, 1 = IPMapS */
{   I32s status = 0;
    I32s i, NumNodes;
    MapNode  *IPMap;

#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {   FEClrmsg(MSG_Y,
            ((OPSYS == BCDOS) || (OPSYS == DJGPPDOS) ||
            ((OPSYS == AMIGADOS) && (FRONTEND == BASIC))
            || (OPSYS == MACTIE) || (OPSYS == WIN32TIE) ?(I8s *)
"                       Press Any Key for Menu ...                           "
            :(I8s *)
"                    Press Interrupt Key for Menu ...                        "
        ));
        FEStats();
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
    NumNodes=(whichmap)?NumNodesS:NumNodesC;
    IPMap=(whichmap)?IPMapS:IPMapC;
    for (i = 0; i < NumNodes; i++)/* send stream to every site in IPMap */
    {   if(((IPMap[i].address.node)!=(LocalNetAddr.node))||
            ((IPMap[i].address.portnb)!=(LocalNetAddr.portnb)))
        {
#ifdef CLSTRSRVR
            if(Addr_send_mesg(IPMap[i].address, whichmap, i))
#else /* CLSTRSRVR */
            if(Addr_send_mesg(IPMap[i].address, whichmap, -1))
#endif /* CLSTRSRVR */
                status=-1;
        }
    }
#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {   FEClrmsg(MSG_Y,
            ((OPSYS == BCDOS) || (OPSYS == DJGPPDOS) ||
            ((OPSYS == AMIGADOS) && (FRONTEND == BASIC))
            || (OPSYS == MACTIE) || (OPSYS == WIN32TIE) ?(I8s *)
"                       Press Any Key for Menu ...                           "
            :(I8s *)
"                    Press Interrupt Key for Menu ...                        "
        ));
        FEStats();
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
    return status;
}

/*
 * Addr_send_mesg - send local name, address and port number to
 *                  given destination
 *
 * whichmap - IPMap containing given destination
 *            0 = IPMapC(local cluster),
 *            1 = IPMapS(cluster servers)
 *
 * Returns: 0 = send sucessful, 1 = send failed
 */
I32s Addr_send_mesg(destaddr, whichmap, index)
NetAddr destaddr;
I32s whichmap, index;
{   I32s len, status = 0, sufrembndcap;
    TMessage mesg;
    struct TieCommID dstcommid;

    memset(&mesg, 0, sizeof(TMessage));
    mesg.info.tag=SENDADDRTAG;  /* compose XDR stream */
    len=strlen((const char *)(&(LocalHostName[0])))+1;
    mesg.info.Data_u.Addr.name.name_len=len;
    mesg.info.Data_u.Addr.name.name_val=
        (I8s *) tcalloc(len, sizeof(I8s), 1004);
    tsprintf((char *)(&(mesg.info.Data_u.Addr.
        name.name_val[0])), "%s", LocalHostName);
    (mesg.info.Data_u.Addr.name.name_val)[len-1]='\0';
    mesg.PingData.TransitTime=0;
    mesg.dstmap=whichmap;

#ifdef CLSTRSRVR
    mesg.srcmap=1;
    GetTPingData(&(mesg.PingData), &(mesg.PrivPingData),
        (whichmap)?(-1):index);
#else /* CLSTRSRVR */
    mesg.srcmap=0;
    GetTPingData(&(mesg.PingData), &(mesg.PrivPingData), -1);
#endif /* CLSTRSRVR */

    if(((destaddr.node)!=(LocalNetAddr.node))||
        ((destaddr.portnb)!=(LocalNetAddr.portnb)))
    {   dstcommid.tiec_node=destaddr.node;
        dstcommid.tiec_port=destaddr.portnb;
        RemotPort=ntohs(dstcommid.tiec_port);
        if((destaddr.node)&&(destaddr.portnb))
            status=TieCommSend(&globtcibuf, &mesg, dstcommid, &sufrembndcap);
    }
    tfree(mesg.info.Data_u.Addr.name.name_val,len*sizeof(I8s), 1004);
    return status;
}

/*
 * Addr_recv - process incoming message of type SENDADDRTAG
 *             (address message)
 *
 * mesg - pointer to incoming address message
 *
 * Returns: 0 - added to IPMap
 *          1 - already in IPMap
 */
I32s Addr_recv(mesg,msgsrc)
TMessage *mesg;
NetAddr *msgsrc;
{   MapNode recv_node;
    struct in_addr recv_addr;
    I32s status = 0;

    recv_node.address=*msgsrc;
    memset((I8s *) & recv_node.data, 0, sizeof(recv_node.data));
    recv_addr.s_addr = recv_node.address.node;

#ifdef TIERRA
#if FRONTEND == BASIC
    if(!HistoInEffect())
    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */

        tsprintf((char *)(&(Fbuf[0])), "Node address %s:%s:%hu received\n",
            mesg->info.Data_u.Addr.name.name_val, inet_ntoa(recv_addr),
            ntohs(recv_node.address.portnb));
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));

#ifdef TIERRA
#ifdef BGL
        TBgl_make_femessages(&(Fbuf[0]),
            strlen((const char *)((const char *)(&(Fbuf[0])))));
#endif /* BGL */
#if FRONTEND == BASIC
    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */

    if (InIPMap(recv_node.address, mesg->srcmap) == -1)
    {   if(AutoAddIPMap)
            AddIPMap(recv_node, mesg->srcmap);
    }
    else
    {
#ifdef TIERRA
#ifdef BGL
            TBgl_make_femessages(&(Fbuf[0]),
                strlen((const char *)(
                    (const char *)(&(Fbuf[0])))));
#endif /* BGL */
#endif /* TIERRA */
        status = 1;
    }
    return status;
}

/*
 * Bye_send - signal to every known node in IPMapC that the local Tierra
 *            node is going down (BYETAG)
 *
 * whichmap - 0 = IPMapC, 1 = IPMapS
 *
 * Returns: 0
 */
I32s Bye_send(whichmap)
I32s whichmap; /* 0 = IPMapC, 1 = IPMapS */
{   I32s i,NumNodes;
    MapNode *IPMap;

    NumNodes=(whichmap)?NumNodesS:NumNodesC;
    IPMap=(whichmap)?IPMapS:IPMapC;
    for(i=0; i<NumNodes; i++)
#ifdef CLSTRSRVR
        Bye_send_mesg(IPMap[i].address, whichmap, i);
#else /* CLSTRSRVR */
        Bye_send_mesg(IPMap[i].address, whichmap, -1);
#endif /* CLSTRSRVR */
    return 0;
}

/*
 * Bye_send_mesg - build and send Bye message to single destination
 *
 * destaddr - destination address
 * whichmap - which IPMap destination was picked from
 *            0 = IPMapC, 1 = IPMapS
 * Returns: 0
 */
I32s Bye_send_mesg(destaddr, whichmap, index)
NetAddr destaddr;
I32s whichmap; /* 0 = IPMapC, 1 = IPMapS */
I32s index;
{   TMessage mesg;
    struct TieCommID dstcommid;
    I32s sufrembndcap;

    mesg.info.tag=BYETAG;
    mesg.PingData.TransitTime = 0;
    mesg.dstmap=whichmap;

#ifdef CLSTRSRVR
    mesg.srcmap=1;
    GetTPingData(&(mesg.PingData), &(mesg.PrivPingData),
        (whichmap)?(-1):index);
#else /* CLSTRSRVR */
    mesg.srcmap=0;
    GetTPingData(&(mesg.PingData), &(mesg.PrivPingData), -1);
#endif /* CLSTRSRVR */

    if((destaddr.node)&&(destaddr.portnb))
        if(((destaddr.node)!=(LocalNetAddr.node))||
            ((destaddr.portnb)!=(LocalNetAddr.portnb)))
        {   dstcommid.tiec_node=destaddr.node;
            dstcommid.tiec_port=destaddr.portnb;
            TieCommSend(&globtcibuf, &mesg, dstcommid, &sufrembndcap);
        }
    return 0;
}

/*
 * Bye_recv - remove a node from IPMap when receiving a BYETAG message from it
 *
 * mesg - Bye message received
 *
 * Returns: >=0 node removed
 *          < 0 node not removed
 */
I32s Bye_recv(mesg,msgsrc)
TMessage *mesg;
NetAddr *msgsrc;
{   NetAddr dead;
    struct in_addr source;
    I32s dropok,rtnval;

    dropok=1;
#ifndef CLSTRSRVR
    if(LocalClusterAddr.node)
        if(msgsrc->node==LocalClusterAddr.node)
            if(msgsrc->portnb==LocalClusterAddr.portnb)
            {   dropok=0;
                ClusterServerUp=0;
            }
#endif /* CLSTRSRVR */

    if(dropok)
    {   source.s_addr=msgsrc->node;
        if(AutoRemIPMap)
        {
#ifdef TIERRA
#if FRONTEND == BASIC
            if(!HistoInEffect())
            {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                tsprintf((char *)(&(Fbuf[0])),
                    "Node %s:%hu leaving the net\n",
                    inet_ntoa(source),
                    ntohs(msgsrc->portnb));
                FEPrintf(MSG_X, MSG_Y, 0, 0,
                    (char *)(&(Fbuf[0])));
#ifdef BGL
                TBgl_make_femessages(&(Fbuf[0]),
                    strlen((const char *)(&(Fbuf[0]))));
#endif /* BGL */
#ifdef TIERRA
#if FRONTEND == BASIC
            }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
            dead=*msgsrc;
            rtnval=RemoveIPMap(dead, mesg->srcmap);
        }
        else
            rtnval=-1;
    }
    else
        rtnval=-1;
    return rtnval;
}

/*
 * CleanIPMap - zero listed speeds for nodes out of contact over time limit
 *              set in SpdZeroTime, and delete nodes out of contact
 *              over time limit setin PendReqTime
 *
 * Returns: none
 */
void CleanIPMap(whichmap)
I32s whichmap;
{   struct timeval now;
    struct in_addr source;
    MapNode *IPMap=(whichmap)?IPMapS:IPMapC;
    I32s NumNodes=(whichmap)?NumNodesS:NumNodesC;
#ifdef TIERRA
    I32s *Pipmap=(whichmap)?(&PipmapS):(&PipmapC);
#endif /* TIERRA */
    I32s i, prop;

    tgettimeofday(&now, NULL);

#ifdef TIERRA
    prop=((InstExe.i)*NumNodes)/1000000L;
    if(prop>=(I32s)NumNodes)
        prop=0;
    if((*Pipmap)>=(I32s)NumNodes)
        (*Pipmap)=0;
    if(prop==(*Pipmap))
        return;
    i=(*Pipmap);
#else /* TIERRA */
    prop=NumNodes;
    i=0;
#endif /* TIERRA */

    while(i!=prop)
    {   if((((IPMap[i].address.node)!=(LocalNetAddr.node)))||
          (((IPMap[i].address.portnb)!=(LocalNetAddr.portnb))))
        {
#ifdef TIERRA
            if(Remot_from_IPMap(i,whichmap)==0)
            {
#endif /* TIERRA */
                if((((I32s)((now.tv_sec)-(IPMap[i].
                    data.Fresh)))>=PendReqTime)&&(PendReqTime))
                {   source.s_addr = IPMap[i].address.node;
#ifdef TIERRA
                    if((whichmap)&&(((IPMap[i].address.node)!=
                        (LocalClusterAddr.node))||
                        ((IPMap[i].address.portnb)!=
                        (LocalClusterAddr.portnb))))
                    {
#endif /* TIERRA */
                        if(AutoRemIPMap)
                        {
#ifdef TIERRA
#if FRONTEND == BASIC
                            if(!HistoInEffect())
                            {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
#ifdef CLSTRSRVR
                                if(!(IPMap[i].data.Speed))
                                {
#endif /* CLSTRSRVR */
                                    tsprintf((char *)(&(Fbuf[0])),
                                        "Node %s:%hu leaving the net\n",
                                        inet_ntoa(source),
                                        ntohs(IPMap[i].address.portnb));
                                    FEPrintf(MSG_X,MSG_Y,0,0,
                                        (char *)(&(Fbuf[0])));
#ifdef BGL
                                    TBgl_make_femessages(&(Fbuf[0]),
                                        strlen((const char *)
                                        ((const char *)(&(Fbuf[0])))));
#endif /* BGL */
#ifdef CLSTRSRVR
                                }
#endif /* CLSTRSRVR */
#ifdef TIERRA
#if FRONTEND == BASIC
                            }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
#ifdef CLSTRSRVR
                            if(!(IPMap[i].data.Speed))
                            {
#endif /* CLSTRSRVR */
                                if(RemoveIPMap(IPMap[i].
                                    address,0)>=0)
                                {   NumNodes=(whichmap)?
                                        NumNodesS:NumNodesC;
                                    IPMap=(whichmap)?IPMapS:IPMapC;
                                    if(prop>=(I32s)NumNodes)
                                        prop=0;
                                    if(i>=(I32s)NumNodes)
                                        i=0;
                                }
                                else
                                    i++;
#ifdef CLSTRSRVR
                            }
                            else
                                i++;
#endif /* CLSTRSRVR */
                        }
                        else
                            i++;
#ifdef TIERRA
                    }
                    else
                        i++;
#endif /* TIERRA */
                }
                else
                {   if((((I32s)((now.tv_sec)-(IPMap[i].
                        data.Fresh)))>=SpdZeroTime)&&(SpdZeroTime))
                    {
#ifdef CLSTRSRVR
                        if(!whichmap)
                            Speed-=(IPMap[i].data.Speed);
#else /* CLSTRSRVR */
                        IPMap[i].PrivPingData.subnet=(-1);
#endif /* CLSTRSRVR */
                        IPMap[i].data.Speed=0;
                    }
                    i++;
                }
#ifdef TIERRA
            }
            else
                i++;
#endif /* TIERRA */
        }
        else
            i++;
        if(i>(I32s)NumNodes)
            i=0;
    }
#ifdef TIERRA
    (*Pipmap)=i;
#endif /* TIERRA */
}

#ifndef _WIN32
#ifndef AMIGA
/*
 * open_max - get maximum number of socket descriptors available
 *
 * Returns: maximum number of socket descriptors available
 */
I32s open_max()
{
#ifdef OPEN_MAX
    I32s openmax = OPEN_MAX;
#else
    I32s openmax = 0;
#endif
    if(openmax == 0)
    {  /* first time through */
        if((openmax = sysconf(_SC_OPEN_MAX)) < 0)
        {   if(errno == 0)
                openmax = OPEN_MAX_GUESS;   /* It's indeterminate */
            else
            {   tsprintf((char *)(&(Fbuf[0])),
                    "sysconf error for _SC_OPEN_MAX");
                FEError(-1019, EXIT, WRITE, (char *)(&(Fbuf[0])));
            }
        }
    }
    return (openmax);
}
#endif /* AMIGA */
#endif /* _WIN32 */

/*
 * SockBlkCtl - set socket for blocking/non-blocking I/O
 *
 * s - socket descriptor
 * blksktflg - 0=non-blocking I/O, 1=blocking I/O
 *
 * Returns: 0=success -1=error
 */
I32s SockBlkCtl(s, blksktflg)
int s;
I32s blksktflg;
{
    I32s rtnval;
#ifdef _WIN32
    unsigned long dontblock;
#else /* _WIN32 */
#ifdef AMIGA
    unsigned long dontblock;
#else /* AMIGA */
#ifdef __MWERKS__
    unsigned long dontblock;
#else /* __MWERKS__ */
    int sktflg;
#endif /* __MWERKS__ */
#endif /* AMIGA */
#endif /* _WIN32 */

#ifdef SIGBLOCK
    sigset_t SigIOSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigIOSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef _WIN32
    dontblock=(blksktflg)?0:1;
    rtnval=(!ioctlsocket(s, (unsigned long)
        FIONBIO, &dontblock))?0:(-1);
#else /* _WIN32 */
#ifdef AMIGA
    dontblock=(blksktflg)?0:1;
    IoctlSocket(s, (unsigned long)
        FIONBIO, (char *) &dontblock);
    rtnval=0;
#else /* AMIGA */
#ifdef __MWERKS__
    dontblock=(blksktflg)?0:1;
    ioctl(s, FIONBIO, &dontblock);
    rtnval=0;
#else /* __MWERKS__ */
    if((sktflg=tfcntl(s, F_GETFL, 0))>=0)
    {   if(blksktflg)
            sktflg&=(~((int)O_NONBLOCK));
        else
            sktflg|=((int)O_NONBLOCK);
        rtnval=tfcntl(s, F_SETFL, sktflg);
    }
    else
        rtnval=-1;
#endif /* __MWERKS__ */
#endif /* AMIGA */
#endif /* _WIN32 */

#ifdef SIGBLOCK
    TSigRelease(SigIOSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

#if defined(TIERRA)
/*
 * EmigSubNetChk - Emigration subnet check
 *
 * srcsubnet - source subnet
 * dstsubnet - destination subnet
 * dstmap - 0 = IPMapC, 1 = IPMapS for destination IPMap
 *
 * Returns: 0 = emigration denied
 *          1 = emigration permitted
 */
I32s EmigSubNetChk(srcsubnet, dstsubnet, dstmap)
I32s srcsubnet, dstsubnet, dstmap;
{   I32s subnetpop, sbntnodct,subnetavgsiz;
    subnetpop=CalcSubNetPop(dstmap,dstsubnet,
        &sbntnodct,&subnetavgsiz);
    return((SubNetCnt>1)?((dstsubnet>=0)?
        (((sbntnodct>0)&&((subnetpop<=
        (NumCellsMin*sbntnodct))||
        (dstsubnet==srcsubnet)))?1:0):0):1);
}

/*
 * CalcSubNetPop - calculate subnet population
 *
 * whichmap - 0 = IPMapC, 1 = IPMapS
 * subnet - subnet for which to calculate total population
 * sbntnodct - pointer to node count of subnet
 *
 * Returns:  subnetpop - total subnet population
 *          *sbntnodct - node count of subnet
 */
I32s CalcSubNetPop(whichmap, subnet, sbntnodct, subnetavgsiz)
I32s whichmap, subnet, *sbntnodct,*subnetavgsiz;
{   I32s i, subnetpop=0;
    I32s NumNodes=(whichmap)?NumNodesS:NumNodesC;
    MapNode *IPMap=(whichmap)?(&(IPMapS[0])):(&(IPMapC[0]));

    (*sbntnodct)=0;
    (*subnetavgsiz)=0;
    for(i=0;i<NumNodes;i++)
        if((IPMap[i].PrivPingData.subnet)==(subnet))
        {   subnetpop+=IPMap[i].data.NumCells;
            (*subnetavgsiz)+=IPMap[i].PrivPingData.avgsize;
            (*sbntnodct)++;
        }
    (*subnetavgsiz)=(*sbntnodct)?((*subnetavgsiz)/(*sbntnodct)):0;
    return subnetpop;
}
#endif /* defined(TIERRA) */

/*
 * WriteTPingDat - write Map TPing data to TPing log
 *
 *     whichmap - 0 - local node map
 *                1 - cluster server map
 */
void WriteTPingDat(whichmap)
I32s  whichmap;
{   I32s i, NumNodes;
    struct in_addr addr;
    struct timeval now;
    MapNode  *IPMap;

    if (whichmap)
    {   NumNodes = NumNodesS;
        IPMap = IPMapS;
    }
    else
    {   NumNodes = NumNodesC;
        IPMap = IPMapC;
    }

    tgettimeofday(&now, NULL);
    tfprintf(ping_fp, "\nie = %d,%d  Time = %lu\n",
        InstExe.m, InstExe.i, now.tv_sec);
    for (i = 0; i < NumNodes; i++)
    {   addr.s_addr = IPMap[i].address.node;
        tfprintf(ping_fp, "%s:%u:%d:%d:%d:%u:%u:%d:%d:%d\n",
            inet_ntoa(addr),
            IPMap[i].data.Fresh,
            IPMap[i].data.Speed,
            IPMap[i].data.NumCells,
            IPMap[i].data.SoupSize,
            IPMap[i].data.TransitTime,
            IPMap[i].data.Time,
            IPMap[i].data.InstExec,
            IPMap[i].data.InstExecConnect,
            IPMap[i].data.OS
            );
    }
}
#endif /* NET */
