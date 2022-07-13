/* bgltcp.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri Aug 13 10:07:28 1999
 * Update Count    : 86
 * Status          : 
 */
/* Socket related functions of common beagle code                          */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #106XX, in use:   */
/* BglError labels: #106XX, in use:   10601 - 10616 */

#include "bgl.h"
#include "bglprint.h"

static I32s bgl_connectsock	P_((I8s*, I8s*, I8s*));
static I32s bgl_passivesock	P_((I8s*, I8s*, I32s));


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * bgl_onnectTCP.c
 */
I32s bgl_connectTCP(host, service)
     I8s* host;
     I8s* service;
{
  return bgl_connectsock(host, service, (I8s *)"tcp");
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * bgl_connectUDP.c
 */
I32s bgl_connectUDP(host, service)
     I8s* host;
     I8s* service;
{
  return bgl_connectsock(host, service, (I8s *)"udp");
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * bgl_connectsock.c
 */
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif /* INADDR_NONE */

static I32s bgl_connectsock(host, service, protocol)
     I8s* host;
     I8s* service;
     I8s* protocol;
{
  struct hostent *phe;
  struct servent *pse;
  struct protoent *ppe;
  struct sockaddr_in sIn;
  I32s s, type;

  tsprintf((char *)(&(bglStrBuf[0])),
      "Trying to connect:\n host=%s service=%s", host,service);
  BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X,
      BGL_ROOT_Y, 1, 0, &(bglStrBuf[0]));

  memset ((I8s *) &sIn, 0, sizeof(sIn));
  sIn.sin_family = AF_INET;

  if ( (pse = tgetservbyname(service, protocol)) != (struct servent*)0 )
    sIn.sin_port = pse->s_port;
  else if ((sIn.sin_port = htons ((u_short)
    atoi ((char *)service))) == 0) {
    tsprintf((char *)(&(bglStrBuf[0])),
        "can't get %s service entry", service);
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X,
        BGL_ERRMES_Y, 1, -1, &(bglStrBuf[0]));
    return -10601;
  }

  if ( (phe = tgethostbyname(host)) != (struct hostent*)0 )
    memcpy ((I8s *) &sIn.sin_addr, phe->h_addr, phe->h_length);
  else if ((sIn.sin_addr.s_addr = inet_addr ((char *)host)) == INADDR_NONE) {
    tsprintf((char *)(&(bglStrBuf[0])), "can't get %s host entry", host);
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X,
        BGL_ERRMES_Y, 1, -1, &(bglStrBuf[0]));
    return -10602;
  }

  if ((ppe = tgetprotobyname(protocol)) == 0) {
    tsprintf((char *)(&(bglStrBuf[0])),
        "can't get %s protocol entry", protocol);
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -10603;
  }

  if (strcmp((const char *)protocol,(const char *)"udp")==0)
      type = SOCK_DGRAM;
  else					type = SOCK_STREAM;

  s = tsocket(PF_INET, type, ppe->p_proto);
  if (s < 0) {
    tsprintf((char *)(&(bglStrBuf[0])), "can't create socket");
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -10604;
  }

  if ( tconnect(s, (struct sockaddr *)&sIn, sizeof(sIn),1) < 0) {
    tsprintf((char *)(&(bglStrBuf[0])),
        "can't connect to %s:%s", host, service);
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
    return -10605;
  }
  tsprintf((char *)(&(bglStrBuf[0])),
      "Connected:\n host=%s service=%s", host,service);
  BglPrintf (BGL_ROOT_SCR, BGL_ROOT_X, BGL_ROOT_Y, 1, 0, bglStrBuf);
  
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
 * bgl_passiveTCP.c
 */
I32s bgl_passiveTCP(service, qlen)
     I8s* service;
     I32s qlen;
{
  return bgl_passivesock (service, (I8s *)"tcp", qlen);
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * bgl_passivesock.c
 */
I16u portbase = 0;
static I32s bgl_passivesock(service, protocol, qlen)
     I8s* service;
     I8s* protocol;
     I32s qlen;
{
  struct protoent* ppe;
  struct sockaddr_in sIn;
  I32s s, type;
  char hostname[MAXHOSTNAME+1];
  struct hostent *hostentbuf;

  memset ((I8s *) &sIn, 0, sizeof(sIn));

  sIn.sin_port=htons((u_short)atoi((char *)service));
  if(!(tgethostname((I8s *)(&(hostname[0])), sizeof(hostname))))
      if((hostentbuf=tgethostbyname((I8s *)(&(hostname[0])))))
      {   sIn.sin_family=hostentbuf->h_addrtype;
          sIn.sin_addr.s_addr=((struct in_addr *)
              &(hostentbuf->h_addr_list[0][0]))->s_addr;
      }
      else
      {   tsprintf((char *)(&(bglStrBuf[0])), "can't get host by name");
          BglError (-10615, BGL_EXIT, BGL_NOWRITE, bglStrBuf);
      }
  else
  {   tsprintf((char *)(&(bglStrBuf[0])), "can't get own name");
      BglError (-10616, BGL_EXIT, BGL_NOWRITE, bglStrBuf);
  }

  if ((ppe = tgetprotobyname(protocol)) == 0) {
    tsprintf((char *)(&(bglStrBuf[0])),
        "can't get %s protocol entry", protocol);
    BglError (-10607, BGL_EXIT, BGL_NOWRITE, bglStrBuf);
  }

  if (strcmp ((const char *)protocol,
      (const char *)"udp") == 0)    type = SOCK_DGRAM;
  else					type = SOCK_STREAM;

  s = tsocket(PF_INET, type, ppe->p_proto);
  if (s < 0) {
    tsprintf((char *)(&(bglStrBuf[0])), "can't create socket");
    BglError (-10608, BGL_EXIT, BGL_NOWRITE, bglStrBuf);
  }

  {
    I32s false = 1;
    if ( setsockopt(s, SOL_SOCKET, SO_REUSEADDR, 
		    (char *)&false, sizeof(false))) {
      tsprintf((char *)(&(bglStrBuf[0])), "set option failed");
      BglError (-10609, BGL_EXIT, BGL_NOWRITE, bglStrBuf);
    }
  }

  if ( tbind(s, (struct sockaddr *) &sIn, sizeof(sIn)) < 0) {
    tsprintf((char *)(&(bglStrBuf[0])), "can't bind %s port\n", service);
    BglError (-10610, BGL_EXIT, BGL_NOWRITE, bglStrBuf);
    return -1;
  }
  if ((type == SOCK_STREAM) && (listen (s, qlen) < 0)) {
    tsprintf((char *)(&(bglStrBuf[0])),
        "can't listen on %s port\n", service);
    BglError (-10611, BGL_EXIT, BGL_NOWRITE, bglStrBuf);
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
 * I32s bgl_open_max();
 */
I32s bgl_open_max()
{
#ifdef OPEN_MAX
  I32s openmax = OPEN_MAX;
#else
  I32s openmax = 0;
#endif

  if(openmax == 0) {  /* first time through */
#ifdef _WIN32
    openmax = FOPEN_MAX;
#else
    if((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
      if(errno == 0)
	openmax = OPEN_MAX_GUESS;   /* It's indeterminate */
      else {
	tsprintf((char *)(&(bglStrBuf[0])),
            "sysconf error for _SC_OPEN_MAX");
	BglError(-10612, BGL_EXIT, BGL_NOWRITE, bglStrBuf);
      }
    }
#endif
  }
  return (openmax);
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * I32s bgl_select();
 */
I32s bgl_select(width, readfds, timeout)
     const I32s width;
     fd_set* readfds;
     const double timeout;
{
  I32s nselect = 0;
  struct timeval w;

  if( timeout >= (double)0 ) {
    w.tv_sec = (I32s)timeout;
    w.tv_usec = (I32s)((timeout - (float)w.tv_sec)*1000000);
    nselect = (I32s)tselect(width, readfds, (fd_set *)0, (fd_set *)0,
			    (struct timeval *)&w);
  }
  else{
    nselect = (I32s)tselect(width, readfds, (fd_set *)0, (fd_set *)0, 
			    (struct timeval *)0);
  }

  return nselect;
} 



/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * I32s bglwrite(); low level write func. for monitor tool.
 */
I32s bglwrite(fd, buf, len)
     const I32s fd;
     I8s* buf;
     const I32s len;
{
  I32s n, nwrite, nleft;

  nwrite = 0;
  nleft = len;
  while (nleft > 0) {
    n = tsend(fd, buf + nwrite, nleft, 0);
    if (n == 0)
      break;
    if (n > 0) {
      nwrite += n;
      nleft -= n;
    }
    else{
      tsprintf((char *)(&(bglStrBuf[0])),
          "Error in bglwrite() errno=%d\n", errno);
      BglError(-10613, BGL_NOEXIT, BGL_NOWRITE, bglStrBuf);
      return (n);
    }
  }
  return (nwrite);
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * I32s bglread(); low level read func. for monitor tool.
 */
I32s bglread(fd, tag, buf, len)
     const I32s fd;
     const I32s tag;
     I8s* buf;
     const I32s len;
{
  I32s n, nread = 0, nleft;

  nleft = len;
  while( nleft ){
    if ( ( n = trecv(fd, buf+nread, nleft, 0) ) < 0 ){
      BglError (-10301, NOEXIT, NOWRITE, (I8s *)"bglread error.");
      return -1;
    }
    nread += n;
    nleft -= n;
  }

  if (tag == BGL_TAG_COM) {
    BglComDecode(buf, len);
  }
  else if(tag == BGL_TAG_DAT) {
    BglDatDecode(buf, len);
  }

  return nread;
}
