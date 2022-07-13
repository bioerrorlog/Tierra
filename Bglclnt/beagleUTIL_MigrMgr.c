/* beagleUTIL_MigrMgr.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Fri Aug  1 14:14:53 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 10:37:27 1998
 * Update Count    : 141
 * Status          :
 *          Migration utility
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleUTIL_MigrMgr.c	1.23	11/12/98 10:37:46";
#endif /* lint */

#include "clnt.h"
#include "beagleUTIL_MigrMgr.h"
#include "beagleUTIL_SockMgr.h"


/*
 * ranID control declarations
 */
#define MAX_RANID_BUF 200
#define MAX_RANID_GAR 90
struct ranID_s {
  I32s session;
  I32s ranID;
  int  migrtime;
};

static struct ranID_s  ranIDlist[MAX_RANID_BUF];
static int             ranIDused;
static int             ranIDfirsttime, ranIDlasttime;

static int UTIL_MigrMgr_CtrlranIDPut	P_((I32s, I32s));
static int UTIL_MigrMgr_CtrlranIDGet	P_((I32s, I32s));
static void ranIDgarbagecollector	P_((void));

/*
 * variable for writing file
 */
static const int   MIGRF_MAX_SIZE = 1000;
static       int   n              = 0;
static       int   filenumber     = 0;
static       FILE *output;


/* Global variable for storing the last node which sent migr info to Bealge */
N32u UTIL_MigrMgr_LastNode;

/* temporary : migration control port and migration data port */
static char MigrCtrlPort[7] = "";
static char MigrPort[7]     = "";

/* variable for timeout */
static struct timeval LastMigrDataIn;

/* variables for session control */
static I32s CurrentSession = 0, LastReceveidSession;

/* variable for sleeping and apocalypse control */
static int  StoppingRequest    = 0;

/* variable for parameter for migration tracking */
static char CurrentParameter[8];
static int  TrackedCreatureMutated;





/************************************************************
 * timeout control
 */

/* put the LastMigrDataIn to the current time */
void UTIL_MigrMgr_TimeoutInit()
{
  tgettimeofday(&LastMigrDataIn, NULL);
}



/* compare LastMigrDataIn + Timeout with current time */
int  UTIL_MigrMgr_TimeoutChek()
{
  struct timeval now;

  tgettimeofday(&now, NULL);
  return (now.tv_sec - LastMigrDataIn.tv_sec) < Bgl_wait2;
  /*Bgl_wait2 is for tracking timeout (see clnt_global.h) */
}

/************************************************************
 * session control
 */

/* give a new number session */
I32s UTIL_MigrMgr_NewSession()
{
  return ++CurrentSession;
}

/* get the session number of the last migration data in */
void UTIL_MigrMgr_Checksession(session)
     I32s session;
{
  LastReceveidSession = session;
}

/* compare CurrentSession with LastReceivedSession */
int  UTIL_MigrMgr_ValidMigrSession()
{
  return LastReceveidSession == CurrentSession;
}


/************************************************************
 * sleeping and apocalypse control
 */

/* put the stopping node to 0 */
void UTIL_MigrMgr_TrackingStopReset()
{
  StoppingRequest = 0;
}

/* store the node of the last traking stop request */
void UTIL_MigrMgr_TrackingStopRequest()
{
  StoppingRequest = 1;
}

/* say if a stoping request have been done*/
int UTIL_MigrMgr_StopRequestCheck()
{
  return StoppingRequest > 0;
}



/************************************************************
 * migration control
 */

/* put parameter and initialize the buffer */
void UTIL_MigrMgr_PutParameter(parameter)
     I8s parameter[8];
{
  strcpy(CurrentParameter, parameter);
}

/* return the value of a node number for controling tracking */
N32u UTIL_MigrMgr_BuffCtrlRead(localparam)
     I8s localparam[8];
{
  strcpy(localparam, CurrentParameter);
  TrackedCreatureMutated = 0;	/* the creature didn't mutate */
  return UTIL_Sock_GetCurrent_Connection()->addr.ip;
}


/* check if creature muteted during its life on one machine */
int UTIL_MigrMgr_CheckCreatureName(info, localparameter)
     I8s info;
     I8s localparameter[8];
{
  static I8s LastCreatureName[8];
  
  if (!UTIL_MigrMgr_ValidMigrSession())    return 0;

  switch (info) {
  case 'B':
  case 'I':
    strcpy(LastCreatureName, localparameter);
    break;
  case 'M':
  case 'D':
  case 'A':
  case 'S':
    TrackedCreatureMutated = TrackedCreatureMutated
      || strcmp(localparameter, LastCreatureName);
    break;
  default:
    break;
  }

  return ! TrackedCreatureMutated;
}




/* Initialize the value of the migration control port */
void UTIL_MigrMgr_CtrlPortInit(portnb)
     char* portnb;
{
  strcpy(MigrCtrlPort, portnb);
  if ( !strlen(MigrPort) ){
    /* WE MUST NOT CHANGE this value from at start time */
    strcpy(MigrPort, portnb);
    UTIL_MigrMgr_PortChange();
  }
}


/* return the value of the migration control port */
char *UTIL_MigrMgr_CtrlPortGet()
{
  return MigrCtrlPort;
}


/* function to write datas into a file*/
int UTIL_MigrMgr_Write(d)
     BglMigrationDataSt* d;
{
  char  filename[15];
  int   test = 1;

  if ( n == 0){ /* we open a new file */
    do{   /* compose file name */
      tsprintf(filename, "Migr%04d.bgl", filenumber++);
      if (output = tfopen(filename, "r"))
	tfclose(output);
    }
    while (output);
    if (! (output = tfopen(filename, "w")))
      return 0;
  }

  /* we add the new migration information into the file */
  switch (d->information){
  case 'B':
  case 'R':
    test = test && UTIL_MigrMgr_CtrlranIDPut(d->session, d->ranID);
    break;
  case 'I':
    test = test && UTIL_MigrMgr_CtrlranIDGet(d->session, d->ranID);
    test = test && UTIL_MigrMgr_CtrlranIDPut(d->session, d->ranID);
    break;

  case 'M':
    test = test && UTIL_MigrMgr_CtrlranIDGet(d->session, d->ranID);
    test = test && UTIL_MigrMgr_CtrlranIDPut(d->session, d->ranID);
    break;
  case 'D':
  case 'A':
  case 'S':
    test = test && UTIL_MigrMgr_CtrlranIDGet(d->session, d->ranID);
    break;
  }

/*   tfprintf( stdout, "%s\n", d->localparameter); */
/*   tfprintf( stdout, "%c %15s %5d %10d %15d %5d %15d\n", */
/* 	 d->information, inet_ntoa(*((struct in_addr *)(&(d->node)))), */
/* 	 d->session, d->ranID, d->tierraTime, d->fecundity, d->beagleTime); */
  
  if (test){
    tfprintf(output, "%c %15s %5d %10d %15d %5d %15d\n",
	     d->information, inet_ntoa(*((struct in_addr *)(&(d->node)))),
	     d->session, d->ranID, d->tierraTime, d->fecundity, d->beagleTime);
    n ++;
  }

  /* look if the file is empty and close it if it is */
  if (n == MIGRF_MAX_SIZE){
    tfclose(output);
    n = 0;
  }

  return (test ? 1 : -1);
}


/* close the last file */
void UTIL_MigrMgr_Close()
{
  if (output){
    tfclose(output);
    n = 0;
  }
}


/*********************************************************************************/
/* ranID control
 */

static int UTIL_MigrMgr_CtrlranIDPut(session, ranID)
     I32s session, ranID;
{
  int i;

  /* look for the session into the buffer */
  for (i = ranIDused - 1 ; (i >= 0)&&(ranIDlist[i].session - session) ; i--);

  while ((i < 0) || (i == ranIDused)){
    /* look for an empty space */
    for (i = ranIDused - 1 ; (i >= 0) && (ranIDlist[i].session) ; i--);
    i = i < 0 ? ranIDused : i;

    if (i < MAX_RANID_BUF){
      ranIDlist[i].session   = session;
      ranIDlist[i].ranID     = ranID;
      ranIDlist[i].migrtime  = ranIDlasttime++;
      ranIDused              = (++i) < ranIDused ? ranIDused : i;
      return 1;
    }
    else
      ranIDgarbagecollector();
  }

  /* this session is already defined into buffer */
  return 0;
}


static int UTIL_MigrMgr_CtrlranIDGet(session, ranID)
     I32s session, ranID;
{
  int i, test;

  /* look for the session into the buffer */
  for (i = ranIDused - 1 ; (i >= 0)&&(ranIDlist[i].session - session) ; i--);

  if (i >= 0){
    test = (ranIDlist[i].ranID == ranID);
    if (test)
      ranIDlist[i].session = 0;
    return test;
  }
  else
    return 0;
}


static void ranIDgarbagecollector()
{
  int lastintogarbage;
  int i, wasted = 0;

  while (wasted < MAX_RANID_GAR){
    lastintogarbage = ranIDfirsttime + MAX_RANID_GAR;
    for(i = 0 ; i < ranIDused ; i++)
      if (ranIDlist[i].migrtime < lastintogarbage)
	wasted++, ranIDlist[i].session = 0;
    ranIDfirsttime = lastintogarbage;
  }
}


/*********************************************************************************/
/* Migration port
 */

/* Initialize the value of the migration port */
void UTIL_MigrMgr_PortChange()
{
  int portnb;

  sscanf(MigrPort, "%d", &portnb);
  portnb++;
  tsprintf(MigrPort, "%d", portnb);
}


/* return the value of the migration port */
char *UTIL_MigrMgr_PortGet()
{
  return MigrPort;
}
