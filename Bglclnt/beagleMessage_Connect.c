/*
 * beagleMessage_Connect.c -- 
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Tue Nov 18 10:25:03 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Dec  2 19:00:03 1999
 * Update Count    : 138
 * Status          : Under construction
 */

#include "clnt.h"
#include "beagleMessage_Connect.h"
#include "beagleMessageMgrP.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleUTIL_Convert.h"
#include "beagleUTIL_MigrMgr.h"


static I32s     BglConnectTCP;
static fd_set	BglConnectAfds;

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Connect_startP		P_((void));
static MM_funcstate_t	BglMM_NB_Connect_abnormtermP	P_((void));
static MM_funcstate_t	BglMM_NB_Connect_normtermP	P_((void));
static MM_funcstate_t	BglMM_NB_Connect_Open		P_((void));
static MM_funcstate_t	BglMM_NB_Connect_Open_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Connect		P_((void));
static MM_funcstate_t	BglMM_NB_ReConnect		P_((void));
static MM_funcstate_t	BglMM_NB_Connect_Close		P_((void));
static I32s	BglMM_NB_ConnectReplyWait		P_((I32s, I32s));
#endif /* NB_MECHANISM */

static I32s		BglMM_B_Connect_Open		P_((char*));
static I32s	BglMM_B_ConnectReplyWait		P_((I32s, I32s));
#ifdef B_MECHANISM
static I32s		BglMM_B_Connect_ReOpen		P_((char*, int));
static MM_funcstate_t	BglMM_B_Connect			P_((void));
static MM_funcstate_t	BglMM_B_ReConnect		P_((int));
static MM_funcstate_t	BglMM_B_Connect_Close		P_((void));
#endif /* B_MECHANISM */

/*
 * BglMM_ConnectCom
 *	function to connect to one machine. This function is used just
 * once at the starting of beagle. Unused after.
 */
static I32s	BglMM_ConnectCom	P_((BglSockSt*));
static I32s	BglMM_ConnectionClose	P_((BglSockSt*));
static I32s	BglMM_RecvConnect	P_((BglSockSt*,I32s));

/**********************************************************************
 * Connection protocol functions
 */
I32s	BglMM_Connect_Open(connectString, block)
     char* connectString;
     int block;
{
  char		ans;
  BglSockSt	tmpS;

#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  I8s *	params[1];
#endif /* NB_MECHANISM */

  if ( ! strlen(connectString) ) return 1;	/* ERROR Return */
  /* analyse of request */
  ans = BglParseArgs( connectString, &tmpS );
  switch(ans){
  case 's':	/* Do Su beagle */
    BglGetUname(&(tmpS.user.name[0]));
    BglGetPasswd(&(tmpS.user));
    break;
  case 'n': 	/*  Do NonSu beagle */
    BglGetUname(&(tmpS.user.name[0]));
    break;
  case 'H':
    return 2;	/* ERROR Return ? User want only look help message */
    break;
  default:
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 0, 0, "wrong command.");
    return 3;	/* ERRIR Return */
    break;
  }

  /* start new connection mechanism */
  UTIL_Sock_Push_PreConnection(&tmpS);
#ifdef NB_MECHANISM
  if (!block){
    params[0] = tmpS.addr.dotted;

    BglMM_MakeProcessCell(sP = BglMM_MakeFuncCell(  BglMM_NB_Connect_Open ),
			  BglMM_NB_Connect_startP,
			  BglMM_NB_Connect_normtermP,
			  BglMM_NB_Connect_abnormtermP,
			  params, 1 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Connect_Open_Wait ); sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Connect );
  }
  else
#endif /* NB_MECHANISM */
    if (BglMM_B_Connect_Open(tmpS.addr.dotted) != 0) return 2;/*ERROR Return*/

  return 0;	/* NORMAL Return */
}

void	BglMM_Connect_ReOpen(connection)
     int connection;
{
  BglSockSt	*tmpS;

#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  I8s *	params[2];
  char		tmp[10];
#endif /* NB_MECHANISM */

  tmpS = UTIL_Sock_Get_Connection(connection);
  UTIL_Sock_Push_PreConnection(tmpS);
#ifdef NB_MECHANISM
  params[0] = tmpS->addr.dotted;
  tsprintf(tmp, "%d", connection);
  params[1] = tmp;

  BglMM_MakeProcessCell(sP = BglMM_MakeFuncCell(  BglMM_NB_Connect_Open ),
			BglMM_NB_Connect_startP,
			BglMM_NB_Connect_normtermP,
			BglMM_NB_Connect_abnormtermP,
			params, 2 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Connect_Open_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_ReConnect );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  (void)BglMM_B_Connect_ReOpen(tmpS->addr.dotted, connection);
#endif /* B_MECHANISM */
}

void	BglMM_Connect_Close()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell(sP = BglMM_MakeFuncCell(  BglMM_NB_Connect_Close ),
			NULL, NULL, NULL,
			NULL, 0 );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Connect_Close();
#endif /* B_MECHANISM */
}

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Connect_startP()
{
  BglConnectTCP	=
    bgl_connectTCP(BglMM_GetProcessParam(0),
		   UTIL_Sock_Pop_PreConnection()->addr.sPortService );

  if (BglConnectTCP < 0)
    return MMFS_FAIL;
  else{
    FD_ZERO(&BglConnectAfds);
    FD_SET(BglConnectTCP, &BglConnectAfds);
    return MMFS_CONTINUE;
  }
}

static MM_funcstate_t	BglMM_NB_Connect_abnormtermP()
{
  if (BglConnectTCP >= 0){
    shutdown(BglConnectTCP, 2);
    tclosesocket(BglConnectTCP);
    FD_CLR((I32u)BglConnectTCP, &BglConnectAfds);
  }
  tsprintf(bglStrBuf2, "Connection Control Aborted:\n host=%s service=%s",
	   BglMM_GetProcessParam(0),
	   UTIL_Sock_Pop_PreConnection()->addr.sPortService);
  BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_Connect_normtermP()
{
  shutdown(BglConnectTCP, 2);
  tclosesocket(BglConnectTCP);
  FD_CLR((I32u)BglConnectTCP, &BglConnectAfds);
  tsprintf(bglStrBuf2, "Connection Control Accepted:\n host=%s service=%s",
	   BglMM_GetProcessParam(0),
	   UTIL_Sock_Pop_PreConnection()->addr.sPortService);
  BglPrintf(BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_Connect_Open()
{
  BglSockSt	tmpS;

  if ( !BglMM_Send(BglConnectTCP, &BglReqPreConn ) ){
    /* Communication NO Error */
    tmpS = *UTIL_Sock_Pop_PreConnection();
    BglReplyMes(BglConnectTCP, tmpS.addr.service);
    return MMFS_CONTINUE;
  }
  /* try another time */
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Connect_Open_Wait()
{
  I32s ret;

  ret = BglMM_NB_ConnectReplyWait( BGL_PRE_CON_OK, BGL_PRE_CON_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request Pre-connection accepted.");    
    return MMFS_CONTINUE;
  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request Pre-connection was refused."); 
    return MMFS_FAIL;
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
#endif /* NB_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Connect()
#endif /* NB_MECHANISM */
#ifdef B_MECHANISM
static MM_funcstate_t	BglMM_B_Connect()
#endif /* B_MECHANISM */
{
  BglSockSt	tmpS;
  int		ret = 0;

  tmpS = *UTIL_Sock_Pop_PreConnection();

  /* close connection with last machine */
  UTIL_Sock_Close_VirtualConnection();
  BglMM_ConnectionClose(UTIL_Sock_GetCurrent_Connection());

  /* re-connect to other tierra */
  ret = BglMM_ConnectCom(&tmpS); 
  if ( ret < 0 ){    /* cannot re-connect */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "can't make new connection.  trying to retain current connection.");
    /* try to recover one last connection */
    while ( UTIL_Sock_GetCurrent_Connection() &&
	   ( BglMM_ConnectCom(UTIL_Sock_GetCurrent_Connection()) < 0 ) ){
      BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
		"can't retain current connection, trying another.");
      UTIL_Sock_Pop_Connection();
    }
    if ( !UTIL_Sock_GetCurrent_Connection()){
      BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
		"This connection was the last one.  Beagle will be going down");
      BglExit(2);
      return MMFS_FAIL;
    }
  }
  else
    UTIL_Sock_Push_Connection(&tmpS);

  BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	    "Beagle Re-Connection done.");
  UTIL_Sock_Open_VirtualConnection();

  return MMFS_CONTINUE;
}

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_ReConnect()
#endif /* NB_MECHANISM */
#ifdef B_MECHANISM
static MM_funcstate_t	BglMM_B_ReConnect(connection)
     int connection;
#endif /* B_MECHANISM */
{
  BglSockSt*	sP	= NULL;
  int		ret	= 0;
#ifdef NB_MECHANISM
  int		connection;

  connection = atoi(BglMM_GetProcessParam(1));  
#endif /* NB_MECHANISM */
  if ( ! (sP =  UTIL_Sock_Pop_PreConnection())){
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 0, 0, "wrong command.");
    return MMFS_FAIL;
  }

  UTIL_Sock_Close_VirtualConnection();
  BglMM_ConnectionClose(UTIL_Sock_GetCurrent_Connection());

  ret = BglMM_ConnectCom(sP);
  if ( ret < 0 ){    /* cannot re-connect */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "can't make new connection.  trying to retain current connection.");
    /* try to recovery one last connection */
    while ( UTIL_Sock_GetCurrent_Connection() &&
	   ( BglMM_ConnectCom(UTIL_Sock_GetCurrent_Connection()) < 0 ) ){
      BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
		"can't retain current connection, trying another.");
      UTIL_Sock_Pop_Connection();
    }
    if ( !UTIL_Sock_GetCurrent_Connection()){
      BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
		"This connection was the last one.  Beagle will be going down");
      BglExit(2);
      return MMFS_FAIL;
    }
  }
  else
    UTIL_Sock_Swap_Connection(connection, sP->fd);

  BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	    "Beagle Re-Connection done.");
  UTIL_Sock_Open_VirtualConnection();

  return MMFS_CONTINUE;
}

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Connect_Close()
#endif /* NB_MECHANISM */
#ifdef B_MECHANISM
static MM_funcstate_t	BglMM_B_Connect_Close()
#endif /* B_MECHANISM */
{
  UTIL_Sock_Close_VirtualConnection();
  BglMM_ConnectionClose(UTIL_Sock_GetCurrent_Connection());
  UTIL_Sock_Pop_Connection();

  while ( UTIL_Sock_GetCurrent_Connection() &&
	 ( BglMM_ConnectCom(UTIL_Sock_GetCurrent_Connection()) < 0 ) ){
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
	      "can't retain current connection, trying another.");
    UTIL_Sock_Pop_Connection();
  }
  if ( !UTIL_Sock_GetCurrent_Connection()){
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
	      "This connection was the last one.  Beagle will be going down");
    BglExit(2);
  }
  BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	    "Beagle Re-Connection done.");
  UTIL_Sock_Open_VirtualConnection();

  return MMFS_CONTINUE;
}


static I32s BglMM_B_Connect_Open(machine)
     char* machine;
{
  I32s ret;
  BglSockSt	tmpS;

  tmpS = *UTIL_Sock_Pop_PreConnection();
  BglConnectTCP	= bgl_connectTCP(machine, tmpS.addr.sPortService );

  /* preconnection connection */
  if (BglConnectTCP < 0){
    tsprintf(bglStrBuf2, "Connection Control Aborted:\n host=%s service=%s",
	     machine, tmpS.addr.sPortService);
    BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
    return 1;	/* ERROR Return */
  }
  else{
    FD_ZERO(&BglConnectAfds);
    FD_SET(BglConnectTCP, &BglConnectAfds);
  }

  /* send request for preconnection */
  if (BglMM_Send(BglConnectTCP, &BglReqPreConn)){
    tsprintf(bglStrBuf2, "Preconnection was failed.");
    BglPrintf(BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
    return 2;	/* ERROR Return */
  }
  BglReplyMes(BglConnectTCP, tmpS.addr.service);

  /* wait for answer */
  ret = BglMM_B_ConnectReplyWait( BGL_PRE_CON_OK, BGL_PRE_CON_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request Pre-connection accepted."); 
    break;
  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request Pre-connection was refused.");
    if (!UTIL_Sock_GetCurrent_Connection()){
      return 3;	/* ERROR Return */
    }
    return 0;	/* NORMAL Return */
  default:
    if ( BglReplyBuf.ack == BGL_MIG_REQ_REF_NONET ){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
      if (!UTIL_Sock_GetCurrent_Connection()){
	return 4;	/* ERROR Return */
      }
      return 0;	/* NORMAL Return */
    }
    break;
  }

  /* Connection */
#ifdef NB_MECHANISM
  if ( BglMM_ConnectCom(&tmpS) >= 0 )
    UTIL_Sock_Push_Connection(&tmpS);
#endif /* NB_MECHANISM */
#ifdef B_MECHANISM
  BglMM_B_Connect();
#endif /* B_MECHANISM */

  /* close preconnection communication */
  shutdown(BglConnectTCP, 2);
  tclosesocket(BglConnectTCP);
  FD_CLR((I32u)BglConnectTCP, &BglConnectAfds);
  tsprintf(bglStrBuf2, "Connection Control Accepted:\n host=%s service=%s",
	   machine, tmpS.addr.sPortService);
  BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);  
  UTIL_Sock_Open_VirtualConnection();

  return 0;	/* NORMAL Return */
}

#ifdef B_MECHANISM
static I32s BglMM_B_Connect_ReOpen(machine, connection)
     char* machine;
     int connection;
{
  I32s ret;
  BglSockSt	tmpS;

  tmpS = *UTIL_Sock_Pop_PreConnection();
  BglConnectTCP	= bgl_connectTCP(machine, tmpS.addr.sPortService );

  /* preconnection connection */
  if (BglConnectTCP < 0){
    tsprintf(bglStrBuf2, "Connection Control Aborted:\n host=%s service=%s",
	     machine, tmpS.addr.sPortService);
    BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
    return 1; /* ERROR Return */
  }
  else{
    FD_ZERO(&BglConnectAfds);
    FD_SET(BglConnectTCP, &BglConnectAfds);
  }

  /* send request for preconnection */
  if (BglMM_Send(BglConnectTCP, &BglReqPreConn)){
    tsprintf(bglStrBuf2, "Preconnection was failed.");
    BglPrintf(BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);
    return 2; /* ERROR Return */
  }
  BglReplyMes(BglConnectTCP, tmpS.addr.service);

  /* wait for answer */
  ret = BglMM_B_ConnectReplyWait( BGL_PRE_CON_OK, BGL_PRE_CON_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request Pre-connection accepted."); 
    break;
  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request Pre-connection was refused."); 
    return 3; /* ERROR Return */
  default:
    if ( BglReplyBuf.ack == BGL_MIG_REQ_REF_NONET ){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
    return 4; /* ERROR Return */
    }
    break;
  }

  /* Connection */
  if (BglMM_B_ReConnect(connection) == MMFS_FAIL) return 5; /* ERROR Return */

  /* close preconnection communication */
  shutdown(BglConnectTCP, 2);
  tclosesocket(BglConnectTCP);
  FD_CLR(BglConnectTCP, &BglConnectAfds);
  tsprintf(bglStrBuf2, "Connection Control Accepted:\n host=%s service=%s",
	   machine, tmpS.addr.sPortService);
  BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf2);  
  UTIL_Sock_Open_VirtualConnection();

  return 0; /* NORMAL Return */
}
#endif /* B_MECHANISM */

  
/**********************************************************************
 * Connection functions
 */
static I32s	BglMM_ConnectCom(s)
     BglSockSt* s;
{
  I32s retCode;
  BglMesSt sendmes;
  I32s len;
  BglConnectSt	dataBuf;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  retCode = bgl_connectTCP(s->addr.dotted, s->addr.service);

  if( retCode < 0)    return retCode;

  FD_ZERO(&BglAfds);
  s->fd = retCode;
  DebugPrint1("s->fd = %d\n", (int)s->fd);
  FD_SET (s->fd, &BglAfds);

  if ( BglMM_Send(s->fd, &BglReqConnect) ) return -1; /* Communication Error */

  strcpy(dataBuf.host,   s->addr.host);
  strcpy(dataBuf.dotted, s->addr.dotted);
  strcpy(dataBuf.ver,    s->bglVersion);
  dataBuf.mode = s->user.priv;
  strcpy(dataBuf.name,   s->user.name);
  strcpy(dataBuf.passwd, s->user.passwd);

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_BglConnectSt(&msgHndl, &dataBuf)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR encode error in the function BglMM_ConnectCom()");
    return -2; /* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
  len = txdr_getpos(&msgHndl);
  len = BglMakeMesData( BGL_CONNECT, xdrMsgBuf, len, xdrMsgBuf2 );

  sendmes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11501);
  BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendmes);

  retCode = BglMM_Send(s->fd, &sendmes);
  tfree(sendmes.buf,BGL_MAX_MES_LEN, 11501);
  txdr_destroy(&msgHndl);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);

  if ( retCode ) return -3; /* Communication Error */

  BglReplyBuf.ack = BGL_ACK_DEFAULT;
  if ( ! BglMM_B_Recv(s->fd) ) return -4; /* Communication Error */

  retCode = BglMM_RecvConnect(s, BglReplyBuf.ack);
  if ( BglMM_Send(s->fd, &BglReqSelect)) return -5; /* Communication Error */
  MM_isReceived = 1;

  if ( retCode < 0 ){
    /* Connection refused by Tierra, Beagle must close TCP session */
    BglMM_ConnectionClose(s);
  }
  else{
    BglMM_Reply_modeReset();
    UTIL_Convert_Initialize();
  }
  return retCode;
}


static I32s	BglMM_ConnectionClose(s)
     BglSockSt* s;
{
  if ( s ){
    BglMM_CloseBglSockFD(s->fd);
    return 1; /* OK */
  }
  else{
    return 0; /* Argument Error */
  }
}

void	BglMM_CloseBglSockFD(fd)
     I32s fd;
{
  shutdown(fd, 2);
  tclosesocket(fd);
  FD_CLR((I32u)fd, &BglAfds);
}


static I32s	BglMM_RecvConnect(s, r)
     BglSockSt* s;
     I32s r;
{
  switch(r){
  case BGL_MIS_VERSION:
    tsprintf(bglStrBuf,
	     "Mismatch Version of Observational tool. Version = %s",
	     s->bglVersion);
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -11501;
    break;

  case BGL_OVR:
    tsprintf(bglStrBuf, "Bgl connection already established.");
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -11502;
    break;

  case BGL_SU_ACCEPTED:
    tsprintf(bglStrBuf, "Su Beagle is accepted");
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    return 0;
    break;

  case BGL_NONSU_ACCEPTED:
    tsprintf(bglStrBuf, "Non Su Beagle is accepted");
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    return 0;
    break;

  case BGL_WRONG_PASSWD:
    tsprintf(bglStrBuf,
	     "SU Beagle is rejected because incorrect user name or passwd.");
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -11503;
    break;

  case BGL_APP_REF:
    tsprintf(bglStrBuf,
	     "Connect to beagle was refused. Owner set disapproval.");
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -11504;
    break;

  case BGL_REF:
    tsprintf(bglStrBuf,
	     "Refused the connection to tierra (by unknown reason).");
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -11505;
    break;

  default:
    tsprintf(bglStrBuf, "Unknown error");
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -11506;
    break;
  }
}

#ifdef NB_MECHANISM
static I32s	BglMM_NB_ConnectReplyWait(repOk, repRef)
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
    ret = BglReplyWait(BglConnectTCP,
		       BglNfds, &BglConnectAfds, repOk, repRef, (double)0);

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
	timeCount = now.tv_sec +  1; /* because Bgl_wait1 ~ 1 */
      if ( now.tv_sec > (timeCount + Bgl_wait1) ){ /* can support polling cycles */
	BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		  "reply message could not reach on time. ignore this request.");
	timeCount = 0;
	return ret;		/* Time out */
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

static I32s BglMM_B_ConnectReplyWait(repOk, repRef)
     I32s repOk, repRef;
{
  I32s ret;

  while(1){
    ret = BglReplyWait(BglConnectTCP,
		       BglNfds, &BglConnectAfds, repOk, repRef, Bgl_wait1);

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
