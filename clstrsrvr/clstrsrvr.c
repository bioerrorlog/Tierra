#include "tierra.h"
#define EXTERN
#include "globals.h"
#include "clstrsetup.h"

static int SrvrProcTierraMsg P_((TMessage *mesg,NetAddr *msgsrc,I32u msgid));
extern void clstrsrvrsetup P_((I32s argc, I8s * argv[]));

int main(argc, argv)
int argc;
char *argv[];
{   int errflag=0; 
    int i, totage, actnodecnt;
    I32s lstclsipsupdtme, curtme, updintrvl;
#ifdef __MWERKS__
#if FRONTEND == STDIO  /* get command line and parse it if on Mac */
    I8s *p1 =
    "\nPlease input arguments for tierra separated by spaces:\n\ntierra ";
#endif /* FRONTEND == STDIO */
#endif /* __MWERKS__ */
#ifdef SIGBLOCK
    sigset_t SigIOSaveSet;
#endif /* SIGBLOCK */
   
#ifdef SIGBLOCK
    SetSigSet();
#endif /* SIGBLOCK */

    tiestrtme=tietime(NULL);
    lstclsipsupdtme=tiestrtme;

#ifdef __MWERKS__
#if FRONTEND == STDIO  /* get command line and parse it if on Mac */
    I8s *p1 =
    "\nPlease input arguments for tierra separated by spaces:\n\ntierra ";
    tfprintf(stdout, p1);
    argc = cmdline("tierra ", &argv);
#else /* FRONTEND == STDIO */
    SIOUXSettings.standalone = FALSE;
    SIOUXSettings.setupmenus = FALSE;
    SIOUXSettings.initializeTB = FALSE;
    SIOUXSettings.showstatusline = FALSE;
    SIOUXSettings.autocloseonquit = FALSE;
    SIOUXSettings.asktosaveonclose = FALSE;
    SIOUXSettings.rows = 10;
    SIOUXSettings.columns = 80;
    SIOUXSettings.toppixel = windowHeight + 64;
    SIOUXSettings.leftpixel = 5;
    FEInitToolbox();
    Alert(128, nil);
    argc = ccommand(&argv);
#ifdef NET
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif /* NET */
    wp = FECreateNewWindow();
#endif /* FRONTEND == STDIO */
#endif /* __MWERKS__ */

    initvar();
    TieCommInit(&globtcibuf);
    FEStartup();
    clstrsrvrsetup(argc, argv);
    if (argc > 2)
        FEMenu();
#if FRONTEND == BASIC
    else
    {
#if INTERFACE != BASICW
#ifndef __linux__
#ifndef DJGPP
        ttouchwin();
#endif /* DJGPP */
#endif /* __linux__ */
#ifndef DJGPP
        trefresh();
#endif /* DJGPP */
#endif /* INTERFACE != BASICW */

        FEVidInit((I32s)0);
        srvrFEPlan(0);
    }
#endif /* FRONTEND == BASIC */
    for(;(!errflag);)
    {   
#ifdef AMIGA
#if FRONTEND == STDIO
        chkabort();
#endif /* FRONTEND == STDIO */
#endif /* AMIGA */

        if (KEYHIT())
            FEMenu();
        else
        {   srvactivity=0;
            if(OnLineStat)
                TieCommProcMsgBkLog(&globtcibuf, SrvrProcTierraMsg);

            if((SubNetCnt>1)&&(BasSubNetChgFrq))
                if(SubNetChgTime<=0)
                    TieCommChgSubNet(TieSubNet,0,0,0);
                else
                    UpdateOwnMapNode();
            else
                UpdateOwnMapNode();

            if(!SrvrApocOK)
                if((tietime(NULL)-tiestrtme)>FirstSrvrApocWait)
                    SrvrApocOK=1;

            if(ApocalypseNow)
            {   ApocalypseNow=0;
                ApocToggle=(ApocToggle)?0:1;
                IPMapTPing_send(0);
                TieSleep(0, SrvrApocSleep);
                ApocalypseNowFreq=((I32s)
                    (ApocFixFrac*((double)ApocalypseFreq)))+
                    (tlrand()%(((I32s)(ApocRanFrac*
                    ((double)ApocalypseFreq)))+1));
            }
            curtme=tietime(NULL);
            updintrvl=curtme-lstclsipsupdtme;
            if(updintrvl>=ClsIPSUpdInt)
            {   CleanIPMap(0);
                CleanIPMap(1);
                IPMapTPing_send(0);
                IPMapTPing_send(1);
                Map_send(1, 0);
                for (i = 0; i < 6; i++)
                {   FEMonCom[i].SNumMess=
                        (MonCom[i].SNumMess)/updintrvl;
                    FEMonCom[i].SNumPack=
                        (MonCom[i].SNumPack)/updintrvl;
                    FEMonCom[i].SNumByte=
                        (MonCom[i].SNumByte)/updintrvl;
                    FEMonCom[i].RNumMess=
                        (MonCom[i].RNumMess)/updintrvl;
                    FEMonCom[i].RNumPack=
                        (MonCom[i].RNumPack)/updintrvl;
                    FEMonCom[i].RNumByte=
                        (MonCom[i].RNumByte)/updintrvl;
                }
                if(NumNodesC)
                {   for(actnodecnt=totage=i=0;i<(I32s)NumNodesC;i++)
                        if(IPMapC[i].data.Speed>0)
                        {   totage+=IPMapC[i].data.AgeAvg;
                            actnodecnt++;
                        }
                    FpAgeAvg=(actnodecnt>0)?(((double)totage)/
                        ((double)actnodecnt)):0.0;
                }
                else
                    FpAgeAvg=0.0;
                AgeAvg=(I32s)FpAgeAvg;
                FEBanner();
                FEStats();
                srvrFEPlan(1);
                for (i = 0; i < 6; i++)
                {   MonCom[i].SNumMess = MonCom[i].SNumPack =
                        MonCom[i].SNumByte =
                    MonCom[i].RNumMess = MonCom[i].RNumPack =
                        MonCom[i].RNumByte = 0;
                }
                if (PingDatLog)
                {   WriteTPingDat(0);
                    WriteTPingDat(1);
                }
                lstclsipsupdtme=curtme;
            }

            if(!srvactivity)
            {   if(ServerSleep)
#ifdef _WIN32
                    Sleep(ServerSleep*1000);
#else /* _WIN32 */
                    sleep(ServerSleep);
#endif /* _WIN32 */
            }
        }
    }
    if(tfp_log)
    {   tfclose(tfp_log);
        tfp_log=NULL;
    }
    if(ping_fp)
    {   tfclose(ping_fp);
        ping_fp=NULL;
    }
    TierraExit(errflag);
    return errflag;
}

/******************************************************************************
 * clstrinitvar
 ******************************************************************************/
void clstrinitvar()
{   RemOutBndCap=Speed=LstOutBndCapRst=srvactivity=SrvrApocOK=0;
    FecundityAvg=FpAgeAvg=0.0;
}

/******************************************************************************
 * SrvrProcTierraMsg
 ******************************************************************************/
static int SrvrProcTierraMsg(mesg,msgsrc,msgid)
TMessage *mesg;
NetAddr *msgsrc;
I32u msgid;
{   I32s result, dstidx, tpingreplyrtn, index, sufrembndcap;
    NetAddr address;
    struct TieCommID dstcommid;

    srvactivity=1;
    switch ((int) mesg->info.tag)
    {   case SENDADDRTAG:
        {   Addr_recv(mesg,msgsrc);
            TreatTPingData(mesg, msgsrc, 0, &index);
            if(index>=0)
                TPing_reply(mesg, index, msgsrc);
            break;
        }
        case EMIGRATETAG:
        {   if(mesg->PrivPingData.subnet==TieSubNet)
            {   if((!(mesg->srcmap))||(NumNodesC>0))
                {   TreatTPingData(mesg, msgsrc, 0, &index);
                    if(index>=0)
                    {   TPing_reply(mesg, index, msgsrc);
                        if(mesg->srcmap)
                        {   dstidx=tlrand()%NumNodesC;
                            mesg->info.Data_u.Creature.dest=
                                IPMapC[dstidx].address;
                            mesg->dstmap=0;
                        }
                        else
                        {   mesg->srcmap=1;
                            GetTPingData(&(mesg->PingData), 
                                &(mesg->PrivPingData), index);
                        }
                        dstcommid.tiec_node=mesg->info.
                            Data_u.Creature.dest.node;
                        dstcommid.tiec_port=mesg->info.
                            Data_u.Creature.dest.portnb;
                        if(!TieCommSend(&globtcibuf, mesg,
                            dstcommid, &sufrembndcap))
                        {   tsprintf(Fbuf,
                                "%s:Send Genome %.4d to %02d.%02d.%02d.%02d\n",
                                LocalHostName, 
                                mesg->info.Data_u.Creature.genome_siz,
                                ((I8u *)(&(dstcommid.tiec_node)))[0],
                                ((I8u *)(&(dstcommid.tiec_node)))[1],
                                ((I8u *)(&(dstcommid.tiec_node)))[2],
                                ((I8u *)(&(dstcommid.tiec_node)))[3]);
                            FEPrintf(MSG_X, MSG_Y, 0, 0, &(Fbuf[0]));
                        }
                    }
                }
            }
            break;
        }
        case TPINGREQTAG:
        {   address = mesg->info.Data_u.TPingReq.source.address;
            TreatTPingData(mesg, &address, 0, &index);
            if(index>=0)
            {   tpingreplyrtn=TPing_reply(mesg, index, msgsrc);
                if (tpingreplyrtn < 0)
                {   tsprintf((char *)(&(Fbuf[0])),
                        "Tping request could not be processed\n");
                    FEPrintf(MSG_X, MSG_Y, 0, 0,&(Fbuf[0]));
                }
                else
                {   tsprintf((char *)(&(Fbuf[0])),
                        "TPing request processed\n");
                    FEPrintf(MSG_X, MSG_Y, 0, 2,&(Fbuf[0]));
                }
            }
            break;
        }
        case TPINGREPTAG:
        {   address = mesg->info.Data_u.TPingRep.source.address;
            TreatTPingData(mesg, &address, 1, &index);
            break;
        }
        case BYETAG:
        {   if((result=Bye_recv(mesg,msgsrc))>=0)
            {   tsprintf((char *)(&(Fbuf[0])),
                    "Node no. %d erased from IPMap\n",result);
                FEPrintf(MSG_X, MSG_Y, 0, 0, &(Fbuf[0]));
            }
            break;
        }
        default:
        {   tsprintf((char *)(&(Fbuf[0])),
                "Unknown message type\n");
            FEPrintf(MSG_X, MSG_Y, 0, 0,&(Fbuf[0]));
            break;
        }
    }
    return 0;
}

/*
 * SrvrFEPlan - display clstrsrvr Plan mode display screen
 *          tielog - 0 - do not add entries to tierra.log
 *                   1 - add entries to tierra.log
 */
void srvrFEPlan(tielog)
I32s tielog;
{   time_t tp;

    tp = tietime(NULL);

    tsprintf(Fbuf, 
        "InstExeC  %8d                                    %s",
        InstExe.m, ctime(&tp));
    FEPrintf(PLN_X, PLN_Y, 0, 0, Fbuf);

    tsprintf(Fbuf, 
    "BndWLim   %8d%42sFecundAvg %8.1f", 
        MaxOutBandWidth, " ", FecundityAvg);
    FEPrintf(PLN_X, PLN_Y+1, 0, 0, Fbuf);

    tsprintf(Fbuf,
        "NumCells  %8d  Speed     %8d  AgeAvg    %8d  NumNodes  %8d",
        NumCells, Speed, AgeAvg, NumNodesC);

    FEPrintf(PLN_X, PLN_Y+2, 0, 0, Fbuf);

    tsprintf(Fbuf,
        "SImmigMs  %8d  SImmigBt  %8d  RImmigMs  %8d  RImmigBt  %8d",
        FEMonCom[EMIGRATETAG].SNumMess, FEMonCom[EMIGRATETAG].SNumByte,
        FEMonCom[EMIGRATETAG].RNumMess, FEMonCom[EMIGRATETAG].RNumByte);
    FEPrintf(PLN_X, PLN_Y+3, 0, 0, Fbuf);

    tsprintf(Fbuf,
        "SAllMesg  %8d  SAllByte  %8d  RAllMesg  %8d  RAllByte  %8d",
        FEMonCom[0].SNumMess, FEMonCom[0].SNumByte,
        FEMonCom[0].RNumMess, FEMonCom[0].RNumByte);
    FEPrintf(PLN_X, PLN_Y+4, 0, 0, Fbuf);

    if (TierraLog && tielog)
    {   tfprintf(tfp_log, "\nie%d ti%d %s",
            InstExe.m, tp, ctime(&tp));
        tfprintf(tfp_log, "nc%d sp%d nn%hu\n",
            NumCells, Speed, NumNodesC);
        tfprintf(tfp_log, "aa%d fa%.1f\n",
            AgeAvg, FecundityAvg);

        tfprintf(tfp_log, "\n");

        tfprintf(tfp_log, "sim%d sip%d sib%d\n",
            FEMonCom[EMIGRATETAG].SNumMess,
            FEMonCom[EMIGRATETAG].SNumPack,
            FEMonCom[EMIGRATETAG].SNumByte);
        tfprintf(tfp_log, "rim%d rip%d rib%d\n",
            FEMonCom[EMIGRATETAG].RNumMess,
            FEMonCom[EMIGRATETAG].RNumPack,
            FEMonCom[EMIGRATETAG].RNumByte);

        tfprintf(tfp_log, "spqm%d spqp%d spqb%d\n",
            FEMonCom[TPINGREQTAG].SNumMess,
            FEMonCom[TPINGREQTAG].SNumPack,
            FEMonCom[TPINGREQTAG].SNumByte);
        tfprintf(tfp_log, "rpqm%d rpqp%d rpqb%d\n",
            FEMonCom[TPINGREQTAG].RNumMess,
            FEMonCom[TPINGREQTAG].RNumPack,
            FEMonCom[TPINGREQTAG].RNumByte);

        tfprintf(tfp_log, "sppm%d sppp%d sppb%d\n",
            FEMonCom[TPINGREPTAG].SNumMess,
            FEMonCom[TPINGREPTAG].SNumPack,
            FEMonCom[TPINGREPTAG].SNumByte);
        tfprintf(tfp_log, "rppm%d rppp%d rppb%d\n",
            FEMonCom[TPINGREPTAG].RNumMess,
            FEMonCom[TPINGREPTAG].RNumPack,
            FEMonCom[TPINGREPTAG].RNumByte);

        tfprintf(tfp_log, "sam%d sap%d sab%d\n",
            FEMonCom[0].SNumMess,
            FEMonCom[0].SNumPack,
            FEMonCom[0].SNumByte);
        tfprintf(tfp_log, "ram%d rap%d rab%d\n",
            FEMonCom[0].RNumMess,
            FEMonCom[0].RNumPack,
            FEMonCom[0].RNumByte);

#ifndef AMIGA
#ifndef DECVAX
        tfflush(tfp_log);
#endif /* DECVAX */
#endif /* AMIGA */
    }
}

/*
 * IPMapTPing_send - - send a TPing message from cell with given
 *              CommAddr to all Tierra nodes in given IP map
 * whichmap - 0 = IPMapC, 1 = IPMapS
 */
void IPMapTPing_send(whichmap)
I32s whichmap;
{   I32s NumNodes;
    ComAddr cell_addr;
    I32s index;
    MapNode *IPMap;

    SelfID(NULL,&cell_addr);  /* fill in IP and port number */
    NumNodes=(whichmap)?NumNodesS:NumNodesC;
    IPMap=(whichmap)?IPMapS:IPMapC;
    for(index=0;index<NumNodes;index++)
        if(((IPMap[index].address.node)!=(LocalNetAddr.node))||
            ((IPMap[index].address.portnb)!=(LocalNetAddr.portnb)))
            TPing_send(index, &cell_addr, whichmap);
}

/*
 * Map_send - send srcmap to every node in dstmap
 * srcmap - 0 = IPMapC
 *          1 = IPMapS
 * dstmap - 0 = IPMapC
 *          1 = IPMapS
 */
I32s Map_send(srcmap, dstmap)
I32s srcmap; /* 0 = IPMapC, 1 = IPMapS */
I32s dstmap; /* 0 = IPMapC, 1 = IPMapS */
{   I32s status = 0;
    I32u i, NumNodes;
    MapNode  *IPMap;

    NumNodes=(srcmap)?NumNodesS:NumNodesC;
    IPMap=(srcmap)?IPMapS:IPMapC;
    for (i = 0; i < NumNodes; i++)/* send stream to every site in IPMap */
    {   if(MapNode_send(dstmap, &(IPMap[i])))
            status=-1;
    }
    return status;
}

/*
 * MapNode_send - send mapnode to every node in dstmap
 * mapnode - map node to be sent
 * dstmap - 0 = IPMapC
 *          1 = IPMapS
 */
I32s MapNode_send(dstmap, mapnode)
I32s dstmap; /* 0 = IPMapC, 1 = IPMapS */
MapNode  *mapnode;
{   I32s status = 0;
    I32u i, NumNodes;
    MapNode  *IPMap;

    NumNodes=(dstmap)?NumNodesS:NumNodesC;
    IPMap=(dstmap)?IPMapS:IPMapC;
    for (i = 0; i < NumNodes; i++)/* send stream to every site in IPMap */
    {   if(((IPMap[i].address.node)!=(LocalNetAddr.node))||
            ((IPMap[i].address.portnb)!=(LocalNetAddr.portnb)))
            if(MapNode_send_mesg(IPMap[i].address, dstmap, mapnode))
                status=-1;
    }
    return status;
}

/*
 * MapNode_send_mesg - send mapnode message to dest addr
 * destaddr - destination address for map node
 * dstmap - 0 = IPMapC
 *          1 = IPMapS
 * mapnode - map node to be sent
 */
I32s MapNode_send_mesg(destaddr, dstmap, mapnode)
NetAddr destaddr;
I32s dstmap;
MapNode  *mapnode;
{   I32s len, status=0, sufrembndcap;
    TMessage mesg;
    struct TieCommID dstcommid;
    struct in_addr addr;
    struct hostent *hostentbuf;
    I8s *nodename;

    memset(&mesg, 0, sizeof(TMessage));
    mesg.info.tag=SENDADDRTAG;  /* compose XDR stream */
    addr.s_addr=*((uint32_t *)(&(mapnode->address.node)));
    hostentbuf=tgethostbyaddr((char *)(&addr),
        sizeof(struct in_addr), AF_INET);
    nodename=(hostentbuf)?(hostentbuf->h_name):"name unknown";
    len=strlen(nodename)+1;
    mesg.info.Data_u.Addr.name.name_len=len;
    mesg.info.Data_u.Addr.name.name_val=
        (I8s *) tcalloc(len, sizeof(I8s), 1004);
    tsprintf(mesg.info.Data_u.Addr.name.name_val, "%s", nodename);
    (mesg.info.Data_u.Addr.name.name_val)[len-1]='\0';
    mesg.PingData.TransitTime=0;
    mesg.dstmap=dstmap;

#ifdef CLSTRSRVR
    mesg.srcmap=1;
#else /* CLSTRSRVR */
    mesg.srcmap=0;
#endif /* CLSTRSRVR */

    mesg.PingData=mapnode->data;
    mesg.PrivPingData=mapnode->PrivPingData;
    if(((destaddr.node)!=(LocalNetAddr.node))||
        ((destaddr.portnb)!=(LocalNetAddr.portnb)))
    {   dstcommid.tiec_node=destaddr.node;
        dstcommid.tiec_port=destaddr.portnb;
        RemotPort=ntohs(dstcommid.tiec_port);
        if((destaddr.node)&&(destaddr.portnb))
            status=TieCommSend(&globtcibuf, &mesg, dstcommid, &sufrembndcap);
    }
    tfree(mesg.info.Data_u.Addr.name.name_val,32*sizeof(I8s), 1004);
    return status;
}

void SetRemOutBndCap()
{
#ifdef SIGBLOCK
    sigset_t SigIOSaveSet;
#endif /* SIGBLOCK */

    I32s allocoutbndcap, allocminoutbndcap, i;

#ifdef SIGBLOCK
    SigIOSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    RemOutBndCap=(MaxOutBandWidth>=0)?
        (MaxOutBandWidth*IntrvlOutBndCap):-1;

    if(MaxOutBandWidth>=0)
    {   allocoutbndcap=RemOutBndCap;
        allocoutbndcap-=((I32s)(((double)
            SrvrOutBndFrac)*((double)RemOutBndCap)));
        allocminoutbndcap=((I32s)(((double)
            TieMinOutBndFrac)*((double)RemOutBndCap)));
        allocoutbndcap-=allocminoutbndcap;
        allocminoutbndcap/=NumNodesC;
    }

    for(i=0;(i<(I32s)NumNodesC)&&(Speed>0);i++)
        IPMapC[i].PrivPingData.remoutbndcap=
            (MaxOutBandWidth>=0)?(allocminoutbndcap+
            ((I32s)(((double)allocoutbndcap)*
            ((Speed>0)?(((double)(IPMapC[i].data.Speed))/
            ((double)Speed)):(1.0/((double)NumNodesC)))))):(-1);

    LstOutBndCapRst=tietime(NULL);
    OutCapUpdTog=(OutCapUpdTog)?0:1;

#ifdef SIGBLOCK
    TSigRelease(SigIOSaveSet);
#endif /* SIGBLOCK */
}

void UpdRemOutBndCap()
{   I32s timesv;
    timesv=tietime(NULL);
    if((timesv-LstOutBndCapRst)>=IntrvlOutBndCap)
        SetRemOutBndCap();
}
