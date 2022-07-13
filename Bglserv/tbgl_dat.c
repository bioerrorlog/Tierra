/* tbgl_dat.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 10:41:11 1999
 * Update Count    : 63
 * Status          :
 */

/* Data decode of functions for hooking the beagle code                    */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf labels: #122XX, in use:   */
/* FEError labels: #122XX, in use:   */


#include "license.h"
#include "tierra.h"
#include "globals.h"

#include "bgl.h"
#include "tbgl.h"

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_ABORT(buf)
     I8s* buf;
{
  DebugPrint( "/*** do_BGL_ABORT ***/\n" );
  return 1;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_REPLY(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_REPLY ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglReplySt(&msgHndl, &BglReplyBuf) ){
    n = 0;	/* Decode ERROR */
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR decode error on the function do_BGL_REPLY()");
    TBgl_make_femessages(bglStrBuf,
        strlen((const char *)(&(bglStrBuf[0]))));
  }
  else{
    n = txdr_getpos(&msgHndl);
  }
  txdr_destroy(&msgHndl);
  DebugPrint1("BglReplyBuf.ack=%d\n", BglReplyBuf.ack);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_REPLYMES(buf)
     I8s* buf;
{  
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_REPLYMES ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglReplyMesSt(&msgHndl, &BglReplyMesBuf) ){
    n = 0;	/* Decode ERROR */
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR decode error on the function do_BGL_REPLYMES()");
    TBgl_make_femessages(bglStrBuf,strlen(
        (const char *)(&(bglStrBuf[0]))));
  }
  else{
    n = txdr_getpos(&msgHndl);
  }
  txdr_destroy(&msgHndl);
  DebugPrint1("do_BGL_REPLYMES : buf = %s", BglReplyMesBuf.mes);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	do_BGL_CONNECT(buf)
     I8s* buf;
{
  XDR msgHndl;
  BglConnectSt	dataBuf;
  I32s n;

  DebugPrint( "/*** do_BGL_CONNECT ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglConnectSt(&msgHndl, &dataBuf) ){
    txdr_destroy(&msgHndl);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR decode error on the function do_BGL_CONNECT()");
    TBgl_make_femessages(bglStrBuf,
        strlen((const char *)(&(bglStrBuf[0]))));
    return 0;	/* Decode ERROR */
  }
  n = txdr_getpos(&msgHndl);
  txdr_destroy(&msgHndl);

  strcpy((char *)(&(BglSockBuf[SockBufSize-1].addr.host[0])),
      (const char *)(&(dataBuf.host[0])));
  DebugPrint1("host=%s\n", BglSockBuf[SockBufSize-1].addr.host);

  strcpy((char *)(&(BglSockBuf[SockBufSize-1].addr.dotted[0])),
      (const char *)(&(dataBuf.dotted[0])));
  DebugPrint1("dotted=%s\n", BglSockBuf[SockBufSize-1].addr.dotted);

  strcpy((char *)(&(BglSockBuf[SockBufSize-1].bglVersion[0])),
      (const char *)(&(dataBuf.ver[0])));
  DebugPrint1("version=%s\n", BglSockBuf[SockBufSize-1].bglVersion);

  BglSockBuf[SockBufSize-1].user.priv = dataBuf.mode;
  DebugPrint1("mode=%c\n", BglSockBuf[SockBufSize-1].user.priv);

  strcpy((char *)(&(BglSockBuf[SockBufSize-1].user.name[0])),
      (const char *)(&(dataBuf.name[0])));
  DebugPrint1("s->user.name=%s\n", BglSockBuf[SockBufSize-1].user.name);

  strcpy((char *)(&(BglSockBuf[SockBufSize-1].user.passwd[0])),
      (const char *)(&(dataBuf.passwd[0])));
  DebugPrint1("s->user.passwd=%s\n", BglSockBuf[SockBufSize-1].user.passwd);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_SAVE_CODE(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_SAVE_CODE ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_SaveCodeSt(&msgHndl, &SaveBuf) ){
    txdr_destroy(&msgHndl);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR decode error on the function do_BGL_SAVE_CODE()");
    TBgl_make_femessages(bglStrBuf,
        strlen((const char *)(&(bglStrBuf[0]))));
    return 0;	/* Decode ERROR */
  }
  n = txdr_getpos(&msgHndl);
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_BRK(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_BRK ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BrkPointSt(&msgHndl, &BglBrkPointBuf) ){
    txdr_destroy(&msgHndl);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR decode error on the function do_BGL_BRK()");
    TBgl_make_femessages(bglStrBuf,
        strlen((const char *)(&(bglStrBuf[0]))));
    return 0;	/* Decode ERROR */
  }
  n = txdr_getpos(&msgHndl);
  txdr_destroy(&msgHndl);
  
  return n;
}


#ifdef NET
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_MIGR_CTRL(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_MIGR_CTRL ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglMigrationConnectSt(&msgHndl, &BglMigrationConnectBuf) ){
    txdr_destroy(&msgHndl);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR decode error on the function do_BGL_MIGR_CTRL()");
    TBgl_make_femessages(bglStrBuf,
        strlen((unsigned char *)(&(bglStrBuf[0]))));
    return 0;	/* Decode ERROR */
  }
  n = txdr_getpos(&msgHndl);
  txdr_destroy(&msgHndl);

  BglMigrationConnectBuf.portnb = (N16u)ntohs(BglMigrationConnectBuf.portnb);

  return n;
}
#endif /* NET */


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_INJECT(buf)
	I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_INJECT ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglInjectGeneSt(&msgHndl, &BglInjectGeneBuf) ){
    txdr_destroy(&msgHndl);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR decode error on the function do_BGL_INJECT()");
    TBgl_make_femessages(bglStrBuf,
        strlen((const char *)(&(bglStrBuf[0]))));
    return 0;	/* Decode ERROR */
  }
  n = txdr_getpos(&msgHndl);
  txdr_destroy(&msgHndl);
  
  return n;
}
