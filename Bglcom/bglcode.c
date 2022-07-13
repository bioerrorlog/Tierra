/* bglcode.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 10:34:07 1999
 * Update Count    : 107
 * Status          : 
 */

/* Encode and Decode routines of common beagle code                        */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #102XX, in use:   10201 */
/* BglError labels: #102XX, in use:   */

#include "bgl.h"

static I32s fsminit	P_((void));
static I32s ddinit	P_((void));
static I32s BglDoCom  P_((I32s));
static I32s BglDoDat	P_((I32s, I8s*));

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */

/*
 * Build a Finite State Machine for command
 */
I32s fsmbuild()
{ 
  ttstate = MSSELECT;
  return fsminit();
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Initialize Finite State Machine
 */
static I32s fsminit()
{
  struct fsm_trans* pt;
  I32s sn, cn;
  I32u ti, tisv;

  for (cn = 0; cn < NCHRS; cn++)
    for (ti = 0; ti < NCHRS; ti++)
      ttfsm[ti][cn] = TINVALID;

  for (ti = 0; ttstab[ti].ft_state != FSINVALID; ++ti) {
    DebugPrint4( "ttab[%d].ft_state = %d ft_char=%d ft_next = %d \n",
		 ti,ttstab[ti].ft_state,ttstab[ti].ft_char, ttstab[ti].ft_next  );
    pt = &ttstab[ti];
    sn = pt->ft_state;
    if (pt->ft_char == MCANY) {
      for (cn = 0; cn < NCHRS; ++cn)
	if (ttfsm[sn][cn] == TINVALID)
	  ttfsm[sn][cn] = ti;
    }
    else
      ttfsm[sn][pt->ft_char] = ti;
  }

  tisv = ti;
  /* set all uninitialized indices to an invalid transition */
  for (cn = 0; cn < NCHRS; ++cn)
    for (ti = 0; ti < NCHRS; ++ti)
      if (ttfsm[ti][cn] == TINVALID)
        ttfsm[ti][cn] = tisv;

  return 0; /* tentative */
}


/*
 * Build a Finite State Machine for data
 */
I32s ddbuild()
{ 
  return ddinit();
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Build Data State
 */
static I32s ddinit()
{
  I32u i;

  for (i = 0; i < NCHRS; ++i)
    dfsm[i] = TINVALID;

  for (i = 0; ddstab[i].data_type != BGL_INVALID; ++i) {
    dfsm[ddstab[i].data_type] = i;
    DebugPrint4( "ddstab[%d].data_type = %d fsm[%d]=%d\n",
                 i, ddstab[i].data_type, ddstab[i].data_type,
                 dfsm[ddstab[i].data_type]);
  }
  for (i = 0; i < NCHRS; ++i) {
    if (dfsm[i] != 0){
      DebugPrint3( "dfsm[%d] = %d addr = %d\n",(int)i,dfsm[i],(int)&dfsm[i]);
    }
  }
  return 0; /* tentative */
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s BglMakeMesData(dataLabel, buf, len, data)
     const I32s dataLabel;
     const I8s* buf;
     const I32s len;
     I8s* data;
{
  I32s nLabel;

  nLabel = htonl(dataLabel);
  memcpy(data, &nLabel, 4);
  memcpy(&data[4], buf, len);
  return len + 4;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s BglMakeMes(tag, buf, len, sendbuf)
     const I32s tag;
     const I8s* buf;
     const I32s len;
     BglMesSt* sendbuf;
{
  I32s nlen;
  sendbuf->len = len + BGL_MES_HEADER_LEN; 
  nlen =  htonl(len);
  memcpy(&sendbuf->buf[0], &nlen, BGL_MES_HEADER_LEN-1);
  sendbuf->buf[BGL_MES_HEADER_LEN-1] = (char)tag;
  memcpy(&sendbuf->buf[BGL_MES_HEADER_LEN], buf, len);
  return 0; /* tentative */
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglComDecode( buf, n )
     const I8s* buf;
     const I32s n;
{
  I32s i;
  I32s com;
  for(i=0; i < n/4; i++) {
    memcpy(&com, buf + i*4, 4);
    com = ntohl( com );
    DebugPrint1("com = %d\n", com);
    BglDoCom( com );
  }	
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s BglDoCom(com)
I32s com;
{
  struct fsm_trans *pt;
  I32s ti, cc;

  DebugPrint("BglDoCom\n");
  ti = ttfsm[ttstate][com];
  pt = &ttstab[ti];
  DebugPrint1( "ttstate = %d\n", ttstate );
  ttstate = pt->ft_next;
  cc = (pt->ft_action)();
  return cc;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglDatDecode( buf, buflen )
     I8s* buf;
     const I32u buflen;
{
  I8s *c;
  I32s ndecode=buflen;
  I32s label;
  I32s n;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  c = buf;
  while(ndecode > 0) {
    memcpy(&label, c, 4);
    label = ntohl(label);
    ndecode -= 4;
    c += 4;

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if ( !( n = BglDoDat(label, c) ) ){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      return;/* Decode ERROR */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    c += n;
    ndecode -= n;
  }  /* END of while(ndecode) LOOP */

  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s BglDoDat(label, buf)
I32s label;
I8s* buf;
{
  struct data_decode *pt;

  DebugPrint("BglDoDat\n");
  pt = &ddstab[dfsm[label]];
  return (pt->do_xdr_function)( buf );
}
