/* bglnet.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      :
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 21:01:15 1999
 * Update Count    : 62
 * Status          : 
 */
/* Network routines of common beagle code                                  */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #103XX, in use:   10301 - 10303 */
/* BglError labels: #103XX, in use:   10301 - */

#include "bgl.h"

#ifdef BGL_SERV
extern I32s TBglSend P_((I32s, BglMesSt*));
#else
#include "clnt_global.h"
extern I32s BglMM_Send P_((I32s, BglMesSt*));/*beagleMessageMgr.c*/
#endif

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s BglSend(fd, sendbuf)
     const I32s fd;
     const BglMesSt* sendbuf;
{
  I32s ll;
  I32s cc = 0;

  memcpy(&ll, &sendbuf->buf[0], 4);
  DebugPrint1("Now, sending %d bytes message.\n", ntohl(ll) );
  cc = bglwrite(fd, &sendbuf->buf[0], sendbuf->len);

#ifdef DEBUG
  SendCount++;
  DebugPrint2("sent %d bytes message. SendCount = %d\n", cc, SendCount );
#endif /* DEBUG */

  return (cc);
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s BglRecv(fd)
     const I32s fd;
{
  I32s n, nread, nleft, len, retCode;
  I8s buf[5], tag;
  I8s *body;

  DebugPrint( "Now in BglRecv\n" );
  nread = 0;
  nleft = 5;
  while( nleft ){
    if ( ( n = trecv(fd, buf+nread, nleft, 0) ) <= 0 ){
      return n;
    }
    nread += n;
    nleft -= n;
  }

  tag = buf[4];
  memcpy(&len, &buf[0], 4);
  len = ntohl(len);

  body = (I8s*)tcalloc(1, len, 10301);

  if ( (tag != BGL_TAG_COM )&&(tag != BGL_TAG_DAT) ){
    tsprintf((char *)(&(bglStrBuf[0])),
        "BglRecv error. wrong TAG Code = \'0x%x\'\n", tag);
    BglError( -10301, NOEXIT, NOWRITE, &(bglStrBuf[0]) );
    bglread( fd, BGL_TAG_ERR, body, len ); /* Skip reading */
    retCode = -1;
  }

  else if( len < 0 || len > BGL_MAX_RECV_LEN ) {
    tsprintf((char *)(&(bglStrBuf[0])),
        "BglRecv error. Too large len (= %d)\n", len) ;
    BglError (-10301, NOEXIT, NOWRITE, &(bglStrBuf[0]));
    bglread( fd, BGL_TAG_ERR, body, len ); /* Skip reading */
    retCode =  -1;
  }

  else if ( (retCode = bglread (fd, tag, body, len)) < 0 ){
    BglError (-10301, NOEXIT, NOWRITE, (I8s *)"BglRecv error.");
  }

  tfree(body, len, 10301); 
#ifdef DEBUG
  RecvCount++;
#endif /* DEBUG */
  DebugPrint2( "BglRecv:len = %d RecvCount=%d\n",len,RecvCount );

  return retCode;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s BglReplyAck(fd, ack)
     const I32s fd;
     const I32s ack;
{
  BglMesSt sendmes;
  I32s len, cc;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  DebugPrint1( "ReplyAck = %d\n", ack );

  BglReplyBuf.ack = ack;

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 10304);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_BglReplySt(&msgHndl, &BglReplyBuf)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 10304);
    return 0; /* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 10305);
  len = txdr_getpos(&msgHndl);
  len = BglMakeMesData( BGL_REPLY, xdrMsgBuf, len, xdrMsgBuf2 );

  sendmes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 10302);
  BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendmes);
  txdr_destroy(&msgHndl);

#ifdef BGL_SERV
  cc = TBglSend(fd, &sendmes);
#else
  cc = BglMM_Send(fd, &sendmes);
#endif

  tfree(sendmes.buf, BGL_MAX_MES_LEN, 10302);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 10304);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 10305);

  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s BglReplyMes(fd, mes)
     const I32s fd;
     const I8s* mes;
{
  BglMesSt sendmes;
  I32s len, cc;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  DebugPrint1( "mes = %s\n", mes );

  tsprintf((char *)(&(BglReplyMesBuf.mes[0])), "%s", mes);

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 10304);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_BglReplyMesSt(&msgHndl, &BglReplyMesBuf)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 10304);
    return 0; /* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 10305);
  len = txdr_getpos(&msgHndl);
  len = BglMakeMesData( BGL_REPLYMES, xdrMsgBuf, len, xdrMsgBuf2 );

  sendmes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 10303);
  BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendmes);
  txdr_destroy(&msgHndl);

#ifdef BGL_SERV
  cc = TBglSend(fd, &sendmes);
#else
  cc = BglMM_Send(fd, &sendmes);
#endif

  tfree(sendmes.buf, BGL_MAX_MES_LEN, 10303);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 10304);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 10305);

  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s BglReplyWait(fd, nFds, aFdsP, repOk, repRef, timeOut)
     const I32s fd;
     const I32s nFds;
     const fd_set* aFdsP;
     const I32s repOk;
     const I32s repRef;
     const double timeOut;
{
  I32s ret;
  fd_set rFds;

  memcpy((I8s*)&rFds, (I8s*)aFdsP,  sizeof(rFds));
  ret = bgl_select( nFds, &rFds, timeOut );
  if( ret > 0){
    if ( FD_ISSET( fd, &rFds ) ){
      BglReplyBuf.ack = BGL_ACK_DEFAULT;
      if ( BglRecv( fd ) < 0  ){
	return BGL_REPLY_COM_ERR;	/* Communication Error */
      }
      if ( BglReplyBuf.ack == repOk ){
	return BGL_REPLY_RET_OK;	/* GOOD! (Normal return) */
      }
      else if (BglReplyBuf.ack == repRef){
	return BGL_REPLY_RET_REF;	/* Request REFUSED */
      }
      else if ( BglReplyBuf.ack == BGL_ACK_DEFAULT ){
	return BGL_REPLY_RET_NONE;	/* Read none */
      }
      else{
	return BGL_REPLY_RET_UNEXPECT;	/* unexpected reply data come from */
      }
    }
    else{
      return BGL_REPLY_RET_ILL_FD;	/* unexpected FD */
    }
  }
  else if ( ret == 0 ){
    return BGL_REPLY_RET_TIMEOUT;	/* message TIME OUT */
  }
  else{
    return BGL_REPLY_SELECT_ERR;	/* bgl_select return with -1 */
  }
}

