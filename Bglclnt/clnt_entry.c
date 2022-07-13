/* clnt_entry.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      :
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Oct 14 18:33:38 1999
 * Update Count    : 153
 * Status          :
 */
/* Entry functions of client beagle code                                   */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #113XX, in use:   */
/* BglError labels: #113XX, in use:   */

#include "clnt.h"
#include "beagleUI.h"


static BglSockSt BglTmpSock;
static I32s	 bglTmpNiceValue = 0;

static int	BglGetHostent P_((BglAddrSt*,I8s*));
static int	BglGetDefaultTierraAddr P_((BglAddrSt*, I8s*));
static int	BglGetDefaultTierraUser P_((BglUserSt*, char*));
static I8s	GetDefaultFromFile P_((I8s*));
static void	BglGetPortNumber P_((BglAddrSt*));
static void	BglInputUname P_((I8s*));
static void	BglInputPasswd P_((I8s*));
static void	BglPasswdEncode P_((I8s*, I8s*));
static void	BglPasswdDecode P_((I8s*, I8s*));

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglGetDefaultNiceValue()
{
  (void)GetDefaultFromFile(BGL_DEFAULT_FILE);
  return bglTmpNiceValue;
}



/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Parses the argument. 
 *
 * If the number of arguments is 0, 
 *    set the BglSockSt by BGL_DEFAULT_FILE.
 *
 * If the format of argument is like, "s://hostname"
 *    set hostname to the BglSockSt by argument
 *      and the other data set by BGL_DEFAULT_FILE.
 *
 * If the format of argument is like, "s://hostname:port"
 *    set hostname and standard communication port number to BglSockSt
 *      by argument, and the other data set by BGL_DEFAULT_FILE.
 *
 * If the format of argument is like, "s://hostname:port:sPort"
 *    set hostname and standard/short communication port number
 *      to the BglSockSt by this argment.
 *
 * If the format of argment is like "file",
 *    set the BglSockSt by "file".
 *
 * The outcome will be set to the second argment of this function 
 * "BglSockSt *s". 
 *
 * Return: The value of s->user->priv. This will distinguish whether
 *         beagle will start by Su mode, or Non Su mode.
 *         If the BglSockSt will not be set correctory, return -1.
 */  
I8s	BglParseArgs(buf, s)
     I8s* buf;
     BglSockSt* s;
{
  I32s len;
  I8s bglUrl[256], privStr[256], hostStr[256], portStr[256], sPortStr[256];
  I32s bglUrlLen;

  strcpy(s->bglVersion, BGL_VERSION);

  len = strlen(buf);
  while(len&&( (buf[len-1]=='\n')||(buf[len-1]==' ')||(buf[len-1]=='\t') ) ){
    buf[len-1] = '\0';
    len = strlen(buf);
  }

  if ( (buf == NULL)||(len == 0) ) {
    /* There are no argument.
     * This routine will read default variables from BGL_DEFAULT_FILE.
     */
    if ( BglGetDefaultTierraUser(&(s->user), BGL_DEFAULT_FILE) != 0 )
      return 'E';	/* ERROR RETURN */

    if ( strcmp(s->user.passwd, BGL_DEFAULT_PASSWD) != 0 )
      BglPasswdEncode(s->user.passwd, s->user.name);

    if( BglGetDefaultTierraAddr(&(s->addr), BGL_DEFAULT_FILE) != 0 )
      return 'E';	/* ERROR RETURN */

    if( BglGetHostent(&(s->addr), s->addr.host) != 0 )
      return 'E';	/* ERROR RETURN */

    return s->user.priv;	/* Normal return */
  }
  else if(strncmp(buf, "help",4) == 0) {
    /* If there is one argument only and that wrote "help"
     * then say usage message and return soon.
     */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
"Beagle URL: [<priv string>[://<hest name>[:<port number>[:<sPort number>]]]]\n\
or Connection script: connection_file_name\n\
 <priv string>: \"bgls\" | \"bgln\"\n\
 <port number>: long communication port number\n\
 <sPort number>: short communication port number");
    return 'H';	/* Wrote Help information */
  }


  strcpy( bglUrl, buf ); /* URL Style argument store to bglUrl. */
  bglUrlLen = strlen(bglUrl);
  if ( bglUrl[bglUrlLen-1] != '/' ){
    /* If there is not a '/' character at the end of argument then add it. */
    bglUrl[bglUrlLen] = '/';
    bglUrlLen += 1;
    bglUrl[bglUrlLen] = '\0';
  }

  /* Take out priv string (like a "bgls") from buf. */
  strcpy( privStr, buf );
  /* Chop after the ':' character from privStr if there is. */
  strtok( privStr, ":");

  if (strcmp(privStr,"bgls")!=0 && strcmp(privStr,"BGLS")!=0 &&
      strcmp(privStr,"bgln")!=0 && strcmp(privStr,"BGLN")!=0){
    /* Here is a connection script file name.
     * Then all connection variable read and set from script file.
     */
    if ( BglGetDefaultTierraUser(&(s->user), buf) != 0 )
      return 'E';	/* ERROR RETURN */

    if ( strcmp(s->user.passwd, BGL_DEFAULT_PASSWD) != 0 )
      BglPasswdEncode(s->user.passwd, s->user.name);

    if( BglGetDefaultTierraAddr(&(s->addr), buf) != 0 )
      return 'E';	/* ERROR RETURN */

    if( BglGetHostent(&(s->addr), s->addr.host) != 0 )
      return 'E';	/* ERROR RETURN */

    return s->user.priv;	/* Normal return */
  }

  else {
    /* Here is a URL style argument.
     * Then set default variable read and set from BGL_DEFAULT_FILE
     */
    if ( BglGetDefaultTierraUser(&(s->user), BGL_DEFAULT_FILE) != 0 )
      return 'E';	/* ERROR RETURN */

    if ( strcmp(s->user.passwd, BGL_DEFAULT_PASSWD) != 0 )
      BglPasswdEncode(s->user.passwd, s->user.name);

    if( BglGetDefaultTierraAddr(&(s->addr), BGL_DEFAULT_FILE) != 0 )
      return 'E';	/* ERROR RETURN */
  }

  /* Check priv string specify to SU mode or Non SU mode. */
  if (strcmp(privStr,"bgls")==0 || strcmp(privStr,"BGLS")==0)
    s->user.priv = BGL_PRIV_S;
  else
    s->user.priv = BGL_PRIV_N;

  portStr[0]  = '\0'; /* Initialize for Null string to port string area */

  if ( strlen(privStr)+3 <= strlen(bglUrl) ){
    /* If ther are not string "://" then return with ERROR code. */
    if ( ( bglUrl[strlen(privStr)+0] != ':' )||
	 ( bglUrl[strlen(privStr)+1] != '/' )||
	 ( bglUrl[strlen(privStr)+2] != '/' ) ){
      return 'E';	/* ERROR RETURN */
    }

    /* Take out host name string from bglUrl. (3 means "://") */
    strcpy( hostStr, bglUrl+strlen(privStr)+3 );

    if ( strlen(hostStr) ){
      if ( strpbrk( hostStr, ":" )){
	/* There is a port string.
	 * then chop after the ':' character from hostStr. */
	strtok( hostStr, ":" );

	/* Take out long communication port number string from bglUrl. */
	strcpy( portStr, bglUrl+strlen(privStr)+3+strlen(hostStr)+1 );

	if ( strpbrk( portStr, ":" )){
	  /* There is a sPort string.
	   * then chop after the ':' character from hostStr. */
	  strtok( portStr, ":" );

	  /* Take out short communicationport number string from bglUrl. */
	  strcpy(sPortStr,
		 bglUrl+strlen(privStr)+3+strlen(hostStr)+1+strlen(portStr)+1);

	  strtok( sPortStr, "/" );
	  s->addr.sPort = atoi(sPortStr);
	  strcpy(s->addr.sPortService, sPortStr );
	}
	else{
	  strtok( portStr, "/" );
	}
      }
      else{
	strtok( hostStr, "/" );
      }
      strcpy( s->addr.host, hostStr );
    }
  }


  if( BglGetHostent(&(s->addr), s->addr.host) != 0 )
    return 'E';/* ERROR RETURN */

  if(portStr[0] == '\0'){
    if ( s->addr.port == 0 ){
      BglGetPortNumber(&(s->addr));
    }
  }
  else{
    s->addr.port = atoi(portStr);
    tsprintf(s->addr.service, "%s", portStr);
  }

  return s->user.priv;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Get a value of BglAddrSt from Internet address.
 *
 * Return: If succussed, set BglAddrSt *addr and return with 0.
 *         If failed, return non 0;
 */ 
static int	BglGetHostent(aP, inp)
     BglAddrSt* aP;
     I8s *inp;
{
  struct hostent *hp;
  struct in_addr tmp;
  struct in_addr *ptr;
  I8s tmpdomain[64];

  tmp.s_addr = inet_addr(inp);
  if((hp = gethostbyaddr((char *)&tmp, sizeof(struct in_addr), AF_INET))
     == NULL){
    if((hp = tgethostbyname(inp)) == NULL) {
      tsprintf( bglStrBuf, "Can't get IP by %s\n", inp );
      BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
      return 1;
    } 
  } 

  ptr = (struct in_addr *)hp->h_addr_list[0];
  if(index(hp->h_name, '.') == 0) {
    (void)getdomainname(tmpdomain,sizeof(tmpdomain));
    tsprintf(aP->host , "%s.%s", hp->h_name, tmpdomain);
  }
  else {
    tsprintf(aP->host , "%s", hp->h_name);
  }
  tsprintf(aP->dotted, "%s", inet_ntoa(*ptr));
  aP->ip = ptr->s_addr;

  return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Get default Tierra's address from file "addr_fn".
 *
 * Return: If succussed, return BglAddrSt *addr.
 *         If failed, return NULL;
 */
static int	BglGetDefaultTierraAddr(aP, addr_fn)
     BglAddrSt* aP;
     I8s *addr_fn;
{
  strcpy( BglTmpSock.addr.host, "" );
  BglTmpSock.addr.port = 0;
  BglTmpSock.addr.sPort = atoi(BGL_DEFAULT_MIGRCTRL_PORT);
  strcpy( BglTmpSock.addr.sPortService, BGL_DEFAULT_MIGRCTRL_PORT);

  if ( GetDefaultFromFile(addr_fn) != 0 ) return 1;

  strcpy( aP->host, BglTmpSock.addr.host );
  aP->port = BglTmpSock.addr.port;
  strcpy( aP->service, BglTmpSock.addr.service);

  aP->sPort = BglTmpSock.addr.sPort;
  strcpy( aP->sPortService, BglTmpSock.addr.sPortService);

  return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Get default user name, passwd, priveledge from "user_fn".
 *
 * Return: If succussed, set BglUserSt *uP and return with 0.
 *         If failed, return non 0;
 */
static int	BglGetDefaultTierraUser(uP, user_fn)
     BglUserSt* uP;
     char *user_fn;
{
  strcpy(BglTmpSock.user.name, BGL_DEFAULT_UNAME);
  strcpy(BglTmpSock.user.passwd, BGL_DEFAULT_PASSWD);
  BglTmpSock.user.priv = BGL_PRIV_N;

  if ( GetDefaultFromFile(user_fn) != 0 ) return 1;

  strcpy(uP->name , BglTmpSock.user.name);
  strcpy(uP->passwd , BglTmpSock.user.passwd);
  uP->priv = BglTmpSock.user.priv;

  return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I8s	GetDefaultFromFile(user_fn)
     I8s* user_fn;
{
  FILE *inf;
  I8s Buff[84];
  I8s* strP;
#ifdef SIGBLOCK
  sigset_t BglSigSaveSet;

  BglSigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
  inf = tfopen(user_fn, "r");
  if (inf == NULL) {
    tsprintf(bglStrBuf, "File \"%s\" not opened\n", user_fn);
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
#ifdef SIGBLOCK
    TSigRelease(BglSigSaveSet);
#endif /* SIGBLOCK */
    return 1;
  }  

  tfgets(Buff, 84, inf);
  while (1) {
    strP = Buff;
    while( *strP == ' ' || *strP == '\t' ) strP++;

    switch (*strP) {
    case '#':
    case '\n':
      break;

    case 'b':
      if (!strncmp(strP, "beagleNice", 10)){
	sscanf(strP, "beagleNice = %d", &bglTmpNiceValue);
      }
      break;

    case 'd':
      if (!strncmp(strP, "dotted", 6)) {
	sscanf(strP, "dotted = %s", (char*)BglTmpSock.addr.dotted);
      }
      break;

    case 'h':
      if (!strncmp(strP, "host", 4)) {
	sscanf(strP, "host = %s", (char*)BglTmpSock.addr.host);
      }
      break;

    case 'i':
      if (!strncmp(strP, "ip", 2)) {
	sscanf(strP, "ip = %u", &BglTmpSock.addr.ip);
      }
      break;

    case 'p':
      if (!strncmp(strP, "passwd", 5)) {
	sscanf(strP, "passwd = %s", (char*)BglTmpSock.user.passwd);
      }
      else if (!strncmp(strP, "priv", 4)) {
	sscanf(strP, "priv = %c", &BglTmpSock.user.priv);
      }
      else if (!strncmp(strP, "port", 4)) {
	sscanf(strP, "port = %hu", &BglTmpSock.addr.port);
	tsprintf(BglTmpSock.addr.service, "%hu", BglTmpSock.addr.port);
      }
      break;

    case 'u':
      if (!strncmp(strP, "uname", 5)) {
	sscanf(strP, "uname = %s", (char*)BglTmpSock.user.name);
      }
      break;

    case 'm':
      if (!strncmp(strP, "migrctrlport", 12)) {
	sscanf(strP, "migrctrlport = %hu", &BglTmpSock.addr.sPort );
	tsprintf(BglTmpSock.addr.sPortService, "%hu", BglTmpSock.addr.sPort );
      }
      break;

    default:
      tsprintf(bglStrBuf, "Bad user_fn line: %s\n", Buff);
      BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, bglStrBuf);
      break;
    }
    tfgets(Buff, 84, inf);
    if (feof(inf)) break;
  }
  tfclose(inf);

#ifdef SIGBLOCK
  TSigRelease(BglSigSaveSet);
#endif /* SIGBLOCK */

  return 0;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void BglGetPortNumber(a)
     BglAddrSt* a;
{
  I8s input[20];

  tsprintf(bglStrBuf, "Input port number(default=%s) :", BGL_DEFAULT_PORT);
  strncpy(input, BglUI_StrInput(bglStrBuf), 20);
  if ( (strlen(input) == 0)||(input[0]=='\n') ){
    a->port = atoi(BGL_DEFAULT_PORT);
    tsprintf(a->service, "%s", BGL_DEFAULT_PORT);
  }
  else{
    a->port = atoi(input);
    tsprintf(a->service, "%s", input);
  }
  return;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglGetUname(inpname)
     I8s *inpname;
{
  I8s tmpbuf[85];
  if ( strcmp(inpname, BGL_DEFAULT_UNAME) == 0 ) {
    BglInputUname(&tmpbuf[0]);
    strcpy(inpname, &tmpbuf[0]);
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void	BglInputUname(buf)
     I8s* buf;
{	
  tsprintf(buf, "%s", BglUI_StrInput("Input your E-mail address:") );
  if( buf[strlen(buf)-1] == '\n') buf[strlen(buf)-1] = (char)NULL;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglGetPasswd(u)
     BglUserSt* u;
{
  if ( strcmp(u->passwd, BGL_DEFAULT_PASSWD) ==0 ) {
    BglInputPasswd(u->passwd);
    BglPasswdEncode(u->passwd, u->name);
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void	BglInputPasswd(input)
     I8s* input;
{
  tsprintf(input, "%s", BglUI_SecretStrInput("Input passwd :") );
  if( input[strlen(input)-1] == '\n') input[strlen(input)-1] = (char)NULL;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void	BglPasswdEncode(input, name)
     I8s* input;
     I8s* name;
{
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void	BglPasswdDecode(input, name)
     I8s* input;
     I8s* name;
{
  I32s i;

  for (i = 0; i < strlen(input); i++){
    input[i] = (I8s)((I32s)input[i] - (I32s)name[i]%10) ;
  }
  DebugPrint1("Decode input = %s\n", input);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
#ifdef TBSD
struct sgttyb tty_mode;  /* save tty mode here */

I32s	BglTtyRaw(fd)
     I32s fd;
{
  struct sgttyb temp_mode;
  if (ioctl(fd, TIOCGETP, (char *)&temp_mode) < 0) {
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
	     "ioctl TIOCGETP error. Your input passwd will be showed.\n");
    return(-1);
  }
  tty_mode = temp_mode;       /* save for restoring later */
  temp_mode.sg_flags &= ~ECHO;    /* turn ECHO off */
  if (ioctl(fd, TIOCSETP, (char *)&temp_mode) < 0) {
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
	      "ioctl TIOCSETP error\n");
    return (-1);
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
/*
 * Reset tty mode for BSD
 */
I32s	BglTtyReset(fd)
     I32s fd;
{
  if (ioctl(fd, TIOCSETP, (char *)&tty_mode) < 0)        return(-1);
  return 0; /* tentative */
}
#endif /* TBSD */


#ifdef TSysV
static struct termio tty_mode; /* save tty mode here */

I32s	BglTtyRaw(fd)
     I32s fd;
     /* of terminal device */
{
  struct termio temp_mode;

  if (ioctl(fd, TCGETA, (char *)&temp_mode) < 0) {
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
	     "ioctl TCGETA error. Your input passwd will be showed.\n");
    return(-1);
  }
  tty_mode = temp_mode;       /* save for restoring later */
  temp_mode.c_lflag &= ~ECHO;
  if (ioctl(fd, TCSETA, (char *)&temp_mode) < 0) {
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
	      "ioctl TIOCSETP error\n");
    return (-1);
  }
  return(0);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Reset tty mode for SysV
 */
I32s	BglTtyReset(fd)
     I32s fd;
     /* of terminal device */
{
  if (ioctl(fd, TCSETA, (char *)&tty_mode) < 0)
    return(-1);

  return (0);
}

#endif /* TSysV */
