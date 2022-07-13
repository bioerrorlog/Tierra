/* beagleMessage_Migr.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Fri Aug 15 17:25:41 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:22:42 1999
 * Update Count    : 184
 * Status          : this module concerns migration tracking
 *                   perfectly work
 */

#include "clnt.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrP.h"
#include "beagleMessage_Migr.h"
#include "beagleMessage_Connect.h"
#include "beagleUTIL_MigrMgr.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleUTIL_Convert.h"
#include "beagleUI.h"


/***  about Migration tracking functions  ***/
static MM_state_t MM_migrationOn_s = MMS_NORMAL;
static MM_state_t MM_migrationOff_s = MMS_NORMAL;

static I32s	   MM_MigrationTrackingMode = 0; /* 0: OFF, 1: ON */
MM_state_t MM_migration_s           = MMS_NORMAL;

static I32s	BglMigrationSockTCP;
static I32s	BglMigrationSockFd;
static fd_set   BglMigrationAfds, BglMigrationRfds;
static I32s     BglMigrCtrlSockTCP;
static fd_set	BglMigrCtrlAfds, BglMigrCtrlRfds;
static I32s	BglMigrationQLEN = 5;
static BglMigrationConnectSt	BglMigrationConnectBuf;


static N32u     newNode; /*warning : never change those variables
			   outside *startP functions */

static I32s	        BglMM_Migration_modeReset	P_((void));

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_MigrationOn_startP     P_((void));
static MM_funcstate_t	BglMM_NB_MigrationOff_startP    P_((void));
static MM_funcstate_t	BglMM_NB_MigrationOn_normtermP  P_((void));
static MM_funcstate_t	BglMM_NB_MigrationOff_normtermP P_((void));
static MM_funcstate_t	BglMM_NB_Migration_abnormtermP  P_((void));
static MM_funcstate_t	BglMM_NB_MigrationOn    	P_((void));
static MM_funcstate_t	BglMM_NB_MigrationOn_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_MigrationOff   	P_((void));
static MM_funcstate_t	BglMM_NB_MigrationOff_Wait	P_((void));

static I32s	BglMM_NB_MigrReplyWait  P_((I32s,I32s));
#endif /* NB_MECHANISM */
#ifdef B_MECHANISM
static I32s     BglMM_B_MigrReplyWait   P_((I32s,I32s));

static I32s	BglMM_B_MigrationOn     P_((void));
static I32s	BglMM_B_MigrationOff    P_((void));
#endif /* B_MECHANISM */

struct MM_BglMigrationList_T {
  struct MM_BglMigrationList_T*	next;
  BglMigrationDataSt		d;
};
typedef struct MM_BglMigrationList_T	MM_BglMigrationList_t;
static MM_BglMigrationList_t	MM_migrationDataRoot;
static MM_BglMigrationList_t*	MM_migrationRp = &MM_migrationDataRoot;
static MM_BglMigrationList_t*	MM_migrationWp = &MM_migrationDataRoot;
static I32s			MM_migrationData_n = 0;

/**************************************************
 * buffer for migration information
 */
static I32s	BglMM_Migration_modeReset()
{
  MM_migration_s = MMS_REQUESTED;
  MM_migrationWp = MM_migrationRp;
  MM_migrationData_n = 0;
  if ( !(MM_migrationDataRoot.next) )
    MM_migrationDataRoot.next = &MM_migrationDataRoot;
  return 1;
}

I32s	BglMM_HowManyMigration(){  return MM_migrationData_n; }

void	BglMM_Migration_store(d)
     BglMigrationDataSt* d;
{
  struct timeval now;
  int		 testprint;

  /*DebugPrint("/==== BglMM_Migration_store ====/\n");*/
  if ( MM_migration_s == MMS_REQUESTED ||
       MM_migration_s == MMS_CHANGED ){

    tgettimeofday(&now, NULL);

    MM_migrationWp->d.node		= d->node;
    strcpy(MM_migrationWp->d.localparameter, d->localparameter);
    MM_migrationWp->d.storelparam	= d->storelparam;
    MM_migrationWp->d.session		= (d->session < 0) ?
      UTIL_MigrMgr_TrackingStopRequest(), - d->session :
      d->session;
    MM_migrationWp->d.information	= d->information;
    MM_migrationWp->d.ranID		= d->ranID;
    MM_migrationWp->d.tierraTime	= d->tierraTime;
    MM_migrationWp->d.beagleTime	= now.tv_sec;
    MM_migrationWp->d.fecundity		= d->fecundity;


    /* Check session number */
    UTIL_MigrMgr_Checksession(MM_migrationWp->d.session);

    /* check if data is coherent with cellID and ranID and write it */
    if ( MM_migrationWp->d.information - 'n'){
      switch ( testprint = UTIL_MigrMgr_Write(&(MM_migrationWp->d))){
      case 0:
	BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		  "Access disk error for migration"); 
	BglMM_MigrationOff_need();
	break;
      case -1:
	return;
      }

      /* Memorize the last node which sent migration info to Beagle */
      if (UTIL_MigrMgr_ValidMigrSession())
	UTIL_MigrMgr_LastNode = MM_migrationWp->d.node;
    }

    /* save new data */
    if ( MM_migrationWp->next == MM_migrationRp ){
      MM_migrationWp->next = (MM_BglMigrationList_t*)
	tcalloc(1,sizeof(MM_BglMigrationList_t),52080);
      MM_migrationWp->next->next = MM_migrationRp;
    }
    MM_migrationWp = MM_migrationWp->next;
    MM_migrationData_n++;
    MM_migration_s = MMS_CHANGED;
  }

  return;
}


BglMigrationDataSt*	BglMM_PickMigration()
{
  BglMigrationDataSt* retP = &(MM_migrationRp->d);
  MM_migrationRp = MM_migrationRp->next;
  if ( MM_migrationRp == MM_migrationWp ) MM_migration_s = MMS_REQUESTED;
  MM_migrationData_n--;
  return retP;
}

/**************************************************
 * migration control
 */
I32s	BglMM_ChangeMigrCtrlPort()
{
  BglSockSt* sP;
  if ( MM_MigrationTrackingMode == 0 ){/* Migration Tracking OFF */
    if ( ( sP = UTIL_Sock_GetCurrent_Connection() ) != NULL ){
      UTIL_MigrMgr_CtrlPortInit(sP->addr.sPortService);
    }
  }
  return 0;	/* No Error */
}

I32s	BglMM_MigrationDataPortOpen()
{
  struct hostent*	hostEntP;

  do{
    BglMigrationSockTCP
      = bgl_passiveTCP( UTIL_MigrMgr_PortGet(), BglMigrationQLEN );
  }
  while (BglMigrationSockTCP < 0 && (UTIL_MigrMgr_PortChange(), 1));
  FD_SET(BglMigrationSockTCP, &BglMigrationAfds);

  tgethostname( bglStrBuf, strlen(bglStrBuf) );
  hostEntP = tgethostbyname( bglStrBuf );
  BglMigrationConnectBuf.node = (*(I32u *)hostEntP->h_addr_list[0]);
  BglMigrationConnectBuf.portnb = htons( atoi( UTIL_MigrMgr_PortGet() ) );
  BglMigrationConnectBuf.session = 0;
  BglMigrationConnectBuf.param[0] = '\0';

  return( 0 );
}

void	BglMM_MigrationDataCheckAndDo()
{
  I32s nselect;
  struct sockaddr_in fsin;
  int alen = sizeof(fsin);
  I32s nread;
#ifdef SIGBLOCK
  sigset_t BglSigSaveSet;

  BglSigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

  memcpy((I8s*)&BglMigrationRfds,(I8s*)&BglMigrationAfds,
	 sizeof(BglMigrationRfds));

  if ((nselect = bgl_select(BglNfds, &BglMigrationRfds, 0)) < 0){
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
	      "Select Error in BglMM_MigrationDataCheckAndDo()");
  }
  else if (nselect > 0){
    if ( FD_ISSET(BglMigrationSockTCP, &BglMigrationRfds)) {

      if ((BglMigrationSockFd
	   = accept(BglMigrationSockTCP, (struct sockaddr*)&fsin, &alen))
	  < 0){
	BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
		  "Accept Error in BglMM_MigrationDataCheckAndDo()");
#ifdef SIGBLOCK
	TSigRelease(BglSigSaveSet);
#endif /* SIGBLOCK */
	return;	/* ERROR Return */
      }
      else{
	FD_SET(BglMigrationSockFd, &BglMigrationAfds);
	/*tfprintf( stdout, "MigrationData Connected : fd = %d\n",
	  BglMigrationSockFd);*/
      }

      /* read MigrationData from MigrationSockFd and store */
      nread = BglMM_B_Recv(BglMigrationSockFd);

      if ( MM_MigrationTrackingMode == 1 && UTIL_MigrMgr_ValidMigrSession()){
	/* Migration Mode ON            and     valid session number*/
	if (UTIL_MigrMgr_StopRequestCheck()){/* if tracking stop requested */
	  BglUI_Migration_Changed();   /* Print the message to screen */
	  BglMM_MigrationOff_need();   /* or we stop tracking */
	}
	/* send ACK to Tierra via MigrationSockFd */
	BglReplyAck(BglMigrationSockFd, BGL_MIG_DATA_OK);
	UTIL_MigrMgr_TimeoutInit();
	UTIL_MigrMgr_TrackingStopReset();
      }
      else{
	/* Migration Mode OFF or invalid session number*/
	/* send NACK to Tierra via MigrationSockFd */ 
	if (UTIL_MigrMgr_StopRequestCheck())
	  BglReplyAck(BglMigrationSockFd, BGL_MIG_DATA_OK);
	/* avoid to send unusable stop request */
	else
	  BglReplyAck(BglMigrationSockFd, BGL_MIG_DATA_REF);
	UTIL_MigrMgr_TrackingStopReset();
      }
      MM_isReceived = 1;
      FD_CLR((I32u)BglMigrationSockFd, &BglMigrationAfds);
      shutdown(BglMigrationSockFd, 2);	
      (void)tclosesocket(BglMigrationSockFd);
    }
  }
  else{
    /* select TIME OUT */
    if ( (MM_MigrationTrackingMode == 1) && (! UTIL_MigrMgr_TimeoutChek()) ){
      UTIL_MigrMgr_TimeoutInit();
      BglMM_MigrationOn_need();      /* start tracking again */
    }
  }
  
#ifdef SIGBLOCK
  TSigRelease(BglSigSaveSet);
#endif /* SIGBLOCK */
}



I32s	BglMM_MigrationOn_need()
{
  MM_SequenceList_t* sP;

  if ( MM_migrationOn_s == MMS_NORMAL ){
    MM_migrationOn_s = MMS_NEED;
    /* Init connexion with Beagle host */
    BglMigrationConnectBuf.session = UTIL_MigrMgr_NewSession();
    UTIL_MigrMgr_TimeoutInit(); /* Instruction to init timeout at each connection */

#ifdef NB_MECHANISM
    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell(  BglMM_NB_MigrationOn ),
			  BglMM_NB_MigrationOn_startP,
			  BglMM_NB_MigrationOn_normtermP,
			  BglMM_NB_Migration_abnormtermP,
			  NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MigrationOn_Wait );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
    BglMM_B_MigrationOn();
#endif /* B_MECHANISM */
    return 1;
  }
  else return 0;
}


I32s	BglMM_MigrationOff_need()
{
  MM_SequenceList_t* sP;

  if ( MM_migrationOff_s == MMS_NORMAL ){
    MM_migrationOff_s = MMS_NEED;

    BglUI_Migration_Changed();
    /* Print the message to screen */
    MM_MigrationTrackingMode = 0;    

#ifdef NB_MECHANISM
    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MigrationOff ),
			  BglMM_NB_MigrationOff_startP,
			  BglMM_NB_MigrationOff_normtermP,
			  BglMM_NB_Migration_abnormtermP,
			  NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MigrationOff_Wait );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
    BglMM_B_MigrationOff();
#endif /* B_MECHANISM */
    return 1;
  }
  else return 0;
}

#ifdef NB_MECHANISM
/*
 * BglMM_NB_MigrationOn_startP
 *	open migration control connection
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_MigrationOn_startP()
{
  MM_migrationOn_s   = MMS_NORMAL;
  newNode = UTIL_MigrMgr_BuffCtrlRead(BglMigrationConnectBuf.param);

  BglMigrCtrlSockTCP =
    bgl_connectTCP(inet_ntoa(*((struct in_addr*)&newNode)),
		   UTIL_MigrMgr_CtrlPortGet() );

  if (BglMigrCtrlSockTCP < 0){
    BglMM_Connect_Close();
    return MMFS_AGAIN;
  }
  else{
    FD_ZERO(&BglMigrCtrlAfds);
    FD_SET(BglMigrCtrlSockTCP, &BglMigrCtrlAfds);
    UTIL_MigrMgr_LastNode = newNode;
    return MMFS_CONTINUE;
  }
}

/*
 * BglMM_NB_MigrationOff_startP
 *	open migration control connection
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_MigrationOff_startP()
{
  MM_migrationOff_s   = MMS_NORMAL;
  newNode = UTIL_MigrMgr_LastNode;
  UTIL_MigrMgr_LastNode
    = UTIL_MigrMgr_BuffCtrlRead(BglMigrationConnectBuf.param);

  BglMigrCtrlSockTCP =
    bgl_connectTCP(inet_ntoa(*((struct in_addr*)&newNode)),
		   UTIL_MigrMgr_CtrlPortGet() );

  if (BglMigrCtrlSockTCP < 0){
    return MMFS_FAIL;
  }
  else{
    FD_ZERO(&BglMigrCtrlAfds);
    FD_SET(BglMigrCtrlSockTCP, &BglMigrCtrlAfds);
    return MMFS_CONTINUE;
  }
}


/*
 * BglMM_NB_Migration_abnormtermP
 *	close migration control connection 
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_Migration_abnormtermP()
{
  if (BglMigrCtrlSockTCP >= 0){
    shutdown( BglMigrCtrlSockTCP, 2 );
    tclosesocket(BglMigrCtrlSockTCP);
    FD_CLR((I32u)BglMigrCtrlSockTCP, &BglMigrCtrlAfds);
  }
  tsprintf(bglStrBuf2, "Migration Control Aborted:\n host=%-14s service=%s",
	   UTIL_Convert_IpToName(((struct in_addr*)&newNode)),
	   UTIL_MigrMgr_CtrlPortGet());
  BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
  return MMFS_CONTINUE;
}


/*
 * BglMM_NB_MigrationOn_normtermP
 *	close connection for normal termination
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_MigrationOn_normtermP()
{
  shutdown(BglMigrCtrlSockTCP, 2);
  tclosesocket(BglMigrCtrlSockTCP);
  FD_CLR((I32u)BglMigrCtrlSockTCP, &BglMigrCtrlAfds);
  tsprintf(bglStrBuf2, "Migration Started:\n host=%-14s service=%s",
	   UTIL_Convert_IpToName(((struct in_addr*)&newNode)),
	   UTIL_MigrMgr_CtrlPortGet());
  BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
  BglUI_MigrationStatsToOn();
  return MMFS_CONTINUE;
}


/*
 * BglMM_NB_MigrationOn_normtermP
 *	close connection for normal termination
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_MigrationOff_normtermP()
{
  shutdown(BglMigrCtrlSockTCP, 2);
  tclosesocket(BglMigrCtrlSockTCP);
  FD_CLR((I32u)BglMigrCtrlSockTCP, &BglMigrCtrlAfds);
  tsprintf(bglStrBuf2, "Migration Stopped:\n host=%-14s service=%s",
	   UTIL_Convert_IpToName((struct in_addr*)&newNode),
	   UTIL_MigrMgr_CtrlPortGet());
  BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
  BglUI_MigrationStatsToOff();

  BglMM_ChangeMigrCtrlPort();

  return MMFS_CONTINUE;
}


/*
 * BglMM_NB_MigrationOn
 *	send migration on request
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_MigrationOn()
{
  if (!BglMM_Send(BglMigrCtrlSockTCP, &BglReqMigTrackOn))
    /* Communication NO Error */
    return MMFS_CONTINUE;
  /* try another time */
  return MMFS_FAIL;
}


/*
 * BglMM_NB_MigrationOn_Wait
 *	wait acknoledgement and send datas for migration control
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_MigrationOn_Wait()
{
  I32s	ret;
  BglMesSt sendMes;
  I32s	len;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  ret = BglMM_NB_MigrReplyWait( BGL_MIG_REQ_OK, BGL_MIG_REQ_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglMigrationConnectSt(&msgHndl, &BglMigrationConnectBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"XDR encode error in the function BglMM_NB_MigrationOn_Wait()");
      return MMFS_FAIL; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
    len = txdr_getpos(&msgHndl);
    len = BglMakeMesData(BGL_MIGR_CTRL, xdrMsgBuf, len, xdrMsgBuf2 );

    sendMes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
    BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendMes);
    twrite(BglMigrCtrlSockTCP, sendMes.buf, sendMes.len );

    tfree( sendMes.buf, BGL_MAX_MES_LEN, 11502 );
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);

    BglMM_Migration_modeReset();

    MM_MigrationTrackingMode = 1; /* Set Mode ON */
    MM_isReceived = 1;
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request Migration Tracking On was refused."); 
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    if ( BglReplyBuf.ack == BGL_MIG_REQ_REF_NONET ){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
      return MMFS_FAIL;
    }
    break;
  }
  return MMFS_AGAIN;
}

/*
 * glMM_NB_MigrationOff
 *	send migration off request
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_MigrationOff()
{
  if (!BglMM_Send(BglMigrCtrlSockTCP, &BglReqMigTrackOff)){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  else
    return MMFS_FAIL;
}

/*
 * BglMM_NB_MigrationOff_Wait
 *	wait acknoledgement and send datas for migration control
 * Returns:	Process status (see process manager)
 */
static MM_funcstate_t	BglMM_NB_MigrationOff_Wait()
{
  I32s	ret;
  BglMesSt sendMes;
  I32s	len;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  ret = BglMM_NB_MigrReplyWait( BGL_MIG_REQ_OK, BGL_MIG_REQ_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:

    xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglMigrationConnectSt(&msgHndl, &BglMigrationConnectBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"XDR encode error in the function BglMM_NB_MigrationOff_Wait()");
      return MMFS_FAIL; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
    len = txdr_getpos(&msgHndl);
    len = BglMakeMesData(BGL_MIGR_CTRL, xdrMsgBuf, len, xdrMsgBuf2 );

    sendMes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
    BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendMes);
    twrite(BglMigrCtrlSockTCP, sendMes.buf, sendMes.len);

    tfree( sendMes.buf, BGL_MAX_MES_LEN, 11502 );
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);

    MM_migration_s = MMS_REQUESTED;

    MM_MigrationTrackingMode = 0; /* Set Mode OFF */
    MM_isReceived = 1;
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request Migration Tracking Off was refused."); 
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    if ( BglReplyBuf.ack == BGL_MIG_REQ_REF_NONET ){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
      BglMM_NB_Cont();
      return MMFS_FAIL;
    }
    break;
  }

  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */



#ifdef B_MECHANISM

static I32s	BglMM_B_MigrationOn()
{
  BglMesSt sendMes;
  I32s	len;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  newNode = UTIL_MigrMgr_BuffCtrlRead(BglMigrationConnectBuf.param);
  MM_migrationOn_s = MMS_NORMAL;
  if ((BglMigrCtrlSockTCP
       = bgl_connectTCP(inet_ntoa(*((struct in_addr*)&newNode)),
			UTIL_MigrMgr_CtrlPortGet()))
      < 0 ){
    BglMM_Connect_Close();
    return 0;
  }
  UTIL_MigrMgr_LastNode = newNode;
  if (BglMM_Send(BglMigrCtrlSockTCP, &BglReqMigTrackOn))
    return 0;/*Communication Error*/

  FD_ZERO(&BglMigrCtrlAfds);
  FD_SET(BglMigrCtrlSockTCP, &BglMigrCtrlAfds);

  if ( BglMM_B_MigrReplyWait( BGL_MIG_REQ_OK, BGL_MIG_REQ_REF )
      != BGL_REPLY_RET_OK ){
    if(BglReplyBuf.ack == BGL_MIG_REQ_REF_NONET){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
    }
    else{
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request Migration Tracking On was refused."); 
    }
  }
  else{
    xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglMigrationConnectSt(&msgHndl, &BglMigrationConnectBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"XDR encode error in the function BglMM_B_MigrationOn()");
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
    len = txdr_getpos(&msgHndl);
    len = BglMakeMesData(BGL_MIGR_CTRL, xdrMsgBuf, len, xdrMsgBuf2 );

    sendMes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
    BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendMes);
    twrite(BglMigrCtrlSockTCP, sendMes.buf, sendMes.len);

    tfree( sendMes.buf, BGL_MAX_MES_LEN, 11502 );
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);

    BglMM_Migration_modeReset();

    MM_MigrationTrackingMode = 1; /* Set Mode ON */
    MM_isReceived = 1;
  }
  shutdown(BglMigrCtrlSockTCP, 2);
  tclosesocket(BglMigrCtrlSockTCP);
  FD_CLR((I32u)BglMigrCtrlSockTCP, &BglMigrCtrlAfds);

  return 1;	/* No Error */
}


static I32s	BglMM_B_MigrationOff()
{
  BglMesSt sendMes;
  I32s	len;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  MM_migrationOff_s = MMS_NORMAL;
  newNode = UTIL_MigrMgr_LastNode;
  UTIL_MigrMgr_LastNode
    = UTIL_MigrMgr_BuffCtrlRead(BglMigrationConnectBuf.param);

  if ((BglMigrCtrlSockTCP
       = bgl_connectTCP(inet_ntoa(*((struct in_addr*)&(newNode))),
			 UTIL_MigrMgr_CtrlPortGet()))
      < 0 ){
    return 0;
  }
  if ( BglMM_Send(BglMigrCtrlSockTCP, &BglReqMigTrackOff) )
    return 0;/*Communication Error*/

  FD_ZERO(&BglMigrCtrlAfds);
  FD_SET(BglMigrCtrlSockTCP, &BglMigrCtrlAfds);

  if ( BglMM_B_MigrReplyWait( BGL_MIG_REQ_OK, BGL_MIG_REQ_REF )
      != BGL_REPLY_RET_OK ){
    if(BglReplyBuf.ack == BGL_MIG_REQ_REF_NONET){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
    }
    else{
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request Migration Tracking On was refused."); 
    }
  }
  else{
    xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglMigrationConnectSt(&msgHndl, &BglMigrationConnectBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"XDR encode error in the function BglMM_B_MigrationOff()");
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
    len = txdr_getpos(&msgHndl);
    len = BglMakeMesData(BGL_MIGR_CTRL, xdrMsgBuf, len, xdrMsgBuf2 );

    sendMes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
    BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendMes);
    twrite(BglMigrCtrlSockTCP, sendMes.buf, sendMes.len);

    tfree( sendMes.buf, BGL_MAX_MES_LEN, 11502 );
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);

    MM_migration_s = MMS_REQUESTED;

    MM_MigrationTrackingMode = 0; /* Set Mode OFF */
    MM_isReceived = 1;
  }
  shutdown(BglMigrCtrlSockTCP, 2);
  tclosesocket(BglMigrCtrlSockTCP);
  FD_CLR((I32u)BglMigrCtrlSockTCP, &BglMigrCtrlAfds);

  BglMM_ChangeMigrCtrlPort();

  return 1;	/* No Error */
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM

static I32s	BglMM_NB_MigrReplyWait(repOk, repRef)
     I32s repOk, repRef;
{
  I32s ret;
  static I32s timeCount = 0;
  struct timeval now;
  BglReplySt* rP;

  if ( BglMM_HowManyReply() ){
    rP = BglMM_PickReply();
    if ( rP->ack == repOk ){
      timeCount = 0;
      return BGL_REPLY_RET_OK;
    }
    else if ( rP->ack == repRef ){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request was refused by Tierra.");
      timeCount = 0;
      return BGL_REPLY_RET_REF;
    }
    else{
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"unexpected data received.");
      timeCount = 0;
      return BGL_REPLY_RET_UNEXPECT;
    }
  }
  else{
    ret = BglReplyWait(BglMigrCtrlSockTCP,
		       BglNfds, &BglMigrCtrlAfds, repOk, repRef, (double)0);

    switch( ret ){
    case BGL_REPLY_RET_OK:	/* GOOD! (Normal return) */
      timeCount = 0;
      (void)BglMM_PickReply();
      return ret;
      break;
    case BGL_REPLY_RET_REF:	/* Request refused by Tierra */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request was refused by Tierra.");
      timeCount = 0;
      (void)BglMM_PickReply();
      return ret;
      break;
    case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"unexpected data received.");
      timeCount = 0;
      (void)BglMM_PickReply();
      return ret;
      break;
    case BGL_REPLY_RET_TIMEOUT:	/* message TIME OUT */
      tgettimeofday(&now, NULL);
      if (!timeCount)
	timeCount = now.tv_sec +  1; /* because Bgl_wait0 ~ 10 */
      if ( now.tv_sec > (timeCount + Bgl_wait0) ){ /* can support polling cycles */
	BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		  "reply message could not reach on time. ignore this request.");
	timeCount = 0;
	return ret;		/* Tiem out */
      }
      else{
	return BGL_REPLY_RET_NONE;	/* There is no reply yet */
      }
      break;
    case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"unexpected FD. (mistrust PROGRAM)");
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_COM_ERR:	/* Communication Error */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Error in BglMM_NB_ReplyWait");
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"something happened during bgl_select. (mistrust PROGRAM)");
      timeCount = 0;
      return ret;
      break;
    default:
      timeCount = 0;
      return ret;
      break;
    }
  }
}
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM

static I32s BglMM_B_MigrReplyWait(repOk, repRef)
     I32s repOk, repRef;
{
  I32s ret;

  while(1){
    ret = BglReplyWait(BglMigrCtrlSockTCP,
		       BglNfds, &BglMigrCtrlAfds, repOk, repRef, Bgl_wait0);

    switch( ret ){
    case BGL_REPLY_RET_OK:	/* GOOD! (Normal return) */
      (void)BglMM_PickReply();
      return ret;
      break;
    case BGL_REPLY_RET_REF:	/* Request refused by Tierra */
      (void)BglMM_PickReply();
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request was refused by Tierra.");
      return ret;
      break;
    case BGL_REPLY_RET_NONE:
      continue;  /* Read again */
      break;
    case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
      (void)BglMM_PickReply();
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"unexpected reply data come from Tierra. (mistrust PROGRAM)");
      return ret;
      break;
    case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
      (void)BglMM_PickReply();
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"unexpected FD. (mistrust PROGRAM)");
      continue;
      /*return BGL_REPLY_RET_ILL_FD;*/
      break;
    case BGL_REPLY_RET_TIMEOUT:	/* message TIME OUT */
      /*BglMM_B_Cont( s );*/
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"reply message could not reach on time. ignore this request.");
      return ret;
      break;
    case BGL_REPLY_COM_ERR:	/* Communication Error */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Error in BglMM_B_ReplyWait");
      return ret;
      break;
    case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
      /*BglMM_B_Cont( s );*/
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"something happened during bgl_select. (mistrust PROGRAM)");
      return ret;
      break;
    }
  }/* END of while Loop */
}

#endif /* B_MECHANISM */
